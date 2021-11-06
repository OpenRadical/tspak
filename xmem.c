#include <stdlib.h>

#include "xmem.h"

 /* - - - - */

void* xcalloc(size_t count, size_t size)
{
    void* p;

    if (count == 0 || size == 0)
        abort();

    p = calloc(count, size);
    if (p == NULL)
        abort();
    return p;
}

void* xmalloc(size_t size)
{
    void* p;

    if (size == 0)
        abort();

    p = malloc(size);
    if (p == NULL)
        abort();
    return p;
}

void xfree(void* ptr)
{
    if (ptr == NULL)
        return;
    free(ptr);
}

void* xrealloc(void* ptr, size_t size)
{
    void* p;

    if (ptr == NULL || size == 0)
        abort();

    p = realloc(ptr, size);
    if (p == NULL)
        abort();
    return p;
}
