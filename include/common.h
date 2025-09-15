#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <assert.h>

/* Error codes */
typedef enum {
    LMS_SUCCESS = 0,
    LMS_ERROR_NULL_POINTER,
    LMS_ERROR_MEMORY,
    LMS_ERROR_NOT_FOUND,
    LMS_ERROR_DUPLICATE,
    LMS_ERROR_INVALID_INPUT,
    LMS_ERROR_LOAN_LIMIT,
    LMS_ERROR_BOOK_UNAVAILABLE,
    LMS_ERROR_FILE_IO,
    LMS_ERROR_PERMISSION_DENIED,
    LMS_ERROR_SYSTEM
} LMS_Result;

/* Log levels */
typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
} LogLevel;

/* Utility macros */
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/* Memory safety macros */
#define SAFE_FREE(ptr) do { \
    if ((ptr) != NULL) { \
        free(ptr); \
        (ptr) = NULL; \
    } \
} while(0)

#define CHECK_NULL(ptr) do { \
    if ((ptr) == NULL) { \
        printf("ERROR: NULL pointer at %s:%d\n", __FUNCTION__, __LINE__); \
        return LMS_ERROR_NULL_POINTER; \
    } \
} while(0)

#define CHECK_NULL_RETURN_NULL(ptr) do { \
    if ((ptr) == NULL) { \
        printf("ERROR: NULL pointer at %s:%d\n", __FUNCTION__, __LINE__); \
        return NULL; \
    } \
} while(0)

/* Function pointer types */
typedef int (*CompareFunc)(const void *a, const void *b);
typedef void (*PrintFunc)(const void *data);
typedef bool (*ConditionFunc)(const void *data, void *context);
typedef void (*FreeFunc)(void *data);
typedef void* (*CopyFunc)(const void *data);

/* Error handling */
const char* lms_get_error_string(LMS_Result error);
void lms_log_error(LMS_Result error, const char *function, int line);

#endif /* COMMON_H */