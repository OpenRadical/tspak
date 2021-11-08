#include <search.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "xmem.h"

uint32_t CRCCOUNT;
uint32_t* P_CRCSUMS;
char* P_CRCSTRS;

int parseCrcFile(char* filename) {
    // Open additional CRC32 database
    FILE* p_crcFile;
    char* crcLine = xmalloc(1024);
    char* crcSum = &crcLine[0];
    char* crcName = &crcLine[12];

    if (!(p_crcFile = fopen(filename, "rb"))) {
        printf("Could not open CRC file: %s", filename);
        return 1;
    };

    CRCCOUNT = 0;
    for (char c = getc(p_crcFile); c != EOF; c = getc(p_crcFile))
        if (c == '\n') // Increment count if this character is newline
            CRCCOUNT++;
    fseek(p_crcFile, 0, SEEK_SET);

    P_CRCSUMS = (uint32_t*)xcalloc(CRCCOUNT, sizeof(uint32_t));
    P_CRCSTRS = (char*)xcalloc(CRCCOUNT, FILENAME_MAX);

    char* crcLeft = crcLine;
    for (uint32_t i = 0; i < CRCCOUNT; i++) {
        crcLeft = fgets(crcLine, 1024, p_crcFile);
        if (!crcLeft) {
            printf("Ran out of CRC entries!\n");
            return 1;
        }
        *strrchr(crcLine, '\n') = 0; // Terminate crcName
        crcLine[10] = 0;             // Terminate crcSum
        *(P_CRCSUMS + i) = (uint32_t)strtoul(crcSum, NULL, 0);
        strcpy(P_CRCSTRS + (FILENAME_MAX * i), crcName);
    }
    xfree(crcLine);
    fclose(p_crcFile);
    return 0;
}

int compare(uint32_t* a, uint32_t* b) {
    return *a - *b;
}

char* lookupCrc(uint32_t crc) {
    return (uint32_t*)bsearch(&crc, P_CRCSUMS, CRCCOUNT, sizeof(uint32_t), (int (*)(const void*, const void*))compare);
    uint32_t* p_crcCheck;
    uint32_t crcIndex = CRCCOUNT;
    p_crcCheck = P_CRCSUMS;
    while ((*p_crcCheck != crc) && (crcIndex > 1)) {
        if (crcIndex % 2 != 0) crcIndex++;
        crcIndex /= 2;
        if (*p_crcCheck > crc) {
            p_crcCheck -= crcIndex;
        }
        else if (*p_crcCheck < crc) {
            p_crcCheck += crcIndex;
        }
    }
    if (*p_crcCheck != crc) return 0; // Failed to find
    uint32_t offset = p_crcCheck - P_CRCSUMS;
    return P_CRCSTRS + (FILENAME_MAX * offset);
}