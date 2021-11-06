#ifndef __STR_HELPERS_H__
#define __STR_HELPERS_H__

#include <stdbool.h>

 /* - - - - */

bool str_isempty(const char* s);
bool str_istrue(const char* s);
const char* str_safe(const char* s);
char* str_strdup_safe(const char* s);
char** str_split(const char* in, size_t in_len, char delm, size_t* num_elm, size_t max);
void str_split_free(char** in, size_t num_elm);

#endif /* __STR_HELPERS_H__ */
