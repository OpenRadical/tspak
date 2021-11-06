#ifndef __XMEM_H__
#define __XMEM_H__

#include <stddef.h>

 /* - - - - */

void* xcalloc(size_t count, size_t size);
void* xmalloc(size_t size);
void* xrealloc(void* ptr, size_t size);
void xfree(void* ptr);

#endif /* __XMEM_H__ */
