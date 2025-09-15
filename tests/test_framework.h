#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include "../include/common.h"

/* Test result structure */
typedef struct TestResult {
    bool passed;
    char error_message[500];
} TestResult;

/* Test case structure */
typedef struct TestCase {
    char name[100];
    TestResult (*test_function)(void);
} TestCase;

/* Test suite structure */
typedef struct TestSuite {
    char name[100];
    TestCase *test_cases;
    int test_count;
    int max_tests;
    int passed_count;
    int failed_count;
} TestSuite;

/* Test assertion macros */
#define TEST_ASSERT(condition, message) do { \
    if (!(condition)) { \
        TestResult result = {false, ""}; \
        snprintf(result.error_message, sizeof(result.error_message), \
                 "ASSERTION FAILED at %s:%d - %s", __FILE__, __LINE__, message); \
        return result; \
    } \
} while(0)

#define TEST_ASSERT_EQUAL_INT(expected, actual) do { \
    if ((expected) != (actual)) { \
        TestResult result = {false, ""}; \
        snprintf(result.error_message, sizeof(result.error_message), \
                 "ASSERTION FAILED at %s:%d - Expected %d, got %d", \
                 __FILE__, __LINE__, (expected), (actual)); \
        return result; \
    } \
} while(0)

#define TEST_ASSERT_EQUAL_STRING(expected, actual) do { \
    if (strcmp((expected), (actual)) != 0) { \
        TestResult result = {false, ""}; \
        snprintf(result.error_message, sizeof(result.error_message), \
                 "ASSERTION FAILED at %s:%d - Expected '%s', got '%s'", \
                 __FILE__, __LINE__, (expected), (actual)); \
        return result; \
    } \
} while(0)

#define TEST_ASSERT_NOT_NULL(pointer) do { \
    if ((pointer) == NULL) { \
        TestResult result = {false, ""}; \
        snprintf(result.error_message, sizeof(result.error_message), \
                 "ASSERTION FAILED at %s:%d - Pointer is NULL", \
                 __FILE__, __LINE__); \
        return result; \
    } \
} while(0)

#define TEST_ASSERT_NULL(pointer) do { \
    if ((pointer) != NULL) { \
        TestResult result = {false, ""}; \
        snprintf(result.error_message, sizeof(result.error_message), \
                 "ASSERTION FAILED at %s:%d - Pointer is not NULL", \
                 __FILE__, __LINE__); \
        return result; \
    } \
} while(0)

#define TEST_SUCCESS() do { \
    TestResult result = {true, ""}; \
    return result; \
} while(0)

/* Test suite management */
TestSuite* test_suite_create(const char *name, int max_tests);
void test_suite_destroy(TestSuite *suite);
void test_suite_add_test(TestSuite *suite, const char *name, TestResult (*test_function)(void));
void test_suite_run(TestSuite *suite);
void test_suite_print_results(TestSuite *suite);

/* Global test statistics */
extern int total_tests_run;
extern int total_tests_passed;
extern int total_tests_failed;

/* Test runner functions */
void run_all_tests(void);
void print_test_summary(void);

/* Individual test function declarations */
TestResult test_dll_create_destroy(void);
TestResult test_dll_insert_operations(void);
TestResult test_dll_delete_operations(void);
TestResult test_dll_search_operations(void);
TestResult test_dll_sort_operations(void);
TestResult test_dll_iterator(void);

TestResult test_book_validation(void);
TestResult test_member_validation(void);
TestResult test_loan_validation(void);

TestResult test_book_repository_crud(void);
TestResult test_member_repository_crud(void);
TestResult test_loan_repository_crud(void);

TestResult test_book_service_operations(void);
TestResult test_member_service_operations(void);
TestResult test_loan_service_operations(void);

#endif /* TEST_FRAMEWORK_H */