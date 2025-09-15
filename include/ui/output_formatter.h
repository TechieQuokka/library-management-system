#ifndef OUTPUT_FORMATTER_H
#define OUTPUT_FORMATTER_H

#include "../common.h"
#include "../models/models.h"
#include "../core/doubly_linked_list.h"

/* Message types */
typedef enum {
    MSG_TYPE_INFO,
    MSG_TYPE_SUCCESS,
    MSG_TYPE_WARNING,
    MSG_TYPE_ERROR
} MessageType;

/* Output formatter structure */
typedef struct OutputFormatter {
    int page_size;
    int column_width;
    bool use_colors;
} OutputFormatter;

/* Output formatter management */
OutputFormatter* output_formatter_create(void);
void output_formatter_destroy(OutputFormatter *formatter);

/* Basic output functions */
void output_print_header(OutputFormatter *formatter, const char *title);
void output_print_separator(OutputFormatter *formatter);
void output_print_message(OutputFormatter *formatter, const char *message, MessageType type);
void output_print_error(OutputFormatter *formatter, const char *error);
void output_print_success(OutputFormatter *formatter, const char *message);

/* Data output functions */
void output_print_book(OutputFormatter *formatter, const Book *book);
void output_print_member(OutputFormatter *formatter, const Member *member);
void output_print_loan(OutputFormatter *formatter, const Loan *loan);

/* List output functions */
void output_print_book_list(OutputFormatter *formatter, const DoublyLinkedList *books);
void output_print_member_list(OutputFormatter *formatter, const DoublyLinkedList *members);
void output_print_loan_list(OutputFormatter *formatter, const DoublyLinkedList *loans);

/* Table output functions */
void output_print_book_table(OutputFormatter *formatter, const DoublyLinkedList *books);
void output_print_member_table(OutputFormatter *formatter, const DoublyLinkedList *members);
void output_print_loan_table(OutputFormatter *formatter, const DoublyLinkedList *loans);

/* Paged output functions */
void output_print_with_paging(OutputFormatter *formatter, const DoublyLinkedList *list,
                              void (*print_item)(OutputFormatter *formatter, const void *item));

/* Statistics output */
void output_print_statistics(OutputFormatter *formatter, int total_books, int available_books,
                            int total_members, int active_members, int total_loans, int active_loans);

/* Configuration functions */
void output_set_page_size(OutputFormatter *formatter, int size);
void output_set_column_width(OutputFormatter *formatter, int width);
void output_set_use_colors(OutputFormatter *formatter, bool use_colors);

/* Utility functions */
void output_clear_screen(OutputFormatter *formatter);
void output_pause(OutputFormatter *formatter);

#endif /* OUTPUT_FORMATTER_H */