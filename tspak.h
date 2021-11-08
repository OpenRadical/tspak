#ifndef __TSPAK_H__
#define __TSPAK_H__

#include <stdint.h>

// P4CK (TS1/2 PS2), P5CK (TS3 All)
typedef struct {
    char format[4];
    uint32_t indexOffset;
    uint32_t indexLength;
} Header;

// P8CK (TS2 GC/Xbox custom)
typedef struct {
    char format[4];
    uint32_t indexOffset;
    uint32_t entryCount;
    uint32_t namesOffset;
} Header2;

// P4CK (TS1/2 PS2)
typedef struct {
    char name[48];
    uint32_t offset;
    uint32_t length;
    uint32_t unknown;
} Entry4;

// P5CK (TS3)
typedef struct {
    uint32_t crc;
    uint32_t offset;
    uint32_t length;
    uint32_t gzLength;
} Entry5;

// P8CK (TS2 GC/XBox)
typedef struct {
    uint32_t nameOffset;
    uint32_t length;
    uint32_t offset;
} Entry8;

/*! Extracts a pak file to a given path.
 *
 * \param[in] p_pakFile A pointer to an open file handle of a PAK file.
 * \param[in] basePath  The path to extract files to.
 *
 * \return 0 on success, 1 on failure.
 */
int extract(FILE* p_pakFile, char* basePath);

#endif /* __TSPAK_H__ */