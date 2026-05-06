#ifndef _ERRORS_H_
#define _ERRORS_H_

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define ERR_MSG_ALLOC   "Failed allocating memory: expected size %zu.\n"
#define ERR_MSG_ARGC    "Unrecognized args count: expected 3, found %d.\n"
#define ERR_MSG_STRDUP  "Failed duplicating string: %p.\n"
#define ERR_MSG_FCLOSE  "Failed closing file: %s.\n"
#define ERR_MSG_FOPEN   "Failed opening file: %s.\n"
#define ERR_MSG_FWRITE  "Failed writing file: %s.\n"
#define ERR_MSG_SYNTAX  "Expected %s, found %s.\n"

void err(const char *const format, ...)
{
    va_list args;
    va_start(args, format);
    while (vfprintf(stderr, format, args) == EOF);
    va_end(args);
}

void *alloc(const size_t length, const size_t element, const bool clear)
{
    void *p = (clear ? calloc(length, element) : malloc(length * element));
    if (!p)
    {
        err(ERR_MSG_ALLOC, length * element);
        return nullptr;
    }
    return p;
}

bool print(FILE *const file, const char *const format, ...)
{
    va_list args;
    va_start(args, format);
    bool t = false;
    while (vfprintf(file, format, args) == EOF)
    {
        t = true;
    }
    va_end(args);
    return t;
}

/*
    Different method value for different reserve method.
    method = false  =>  ++cap;
    method = true   =>  cap <<= 1;
*/
bool reserve(void **memory, const size_t size, size_t *const cap, const size_t element, const bool method)
{
    if (size != *cap)
    {
        return false;
    }
    if (method)
    {
        *cap <<= 1;
    }
    else
    {
        ++*cap;
    }
    *memory = realloc(*memory, *cap * element);
    if (!*memory)
    {
        err(ERR_MSG_ALLOC, *cap * element);
        return true;
    }
    return false;
}

#endif
