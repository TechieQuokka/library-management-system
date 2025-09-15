#include "test_framework.h"

/* Global test statistics */
int total_tests_run = 0;
int total_tests_passed = 0;
int total_tests_failed = 0;

/* Create a new test suite */
TestSuite* test_suite_create(const char *name, int max_tests) {
    if (!name || max_tests <= 0) return NULL;

    TestSuite *suite = malloc(sizeof(TestSuite));
    if (!suite) return NULL;

    suite->test_cases = malloc(sizeof(TestCase) * max_tests);
    if (!suite->test_cases) {
        free(suite);
        return NULL;
    }

    strncpy(suite->name, name, sizeof(suite->name) - 1);
    suite->name[sizeof(suite->name) - 1] = '\0';
    suite->test_count = 0;
    suite->max_tests = max_tests;
    suite->passed_count = 0;
    suite->failed_count = 0;

    return suite;
}

/* Destroy a test suite */
void test_suite_destroy(TestSuite *suite) {
    if (!suite) return;

    free(suite->test_cases);
    free(suite);
}

/* Add a test to the suite */
void test_suite_add_test(TestSuite *suite, const char *name, TestResult (*test_function)(void)) {
    if (!suite || !name || !test_function || suite->test_count >= suite->max_tests) {
        return;
    }

    TestCase *test = &suite->test_cases[suite->test_count];
    strncpy(test->name, name, sizeof(test->name) - 1);
    test->name[sizeof(test->name) - 1] = '\0';
    test->test_function = test_function;

    suite->test_count++;
}

/* Run all tests in the suite */
void test_suite_run(TestSuite *suite) {
    if (!suite) return;

    printf("\n================================================\n");
    printf("Running Test Suite: %s\n", suite->name);
    printf("================================================\n");

    suite->passed_count = 0;
    suite->failed_count = 0;

    for (int i = 0; i < suite->test_count; i++) {
        TestCase *test = &suite->test_cases[i];
        printf("Running: %s... ", test->name);
        fflush(stdout);

        TestResult result = test->test_function();

        total_tests_run++;

        if (result.passed) {
            printf("PASS\n");
            suite->passed_count++;
            total_tests_passed++;
        } else {
            printf("FAIL\n");
            printf("  Error: %s\n", result.error_message);
            suite->failed_count++;
            total_tests_failed++;
        }
    }
}

/* Print test suite results */
void test_suite_print_results(TestSuite *suite) {
    if (!suite) return;

    printf("\n================================================\n");
    printf("Test Suite: %s - Results\n", suite->name);
    printf("================================================\n");
    printf("Total Tests: %d\n", suite->test_count);
    printf("Passed: %d\n", suite->passed_count);
    printf("Failed: %d\n", suite->failed_count);
    printf("Success Rate: %.1f%%\n",
           suite->test_count > 0 ? (suite->passed_count * 100.0 / suite->test_count) : 0.0);
    printf("================================================\n");
}

/* Run all tests */
void run_all_tests(void) {
    printf("Starting Library Management System Test Suite\n");
    printf("==============================================\n");

    /* Reset global counters */
    total_tests_run = 0;
    total_tests_passed = 0;
    total_tests_failed = 0;

    /* Core Data Structure Tests */
    TestSuite *dll_suite = test_suite_create("Doubly Linked List Tests", 10);
    if (dll_suite) {
        test_suite_add_test(dll_suite, "Create and Destroy", test_dll_create_destroy);
        test_suite_add_test(dll_suite, "Insert Operations", test_dll_insert_operations);
        test_suite_add_test(dll_suite, "Delete Operations", test_dll_delete_operations);
        test_suite_add_test(dll_suite, "Search Operations", test_dll_search_operations);
        test_suite_add_test(dll_suite, "Sort Operations", test_dll_sort_operations);
        test_suite_add_test(dll_suite, "Iterator", test_dll_iterator);

        test_suite_run(dll_suite);
        test_suite_print_results(dll_suite);
        test_suite_destroy(dll_suite);
    }

    /* Model Validation Tests */
    TestSuite *model_suite = test_suite_create("Model Validation Tests", 5);
    if (model_suite) {
        test_suite_add_test(model_suite, "Book Validation", test_book_validation);
        test_suite_add_test(model_suite, "Member Validation", test_member_validation);
        test_suite_add_test(model_suite, "Loan Validation", test_loan_validation);

        test_suite_run(model_suite);
        test_suite_print_results(model_suite);
        test_suite_destroy(model_suite);
    }

    /* Repository Tests */
    TestSuite *repo_suite = test_suite_create("Repository Tests", 5);
    if (repo_suite) {
        test_suite_add_test(repo_suite, "Book Repository CRUD", test_book_repository_crud);
        test_suite_add_test(repo_suite, "Member Repository CRUD", test_member_repository_crud);
        test_suite_add_test(repo_suite, "Loan Repository CRUD", test_loan_repository_crud);

        test_suite_run(repo_suite);
        test_suite_print_results(repo_suite);
        test_suite_destroy(repo_suite);
    }

    /* Service Tests */
    TestSuite *service_suite = test_suite_create("Service Tests", 5);
    if (service_suite) {
        test_suite_add_test(service_suite, "Book Service Operations", test_book_service_operations);
        test_suite_add_test(service_suite, "Member Service Operations", test_member_service_operations);
        test_suite_add_test(service_suite, "Loan Service Operations", test_loan_service_operations);

        test_suite_run(service_suite);
        test_suite_print_results(service_suite);
        test_suite_destroy(service_suite);
    }

    /* Print overall summary */
    print_test_summary();
}

/* Print test summary */
void print_test_summary(void) {
    printf("\n");
    printf("========================================\n");
    printf("           TEST SUMMARY\n");
    printf("========================================\n");
    printf("Total Tests Run: %d\n", total_tests_run);
    printf("Tests Passed: %d\n", total_tests_passed);
    printf("Tests Failed: %d\n", total_tests_failed);
    printf("Overall Success Rate: %.1f%%\n",
           total_tests_run > 0 ? (total_tests_passed * 100.0 / total_tests_run) : 0.0);
    printf("========================================\n");

    if (total_tests_failed == 0) {
        printf("All tests passed! Great job!\n");
    } else {
        printf("Some tests failed. Please review the errors above.\n");
    }
}