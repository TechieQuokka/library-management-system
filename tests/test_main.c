#include "test_framework.h"

int main(void) {
    printf("Library Management System - Test Runner\n");
    printf("=======================================\n");

    run_all_tests();

    return (total_tests_failed == 0) ? 0 : 1;
}