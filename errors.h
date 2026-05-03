#ifndef _ERRORS_H_
#define _ERRORS_H_

#define ERR_MSG_ALLOC   "Failed allocating memory: expected size %d.\n"
#define ERR_MSG_ARGC    "Unrecognized args count: expected 3, found %d.\n"
#define ERR_MSG_FCLOSE  "Failed closing file: %s.\n"
#define ERR_MSG_FOPEN   "Failed opening file: %s.\n"
#define ERR_MSG_FWRITE  "Failed writing file: %s.\n"
#define ERR_MSG_SYNTAX  "Expected %s, found %s.\n"

#define ERR(format, ...) do \
{ \
    while (fprintf(stderr, format __VA_OPT__(,) __VA_ARGS__) == EOF) \
    { \
    } \
} \
while (0) \

#endif
