#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER 
 //not #if defined(_WIN32) || defined(_WIN64) because we have strncasecmp in mingw
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

#include "str_helpers.h"

 /* - - - - */

bool str_isempty(const char* s)
{
    if (s == NULL || *s == '\0')
        return true;
    return false;
}

bool str_istrue(const char* s)
{
    if (str_isempty(s))
        return false;

    if (strcasecmp(s, "true") == 0 || strcasecmp(s, "t") == 0 ||
        strcasecmp(s, "yes") == 0 || strcasecmp(s, "y") == 0 ||
        strcasecmp(s, "on") == 0 || strcasecmp(s, "o") == 0 ||
        strcasecmp(s, "1") == 0 || strcasecmp(s, "+") == 0)
    {
        return true;
    }
    return false;
}

const char* str_safe(const char* s)
{
    if (s == NULL)
        return "";
    return s;
}

char* str_strdup_safe(const char* s)
{
    return strdup(str_safe(s));
}

char** str_split(const char* in, size_t in_len, char delm, size_t* num_elm, size_t max)
{
    char* parsestr;
    char** out;
    size_t  cnt = 1;
    size_t  i;

    if (in == NULL || in_len == 0 || num_elm == NULL)
        return NULL;

    parsestr = malloc(in_len + 1);
    memcpy(parsestr, in, in_len + 1);
    parsestr[in_len] = '\0';

    *num_elm = 1;
    for (i = 0; i < in_len; i++) {
        if (parsestr[i] == delm)
            (*num_elm)++;
        if (max > 0 && *num_elm == max)
            break;
    }

    out = malloc(*num_elm * sizeof(*out));
    out[0] = parsestr;
    for (i = 0; i < in_len && cnt < *num_elm; i++) {
        if (parsestr[i] != delm)
            continue;

        /* Add the pointer to the array of elements */
        parsestr[i] = '\0';
        out[cnt] = parsestr + i + 1;
        cnt++;
    }

    return out;
}

void str_split_free(char** in, size_t num_elm)
{
    if (in == NULL)
        return;
    if (num_elm != 0)
        free(in[0]);
    free(in);
}
