#ifndef ERROR_TYPES_H
#define ERROR_TYPES_H

#define printfRed(s, ...) printf("\033[0;31m" s "\033[1;0m", ## __VA_ARGS__)

#define printErr(s, ...) fprintf(stderr, "\033[0;31m" s "\033[1;0m", ## __VA_ARGS__)


/* #define printErr(s, a...) \
{ \
    fprintf(stderr, "\033[0;31mError in file: %s;\nFunction: %s; line: %d\033[1;0m\n", __FILE__, __FUNCTION__, __LINE__); \
    fprintf(stderr, "\033[0;31m" s "\033[1;0m", ## a); \
} \ */


enum stackErrorType{
    NO_ERROR = 0,
    INITIALISATION_ERROR = 1,
    RESIZE_ERROR = 2,
    HASH_ERROR = 3,
    REINITIALISATION_ERROR = 4,
    DESTRUCTION_ERROR = 5,
    EMPTY_ERROR = 6,
    UNINITIALIZED_STACK_ERROR = 7,
    CANARY_ERROR = 8,
    CAPACITY_ERROR = 9,
    NULLPTR_ERROR = 10,
    STRUCT_HASH_ERROR = 11
};

#endif