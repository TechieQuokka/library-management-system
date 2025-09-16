#include "../../include/core/doubly_linked_list.h"

/* Internal helper function to create a node */
Node* node_create(const void *data, size_t data_size) {
    Node *node = malloc(sizeof(Node));
    if (!node) return NULL;

    node->data = malloc(data_size);
    if (!node->data) {
        free(node);
        return NULL;
    }

    memcpy(node->data, data, data_size);
    node->prev = NULL;
    node->next = NULL;

    return node;
}

/* Internal helper function to destroy a node */
void node_destroy(Node *node, FreeFunc free_data) {
    if (!node) return;

    if (node->data) {
        if (free_data) {
            free_data(node->data);
        } else {
            free(node->data);
        }
    }
    free(node);
}

/* Create a new doubly linked list */
DoublyLinkedList* dll_create(size_t data_size, CompareFunc compare, PrintFunc print) {
    DoublyLinkedList *list = malloc(sizeof(DoublyLinkedList));
    if (!list) return NULL;

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    list->data_size = data_size;
    list->compare = compare;
    list->print = print;
    list->free_data = NULL;
    list->copy_data = NULL;

    return list;
}

/* Initialize an existing list */
LMS_Result dll_init(DoublyLinkedList *list, size_t data_size, CompareFunc compare, PrintFunc print) {
    CHECK_NULL(list);

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    list->data_size = data_size;
    list->compare = compare;
    list->print = print;
    list->free_data = NULL;
    list->copy_data = NULL;

    return LMS_SUCCESS;
}

/* Insert at the front of the list */
LMS_Result dll_insert_front(DoublyLinkedList *list, const void *data) {
    CHECK_NULL(list);
    CHECK_NULL(data);

    Node *new_node = node_create(data, list->data_size);
    if (!new_node) return LMS_ERROR_MEMORY;

    if (list->size == 0) {
        list->head = list->tail = new_node;
    } else {
        new_node->next = list->head;
        list->head->prev = new_node;
        list->head = new_node;
    }

    list->size++;
    return LMS_SUCCESS;
}

/* Insert at the rear of the list */
LMS_Result dll_insert_rear(DoublyLinkedList *list, const void *data) {
    CHECK_NULL(list);
    CHECK_NULL(data);

    Node *new_node = node_create(data, list->data_size);
    if (!new_node) return LMS_ERROR_MEMORY;

    if (list->size == 0) {
        list->head = list->tail = new_node;
    } else {
        new_node->prev = list->tail;
        list->tail->next = new_node;
        list->tail = new_node;
    }

    list->size++;
    return LMS_SUCCESS;
}

/* Insert at a specific index */
LMS_Result dll_insert_at(DoublyLinkedList *list, int index, const void *data) {
    CHECK_NULL(list);
    CHECK_NULL(data);

    if (index < 0 || index > list->size) {
        return LMS_ERROR_INVALID_INPUT;
    }

    if (index == 0) {
        return dll_insert_front(list, data);
    }

    if (index == list->size) {
        return dll_insert_rear(list, data);
    }

    Node *new_node = node_create(data, list->data_size);
    if (!new_node) return LMS_ERROR_MEMORY;

    Node *current = list->head;
    for (int i = 0; i < index; i++) {
        current = current->next;
    }

    new_node->next = current;
    new_node->prev = current->prev;
    current->prev->next = new_node;
    current->prev = new_node;

    list->size++;
    return LMS_SUCCESS;
}

/* Insert in sorted order */
LMS_Result dll_insert_sorted(DoublyLinkedList *list, const void *data) {
    CHECK_NULL(list);
    CHECK_NULL(data);

    if (!list->compare) {
        return LMS_ERROR_INVALID_INPUT;
    }

    if (list->size == 0) {
        return dll_insert_front(list, data);
    }

    Node *current = list->head;
    int index = 0;

    while (current && list->compare(data, current->data) > 0) {
        current = current->next;
        index++;
    }

    return dll_insert_at(list, index, data);
}

/* Delete from the front */
LMS_Result dll_delete_front(DoublyLinkedList *list) {
    CHECK_NULL(list);

    if (list->size == 0) {
        return LMS_ERROR_NOT_FOUND;
    }

    Node *to_delete = list->head;

    if (list->size == 1) {
        list->head = list->tail = NULL;
    } else {
        list->head = list->head->next;
        list->head->prev = NULL;
    }

    node_destroy(to_delete, list->free_data);
    list->size--;

    return LMS_SUCCESS;
}

/* Delete from the rear */
LMS_Result dll_delete_rear(DoublyLinkedList *list) {
    CHECK_NULL(list);

    if (list->size == 0) {
        return LMS_ERROR_NOT_FOUND;
    }

    Node *to_delete = list->tail;

    if (list->size == 1) {
        list->head = list->tail = NULL;
    } else {
        list->tail = list->tail->prev;
        list->tail->next = NULL;
    }

    node_destroy(to_delete, list->free_data);
    list->size--;

    return LMS_SUCCESS;
}

/* Delete at a specific index */
LMS_Result dll_delete_at(DoublyLinkedList *list, int index) {
    CHECK_NULL(list);

    if (index < 0 || index >= list->size) {
        return LMS_ERROR_INVALID_INPUT;
    }

    if (index == 0) {
        return dll_delete_front(list);
    }

    if (index == list->size - 1) {
        return dll_delete_rear(list);
    }

    Node *current = list->head;
    for (int i = 0; i < index; i++) {
        current = current->next;
    }

    return dll_delete_node(list, current);
}

/* Delete a specific node */
LMS_Result dll_delete_node(DoublyLinkedList *list, Node *node) {
    CHECK_NULL(list);
    CHECK_NULL(node);

    if (list->size == 0) {
        return LMS_ERROR_NOT_FOUND;
    }

    if (node == list->head && node == list->tail) {
        list->head = list->tail = NULL;
    } else if (node == list->head) {
        list->head = node->next;
        list->head->prev = NULL;
    } else if (node == list->tail) {
        list->tail = node->prev;
        list->tail->next = NULL;
    } else {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    node_destroy(node, list->free_data);
    list->size--;

    return LMS_SUCCESS;
}

/* Delete specific data */
LMS_Result dll_delete_data(DoublyLinkedList *list, const void *data) {
    CHECK_NULL(list);
    CHECK_NULL(data);

    Node *node = dll_search(list, data);
    if (!node) {
        return LMS_ERROR_NOT_FOUND;
    }

    return dll_delete_node(list, node);
}

/* Search for data */
Node* dll_search(DoublyLinkedList *list, const void *data) {
    if (!list || !data || !list->compare) return NULL;

    Node *current = list->head;
    while (current) {
        if (list->compare(current->data, data) == 0) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

/* Find with condition */
Node* dll_find_if(DoublyLinkedList *list, ConditionFunc condition, void *context) {
    if (!list || !condition) return NULL;

    Node *current = list->head;
    while (current) {
        if (condition(current->data, context)) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

/* Get node at index */
Node* dll_get_node_at(DoublyLinkedList *list, int index) {
    if (!list || index < 0 || index >= list->size) return NULL;

    Node *current;
    int i;

    if (index < list->size / 2) {
        current = list->head;
        for (i = 0; i < index; i++) {
            current = current->next;
        }
    } else {
        current = list->tail;
        for (i = list->size - 1; i > index; i--) {
            current = current->prev;
        }
    }

    return current;
}

/* Get data at index */
void* dll_get_at(DoublyLinkedList *list, int index) {
    Node *node = dll_get_node_at(list, index);
    return node ? node->data : NULL;
}

/* Get index of data */
int dll_index_of(DoublyLinkedList *list, const void *data) {
    if (!list || !data || !list->compare) return -1;

    Node *current = list->head;
    int index = 0;

    while (current) {
        if (list->compare(current->data, data) == 0) {
            return index;
        }
        current = current->next;
        index++;
    }

    return -1;
}

/* Get list size */
int dll_size(DoublyLinkedList *list) {
    return list ? list->size : -1;
}

/* Check if list is empty */
bool dll_is_empty(DoublyLinkedList *list) {
    return list ? (list->size == 0) : true;
}

/* Clear all elements */
void dll_clear(DoublyLinkedList *list) {
    if (!list) return;

    Node *current = list->head;
    while (current) {
        Node *next = current->next;
        node_destroy(current, list->free_data);
        current = next;
    }

    list->head = list->tail = NULL;
    list->size = 0;
}

/* Destroy the list */
void dll_destroy(DoublyLinkedList *list) {
    if (!list) return;

    dll_clear(list);
    free(list);
}

/* Reverse the list */
LMS_Result dll_reverse(DoublyLinkedList *list) {
    CHECK_NULL(list);

    if (list->size <= 1) return LMS_SUCCESS;

    Node *current = list->head;
    Node *temp = NULL;

    while (current) {
        temp = current->prev;
        current->prev = current->next;
        current->next = temp;
        current = current->prev;
    }

    if (temp) {
        list->head = temp->prev;
    }

    temp = list->head;
    list->head = list->tail;
    list->tail = temp;

    return LMS_SUCCESS;
}

/* Create iterator */
Iterator* dll_iterator_create(DoublyLinkedList *list) {
    if (!list) return NULL;

    Iterator *iter = malloc(sizeof(Iterator));
    if (!iter) return NULL;

    iter->list = list;
    iter->current = list->head;
    iter->direction = 1;

    return iter;
}

/* Create reverse iterator */
Iterator* dll_reverse_iterator_create(DoublyLinkedList *list) {
    if (!list) return NULL;

    Iterator *iter = malloc(sizeof(Iterator));
    if (!iter) return NULL;

    iter->list = list;
    iter->current = list->tail;
    iter->direction = -1;

    return iter;
}

/* Get next element */
void* iterator_next(Iterator *iter) {
    if (!iter || !iter->current) return NULL;

    void *data = iter->current->data;

    if (iter->direction == 1) {
        iter->current = iter->current->next;
    } else {
        iter->current = iter->current->prev;
    }

    return data;
}

/* Check if has next */
bool iterator_has_next(Iterator *iter) {
    return iter && iter->current != NULL;
}

/* Get current element */
void* iterator_current(Iterator *iter) {
    return (iter && iter->current) ? iter->current->data : NULL;
}

/* Reset iterator */
void iterator_reset(Iterator *iter) {
    if (!iter || !iter->list) return;

    if (iter->direction == 1) {
        iter->current = iter->list->head;
    } else {
        iter->current = iter->list->tail;
    }
}

/* Destroy iterator */
void iterator_destroy(Iterator *iter) {
    if (iter) {
        free(iter);
    }
}

/* For each operation */
void dll_for_each(DoublyLinkedList *list, void (*func)(void *data, void *context), void *context) {
    if (!list || !func) return;

    Node *current = list->head;
    while (current) {
        func(current->data, context);
        current = current->next;
    }
}

/* Clone a doubly linked list */
DoublyLinkedList* dll_clone(DoublyLinkedList *list) {
    if (!list) return NULL;

    DoublyLinkedList *clone = dll_create(list->data_size, list->compare, list->print);
    if (!clone) return NULL;

    clone->free_data = list->free_data;
    clone->copy_data = list->copy_data;

    Node *current = list->head;
    while (current) {
        if (dll_insert_rear(clone, current->data) != LMS_SUCCESS) {
            dll_destroy(clone);
            return NULL;
        }
        current = current->next;
    }

    return clone;
}

/* Helper function for merge sort */
Node* merge_sorted_nodes(Node *left, Node *right, CompareFunc compare) {
    if (!left) return right;
    if (!right) return left;

    Node *result = NULL;

    if (compare(left->data, right->data) <= 0) {
        result = left;
        result->next = merge_sorted_nodes(left->next, right, compare);
        if (result->next) result->next->prev = result;
    } else {
        result = right;
        result->next = merge_sorted_nodes(left, right->next, compare);
        if (result->next) result->next->prev = result;
    }

    result->prev = NULL;
    return result;
}

/* Helper function to split list for merge sort */
void split_list(Node *head, Node **front, Node **back) {
    Node *fast = head;
    Node *slow = head;
    Node *prev = NULL;

    while (fast && fast->next) {
        prev = slow;
        slow = slow->next;
        fast = fast->next->next;
    }

    *front = head;
    *back = slow;

    if (prev) {
        prev->next = NULL;
        if (slow) slow->prev = NULL;
    }
}

/* Recursive merge sort for nodes */
Node* merge_sort_nodes(Node *head, CompareFunc compare) {
    if (!head || !head->next) {
        return head;
    }

    Node *front = NULL;
    Node *back = NULL;

    split_list(head, &front, &back);

    front = merge_sort_nodes(front, compare);
    back = merge_sort_nodes(back, compare);

    return merge_sorted_nodes(front, back, compare);
}

/* Sort the doubly linked list using merge sort */
LMS_Result dll_sort(DoublyLinkedList *list) {
    if (!list || !list->compare) return LMS_ERROR_NULL_POINTER;
    if (list->size <= 1) return LMS_SUCCESS;

    list->head = merge_sort_nodes(list->head, list->compare);

    /* Fix tail pointer and prev/next pointers correctly */
    Node *current = list->head;
    if (current) {
        current->prev = NULL;

        while (current->next) {
            current->next->prev = current;
            current = current->next;
        }
        list->tail = current;
    }

    return LMS_SUCCESS;
}

/* Sort with custom compare function */
LMS_Result dll_sort_with(DoublyLinkedList *list, CompareFunc compare) {
    if (!list || !compare) return LMS_ERROR_NULL_POINTER;
    if (list->size <= 1) return LMS_SUCCESS;

    list->head = merge_sort_nodes(list->head, compare);

    /* Fix tail pointer and prev/next pointers correctly */
    Node *current = list->head;
    if (current) {
        current->prev = NULL;

        while (current->next) {
            current->next->prev = current;
            current = current->next;
        }
        list->tail = current;
    }

    return LMS_SUCCESS;
}