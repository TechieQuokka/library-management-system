#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

#include "../common.h"

/* Forward declarations */
typedef struct Node Node;
typedef struct DoublyLinkedList DoublyLinkedList;
typedef struct Iterator Iterator;

/* Node structure */
struct Node {
    void *data;
    Node *prev;
    Node *next;
};

/* Doubly Linked List structure */
struct DoublyLinkedList {
    Node *head;
    Node *tail;
    int size;
    size_t data_size;
    CompareFunc compare;
    PrintFunc print;
    FreeFunc free_data;
    CopyFunc copy_data;
};

/* Iterator structure */
struct Iterator {
    Node *current;
    DoublyLinkedList *list;
    int direction; /* 1: forward, -1: backward */
};

/* Core functions */
DoublyLinkedList* dll_create(size_t data_size, CompareFunc compare, PrintFunc print);
LMS_Result dll_init(DoublyLinkedList *list, size_t data_size, CompareFunc compare, PrintFunc print);

/* Insert operations */
LMS_Result dll_insert_front(DoublyLinkedList *list, const void *data);
LMS_Result dll_insert_rear(DoublyLinkedList *list, const void *data);
LMS_Result dll_insert_at(DoublyLinkedList *list, int index, const void *data);
LMS_Result dll_insert_sorted(DoublyLinkedList *list, const void *data);

/* Delete operations */
LMS_Result dll_delete_front(DoublyLinkedList *list);
LMS_Result dll_delete_rear(DoublyLinkedList *list);
LMS_Result dll_delete_at(DoublyLinkedList *list, int index);
LMS_Result dll_delete_data(DoublyLinkedList *list, const void *data);
LMS_Result dll_delete_node(DoublyLinkedList *list, Node *node);

/* Search operations */
Node* dll_search(DoublyLinkedList *list, const void *data);
Node* dll_find_if(DoublyLinkedList *list, ConditionFunc condition, void *context);
Node* dll_get_node_at(DoublyLinkedList *list, int index);
void* dll_get_at(DoublyLinkedList *list, int index);
int dll_index_of(DoublyLinkedList *list, const void *data);

/* Sort and manipulation */
LMS_Result dll_sort(DoublyLinkedList *list);
LMS_Result dll_sort_with(DoublyLinkedList *list, CompareFunc compare);
LMS_Result dll_reverse(DoublyLinkedList *list);
DoublyLinkedList* dll_clone(DoublyLinkedList *list);

/* Utility functions */
int dll_size(DoublyLinkedList *list);
bool dll_is_empty(DoublyLinkedList *list);
bool dll_is_sorted(DoublyLinkedList *list);
void dll_clear(DoublyLinkedList *list);
void dll_destroy(DoublyLinkedList *list);

/* Functional operations */
void dll_for_each(DoublyLinkedList *list, void (*func)(void *data, void *context), void *context);
DoublyLinkedList* dll_filter(DoublyLinkedList *list, ConditionFunc condition, void *context);

/* Iterator functions */
Iterator* dll_iterator_create(DoublyLinkedList *list);
Iterator* dll_reverse_iterator_create(DoublyLinkedList *list);
void* iterator_next(Iterator *iter);
void* iterator_prev(Iterator *iter);
bool iterator_has_next(Iterator *iter);
void* iterator_current(Iterator *iter);
void iterator_reset(Iterator *iter);
void iterator_destroy(Iterator *iter);

/* Internal helper functions */
Node* node_create(const void *data, size_t data_size);
void node_destroy(Node *node, FreeFunc free_data);

#endif /* DOUBLY_LINKED_LIST_H */