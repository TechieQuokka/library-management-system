#include "../include/common.h"

/* Get error message string */
const char* lms_get_error_string(LMS_Result error) {
    switch (error) {
        case LMS_SUCCESS:
            return "Success";
        case LMS_ERROR_NULL_POINTER:
            return "Null pointer error";
        case LMS_ERROR_MEMORY:
            return "Memory allocation error";
        case LMS_ERROR_NOT_FOUND:
            return "Data not found";
        case LMS_ERROR_DUPLICATE:
            return "Duplicate data";
        case LMS_ERROR_INVALID_INPUT:
            return "Invalid input";
        case LMS_ERROR_LOAN_LIMIT:
            return "Loan limit exceeded";
        case LMS_ERROR_BOOK_UNAVAILABLE:
            return "Book unavailable for loan";
        case LMS_ERROR_FILE_IO:
            return "File I/O error";
        case LMS_ERROR_PERMISSION_DENIED:
            return "Permission denied";
        case LMS_ERROR_SYSTEM:
            return "System error";
        default:
            return "Unknown error";
    }
}

/* Log error with function and line information */
void lms_log_error(LMS_Result error, const char *function, int line) {
    printf("ERROR [%s:%d]: %s\n", function, line, lms_get_error_string(error));
}