#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xmem.h"
#include "rw_files.h"
#include "c2n.h"
#include "tspak.h"

// tspak
// extract a TimeSplitters PAK file.

char VER4[] = "TS1/2 PS2";
char VER5[] = "TS:FP";
char VER8[] = "TS2 GC/XBox";

void usage(char* name) {
    printf("Usage: %s data.pak [data2.pak...]\n", name);
};

void openerror(char* name) {
    printf("error opening %s\n", name);
};

int pakVersion(char* header) {
    if ((*header != 'P') | (*(header + 2) != 'C') | (*(header + 3) != 'K'))
        return -1;
    return *(header + 1) - 48;
};

int extract(FILE* p_pakFile, char* basePath) {

    char headerBuf[0x10] = { 0 }; // max 16 bytes
    char indexBuf[0x40]  = { 0 }; // max 64 bytes

    printf("Reading header... ");
    fread(headerBuf, sizeof(headerBuf), 1, p_pakFile);
    int version = pakVersion(headerBuf);
    if (version < 0) {
        printf("Not a TimeSplitters PAK file!\n");
        printf("If this is a HomeFront 2 pak file such as lsao_cached.pak\n");
        printf("please use something like hf2decrypt to extract the real TS2 data!\n");
        return 1;
    }

    // Working index data
    char* pakType;
    uint32_t indexOffset;
    uint8_t  indexSize;
    uint32_t entryCount;
    uint32_t namesOffset;

    switch (version) {
    // Index size
    case 4:
        pakType     = VER4;
        indexSize   = sizeof(Entry4);
        indexOffset = ((Header*)headerBuf)->indexOffset;
        entryCount  = ((Header*)headerBuf)->indexLength / indexSize;
        break;
    case 5:
        pakType     = VER5;
        indexSize   = sizeof(Entry5);
        indexOffset = ((Header*)headerBuf)->indexOffset;
        entryCount  = ((Header*)headerBuf)->indexLength / indexSize;
        break;
    case 8:
        pakType     = VER8;
        indexSize   = sizeof(Entry8);
        indexOffset = ((Header2*)headerBuf)->indexOffset;
        entryCount  = ((Header2*)headerBuf)->entryCount;
        namesOffset = ((Header2*)headerBuf)->namesOffset;
        break;
    default:
        printf("Unknown PAK version: %d\n", version);
        return 1;
    };
    printf("PAK version: %u (%s)\n", version, pakType);
    printf("No. of files: %u\n", entryCount);

    uint32_t entryOffset;
    uint32_t entryLength;
    uint32_t entryGzLength;
    uint32_t nameOffset;
    char     entryName[0x100];

    char*    p_entryBuf;
    uint32_t entryBufSize = 0x20000; // 128kB initial
    p_entryBuf = xmalloc(entryBufSize);

    // TS:FP specific CRC lookup
    FILE* p_crcFile;
    char crcLine[1024] = { 0 };
    char* crcSum = &crcLine[0];
    char* crcName = &crcLine[12];
    if (version == 5) {
        char crcFilename[FILENAME_MAX];
        sprintf(crcFilename, "%s.c2n", basePath);
        printf("Opening CRC file %s...", crcFilename);
        if (!(p_crcFile = fopen(crcFilename, "rb"))) {
            printf("error!\n");
            return 1;
        };
        printf("done!\n");

        parseCrcFile("db.c2n");
    }

    char* p_filePath;
    // Extract all entries
    while (entryCount > 0) {
        fseek(p_pakFile, indexOffset, SEEK_SET);
        fread(indexBuf, indexSize, 1, p_pakFile);
        switch (version) {
        case 4:
            entryOffset = ((Entry4*)indexBuf)->offset;
            entryLength = ((Entry4*)indexBuf)->length;
            // Name from index
            strcpy(entryName, ((Entry4*)indexBuf)->name);
            break;
        case 5:
            entryOffset = ((Entry5*)indexBuf)->offset;
            entryLength = ((Entry5*)indexBuf)->length;
            entryGzLength = ((Entry5*)indexBuf)->gzLength;
            // Name from crc lookup
            char* p_crcName = lookupCrc(((Entry5*)indexBuf)->crc);
            if (p_crcFile == NULL) {
                printf("CRC lookup failed!\n");
                return 1;
            }
            strcpy(entryName, p_crcName);
            //fgets(crcLine, 1024, p_crcFile);
            //*strrchr(crcLine, '\n') = 0; // Terminate crcName
            //crcLine[10] = 0;             // Terminate crcSum
            //if (((Entry5*)indexBuf)->crc != (uint32_t)strtoul(crcSum, NULL, 0)) {
            //    printf("CRC Error!\n");
            //    return 1;
            //}
            //strcpy(entryName, crcName);
            // Check if compressed
            if (entryGzLength > 0) {
                // Dump .gz file, for now
                entryLength = entryGzLength;
                strcat(entryName, ".gz");
            }
            break;
        case 8:
            entryOffset = ((Entry8*)indexBuf)->offset;
            entryLength = ((Entry8*)indexBuf)->length;
            nameOffset = ((Entry8*)indexBuf)->nameOffset;
            // Name from separate index
            fseek(p_pakFile, (namesOffset + nameOffset), SEEK_SET);
            fgets(entryName, sizeof(entryName), p_pakFile);
            break;
        };
        p_filePath = rw_join_path(2, basePath, entryName); // basePath + filePath
        rw_fix_path(p_filePath, strlen(p_filePath));  // Fix paths for system

        // Increase buffer size if necessary
        if (entryLength > entryBufSize) {
            while (entryBufSize < entryLength)
                entryBufSize *= 2;
            p_entryBuf = xrealloc(p_entryBuf, entryBufSize);
        };
        printf("Extracting %s...", p_filePath);
        fseek(p_pakFile, entryOffset, SEEK_SET);
        fread(p_entryBuf, entryLength, 1, p_pakFile);
        if ((rw_write_file(p_filePath, p_entryBuf, entryLength, false, true)) != entryLength) {
            printf("error!\n");
            return 1;
        };
        printf("done\n");

        // Next entry
        indexOffset += indexSize;
        entryCount--;
    };
    printf("\nFinished!");
    xfree(p_entryBuf);
    if (version == 5) fclose(p_crcFile);

    return 0;
};

int main(int argc, char* argv[]) {

    printf("\ntspak - Extract TimeSplitters PAK files.\nAuthor: stealthii\nSite: https://timesplitters.dev\n\n");

    // Usage help
    if (argc < 2) { usage(argv[0]); return 1; }

    // Extract all files passed as arguments
    FILE* p_pakFile;
    int ret = 0;
    char basePath[FILENAME_MAX] = { 0 };

    for (char** p_argv = argv + 1; *p_argv != argv[argc]; p_argv++) {
        // Basepath as pak name without extension
        strcpy(basePath, *p_argv);
        *strrchr(basePath, '.') = 0;
        // Open pak file
        printf("\nOpening file: %s\n", *p_argv);
        if (!(p_pakFile = fopen(*p_argv, "rb"))) { openerror(*p_argv); return 1; };

        ret += extract(p_pakFile, basePath); // Extract
        fclose(p_pakFile); // Close
    };

    return ret;
};
