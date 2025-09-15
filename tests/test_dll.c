#include "test_framework.h"
#include "../include/core/doubly_linked_list.h"

/* Helper functions for testing */
static int compare_int(const void *a, const void *b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

static void print_int(const void *data) {
    printf("%d", *(const int*)data);
}

/* Test DLL creation and destruction */
TestResult test_dll_create_destroy(void) {
    DoublyLinkedList *list = dll_create(sizeof(int), compare_int, print_int);
    TEST_ASSERT_NOT_NULL(list);
    TEST_ASSERT_EQUAL_INT(0, dll_size(list));
    TEST_ASSERT(dll_is_empty(list), "New list should be empty");

    dll_destroy(list);
    TEST_SUCCESS();
}

/* Test DLL insert operations */
TestResult test_dll_insert_operations(void) {
    DoublyLinkedList *list = dll_create(sizeof(int), compare_int, print_int);
    TEST_ASSERT_NOT_NULL(list);

    /* Test insert rear */
    int value1 = 10;
    LMS_Result result = dll_insert_rear(list, &value1);
    TEST_ASSERT_EQUAL_INT(LMS_SUCCESS, result);
    TEST_ASSERT_EQUAL_INT(1, dll_size(list));
    TEST_ASSERT(!dll_is_empty(list), "List should not be empty after insert");

    /* Test insert front */
    int value2 = 20;
    result = dll_insert_front(list, &value2);
    TEST_ASSERT_EQUAL_INT(LMS_SUCCESS, result);
    TEST_ASSERT_EQUAL_INT(2, dll_size(list));

    /* Test insert at specific position */
    int value3 = 15;
    result = dll_insert_at(list, 1, &value3);
    TEST_ASSERT_EQUAL_INT(LMS_SUCCESS, result);
    TEST_ASSERT_EQUAL_INT(3, dll_size(list));

    /* Verify order: 20, 15, 10 */
    int *data = (int*)dll_get_at(list, 0);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL_INT(20, *data);

    data = (int*)dll_get_at(list, 1);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL_INT(15, *data);

    data = (int*)dll_get_at(list, 2);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL_INT(10, *data);

    dll_destroy(list);
    TEST_SUCCESS();
}

/* Test DLL delete operations */
TestResult test_dll_delete_operations(void) {
    DoublyLinkedList *list = dll_create(sizeof(int), compare_int, print_int);
    TEST_ASSERT_NOT_NULL(list);

    /* Add some test data */
    int values[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) {
        dll_insert_rear(list, &values[i]);
    }
    TEST_ASSERT_EQUAL_INT(5, dll_size(list));

    /* Test delete front */
    LMS_Result result = dll_delete_front(list);
    TEST_ASSERT_EQUAL_INT(LMS_SUCCESS, result);
    TEST_ASSERT_EQUAL_INT(4, dll_size(list));

    /* Verify first element is now 20 */
    int *data = (int*)dll_get_at(list, 0);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL_INT(20, *data);

    /* Test delete rear */
    result = dll_delete_rear(list);
    TEST_ASSERT_EQUAL_INT(LMS_SUCCESS, result);
    TEST_ASSERT_EQUAL_INT(3, dll_size(list));

    /* Test delete at position */
    result = dll_delete_at(list, 1);
    TEST_ASSERT_EQUAL_INT(LMS_SUCCESS, result);
    TEST_ASSERT_EQUAL_INT(2, dll_size(list));

    /* Test delete specific data */
    int search_value = 40;
    result = dll_delete_data(list, &search_value);
    TEST_ASSERT_EQUAL_INT(LMS_SUCCESS, result);
    TEST_ASSERT_EQUAL_INT(1, dll_size(list));

    dll_destroy(list);
    TEST_SUCCESS();
}

/* Test DLL search operations */
TestResult test_dll_search_operations(void) {
    DoublyLinkedList *list = dll_create(sizeof(int), compare_int, print_int);
    TEST_ASSERT_NOT_NULL(list);

    /* Add test data */
    int values[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) {
        dll_insert_rear(list, &values[i]);
    }

    /* Test search existing value */
    int search_value = 30;
    Node *found = dll_search(list, &search_value);
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL_INT(30, *(int*)found->data);

    /* Test search non-existing value */
    search_value = 99;
    found = dll_search(list, &search_value);
    TEST_ASSERT_NULL(found);

    /* Test get at index */
    int *data = (int*)dll_get_at(list, 2);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL_INT(30, *data);

    /* Test index of */
    search_value = 40;
    int index = dll_index_of(list, &search_value);
    TEST_ASSERT_EQUAL_INT(3, index);

    dll_destroy(list);
    TEST_SUCCESS();
}

/* Test DLL sort operations */
TestResult test_dll_sort_operations(void) {
    DoublyLinkedList *list = dll_create(sizeof(int), compare_int, print_int);
    TEST_ASSERT_NOT_NULL(list);

    /* Add unsorted data */
    int values[] = {50, 10, 30, 20, 40};
    for (int i = 0; i < 5; i++) {
        dll_insert_rear(list, &values[i]);
    }

    /* Sort the list */
    LMS_Result result = dll_sort(list);
    TEST_ASSERT_EQUAL_INT(LMS_SUCCESS, result);

    /* Verify sorted order */
    int expected[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) {
        int *data = (int*)dll_get_at(list, i);
        TEST_ASSERT_NOT_NULL(data);
        TEST_ASSERT_EQUAL_INT(expected[i], *data);
    }

    /* Test sorted insertion */
    int new_value = 25;
    result = dll_insert_sorted(list, &new_value);
    TEST_ASSERT_EQUAL_INT(LMS_SUCCESS, result);
    TEST_ASSERT_EQUAL_INT(6, dll_size(list));

    /* Verify insertion position */
    int *data = (int*)dll_get_at(list, 2);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL_INT(25, *data);

    dll_destroy(list);
    TEST_SUCCESS();
}

/* Test DLL iterator */
TestResult test_dll_iterator(void) {
    DoublyLinkedList *list = dll_create(sizeof(int), compare_int, print_int);
    TEST_ASSERT_NOT_NULL(list);

    /* Add test data */
    int values[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) {
        dll_insert_rear(list, &values[i]);
    }

    /* Test forward iterator */
    Iterator *iter = dll_iterator_create(list);
    TEST_ASSERT_NOT_NULL(iter);

    int i = 0;
    while (iterator_has_next(iter)) {
        int *data = (int*)iterator_next(iter);
        TEST_ASSERT_NOT_NULL(data);
        TEST_ASSERT_EQUAL_INT(values[i], *data);
        i++;
    }
    TEST_ASSERT_EQUAL_INT(5, i);

    iterator_destroy(iter);

    /* Test reverse iterator */
    iter = dll_reverse_iterator_create(list);
    TEST_ASSERT_NOT_NULL(iter);

    i = 4;
    while (iterator_has_next(iter)) {
        int *data = (int*)iterator_next(iter);
        TEST_ASSERT_NOT_NULL(data);
        TEST_ASSERT_EQUAL_INT(values[i], *data);
        i--;
    }
    TEST_ASSERT_EQUAL_INT(-1, i);

    iterator_destroy(iter);
    dll_destroy(list);
    TEST_SUCCESS();
}