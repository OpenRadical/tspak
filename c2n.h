#ifndef __C2N_H__
#define __C2N_H__

#include <stdint.h>

int parseCrcFile(char* filename);
char* lookupCrc(uint32_t crc);

#endif /* __C2N_H__ */