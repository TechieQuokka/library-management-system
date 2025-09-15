#include "../../include/ui/output_formatter.h"

#define DEFAULT_PAGE_SIZE 20
#define DEFAULT_COLUMN_WIDTH 80

/* Create a new output formatter */
OutputFormatter* output_formatter_create(void) {
    OutputFormatter *formatter = malloc(sizeof(OutputFormatter));
    if (!formatter) return NULL;

    formatter->page_size = DEFAULT_PAGE_SIZE;
    formatter->column_width = DEFAULT_COLUMN_WIDTH;
    formatter->use_colors = false; /* Disabled for ASCII compatibility */

    return formatter;
}

/* Destroy the output formatter */
void output_formatter_destroy(OutputFormatter *formatter) {
    if (formatter) {
        free(formatter);
    }
}

/* Print header */
void output_print_header(OutputFormatter *formatter, const char *title) {
    if (!formatter || !title) return;

    int title_len = strlen(title);
    int padding = (formatter->column_width - title_len - 2) / 2;

    printf("\n");
    for (int i = 0; i < formatter->column_width; i++) {
        printf("=");
    }
    printf("\n");

    for (int i = 0; i < padding; i++) {
        printf(" ");
    }
    printf(" %s ", title);
    for (int i = 0; i < padding; i++) {
        printf(" ");
    }
    printf("\n");

    for (int i = 0; i < formatter->column_width; i++) {
        printf("=");
    }
    printf("\n");
}

/* Print separator */
void output_print_separator(OutputFormatter *formatter) {
    if (!formatter) return;

    for (int i = 0; i < formatter->column_width; i++) {
        printf("-");
    }
    printf("\n");
}

/* Print message with type */
void output_print_message(OutputFormatter *formatter, const char *message, MessageType type) {
    if (!formatter || !message) return;

    const char *prefix;
    switch (type) {
        case MSG_TYPE_INFO:
            prefix = "[INFO]";
            break;
        case MSG_TYPE_SUCCESS:
            prefix = "[SUCCESS]";
            break;
        case MSG_TYPE_WARNING:
            prefix = "[WARNING]";
            break;
        case MSG_TYPE_ERROR:
            prefix = "[ERROR]";
            break;
        default:
            prefix = "";
            break;
    }

    printf("%s %s\n", prefix, message);
}

/* Print error message */
void output_print_error(OutputFormatter *formatter, const char *error) {
    output_print_message(formatter, error, MSG_TYPE_ERROR);
}

/* Print success message */
void output_print_success(OutputFormatter *formatter, const char *message) {
    output_print_message(formatter, message, MSG_TYPE_SUCCESS);
}

/* Print book information */
void output_print_book(OutputFormatter *formatter, const Book *book) {
    if (!formatter || !book) return;

    printf("ISBN: %s\n", book->isbn);
    printf("Title: %s\n", book->title);
    printf("Author: %s\n", book->author);
    printf("Publisher: %s\n", book->publisher);
    printf("Year: %d\n", book->publication_year);
    printf("Category: %s\n", book->category);
    printf("Copies: %d (Available: %d)\n", book->total_copies, book->available_copies);
    printf("Price: $%.2f\n", book->price);
    printf("Status: %c\n", book->status);
    printf("\n");
}

/* Print member information */
void output_print_member(OutputFormatter *formatter, const Member *member) {
    if (!formatter || !member) return;

    printf("Member ID: %s\n", member->member_id);
    printf("Name: %s\n", member->name);
    printf("Phone: %s\n", member->phone);
    printf("Email: %s\n", member->email);
    printf("Address: %s\n", member->address);
    printf("Join Date: %s\n", member->join_date);
    printf("Type: %c\n", member->membership_type);
    printf("Active Loans: %d\n", member->loan_count);
    printf("Status: %c\n", member->status);
    printf("\n");
}

/* Print loan information */
void output_print_loan(OutputFormatter *formatter, const Loan *loan) {
    if (!formatter || !loan) return;

    printf("Loan ID: %s\n", loan->loan_id);
    printf("Member ID: %s\n", loan->member_id);
    printf("ISBN: %s\n", loan->isbn);
    printf("Loan Date: %s\n", loan->loan_date);
    printf("Due Date: %s\n", loan->due_date);
    printf("Return Date: %s\n", strlen(loan->return_date) > 0 ? loan->return_date : "Not returned");
    printf("Overdue Days: %d\n", loan->overdue_days);
    printf("Fine Amount: $%.2f\n", loan->fine_amount);
    printf("Status: %c\n", loan->status);
    printf("\n");
}

/* Print book list */
void output_print_book_list(OutputFormatter *formatter, const DoublyLinkedList *books) {
    if (!formatter || !books) return;

    if (dll_is_empty((DoublyLinkedList*)books)) {
        output_print_message(formatter, "No books found.", MSG_TYPE_INFO);
        return;
    }

    output_print_header(formatter, "Book List");
    printf("Total books: %d\n\n", dll_size((DoublyLinkedList*)books));

    Iterator *iter = dll_iterator_create((DoublyLinkedList*)books);
    if (!iter) return;

    int count = 0;
    while (iterator_has_next(iter)) {
        Book *book = (Book*)iterator_next(iter);
        printf("%d. ", ++count);
        output_print_book(formatter, book);

        if (count % formatter->page_size == 0) {
            output_pause(formatter);
        }
    }

    iterator_destroy(iter);
}

/* Print member list */
void output_print_member_list(OutputFormatter *formatter, const DoublyLinkedList *members) {
    if (!formatter || !members) return;

    if (dll_is_empty((DoublyLinkedList*)members)) {
        output_print_message(formatter, "No members found.", MSG_TYPE_INFO);
        return;
    }

    output_print_header(formatter, "Member List");
    printf("Total members: %d\n\n", dll_size((DoublyLinkedList*)members));

    Iterator *iter = dll_iterator_create((DoublyLinkedList*)members);
    if (!iter) return;

    int count = 0;
    while (iterator_has_next(iter)) {
        Member *member = (Member*)iterator_next(iter);
        printf("%d. ", ++count);
        output_print_member(formatter, member);

        if (count % formatter->page_size == 0) {
            output_pause(formatter);
        }
    }

    iterator_destroy(iter);
}

/* Print loan list */
void output_print_loan_list(OutputFormatter *formatter, const DoublyLinkedList *loans) {
    if (!formatter || !loans) return;

    if (dll_is_empty((DoublyLinkedList*)loans)) {
        output_print_message(formatter, "No loans found.", MSG_TYPE_INFO);
        return;
    }

    output_print_header(formatter, "Loan List");
    printf("Total loans: %d\n\n", dll_size((DoublyLinkedList*)loans));

    Iterator *iter = dll_iterator_create((DoublyLinkedList*)loans);
    if (!iter) return;

    int count = 0;
    while (iterator_has_next(iter)) {
        Loan *loan = (Loan*)iterator_next(iter);
        printf("%d. ", ++count);
        output_print_loan(formatter, loan);

        if (count % formatter->page_size == 0) {
            output_pause(formatter);
        }
    }

    iterator_destroy(iter);
}

/* Print book table */
void output_print_book_table(OutputFormatter *formatter, const DoublyLinkedList *books) {
    if (!formatter || !books) return;

    if (dll_is_empty((DoublyLinkedList*)books)) {
        output_print_message(formatter, "No books found.", MSG_TYPE_INFO);
        return;
    }

    output_print_header(formatter, "Book Table");

    /* Table header */
    printf("%-15s %-30s %-20s %-10s %-8s\n", "ISBN", "Title", "Author", "Copies", "Status");
    output_print_separator(formatter);

    Iterator *iter = dll_iterator_create((DoublyLinkedList*)books);
    if (!iter) return;

    int count = 0;
    while (iterator_has_next(iter)) {
        Book *book = (Book*)iterator_next(iter);

        /* Truncate long strings for table display */
        char title[31], author[21];
        strncpy(title, book->title, 30);
        title[30] = '\0';
        strncpy(author, book->author, 20);
        author[20] = '\0';

        printf("%-15s %-30s %-20s %2d/%-5d %c\n",
               book->isbn, title, author,
               book->available_copies, book->total_copies,
               book->status);

        if (++count % formatter->page_size == 0) {
            output_pause(formatter);
        }
    }

    iterator_destroy(iter);
}

/* Print member table */
void output_print_member_table(OutputFormatter *formatter, const DoublyLinkedList *members) {
    if (!formatter || !members) return;

    if (dll_is_empty((DoublyLinkedList*)members)) {
        output_print_message(formatter, "No members found.", MSG_TYPE_INFO);
        return;
    }

    output_print_header(formatter, "Member Table");

    /* Table header */
    printf("%-12s %-25s %-20s %-5s %-6s %-6s\n", "Member ID", "Name", "Email", "Type", "Loans", "Status");
    output_print_separator(formatter);

    Iterator *iter = dll_iterator_create((DoublyLinkedList*)members);
    if (!iter) return;

    int count = 0;
    while (iterator_has_next(iter)) {
        Member *member = (Member*)iterator_next(iter);

        /* Truncate long strings for table display */
        char name[26], email[21];
        strncpy(name, member->name, 25);
        name[25] = '\0';
        strncpy(email, member->email, 20);
        email[20] = '\0';

        printf("%-12s %-25s %-20s %-5c %-6d %-6c\n",
               member->member_id, name, email,
               member->membership_type, member->loan_count,
               member->status);

        if (++count % formatter->page_size == 0) {
            output_pause(formatter);
        }
    }

    iterator_destroy(iter);
}

/* Print loan table */
void output_print_loan_table(OutputFormatter *formatter, const DoublyLinkedList *loans) {
    if (!formatter || !loans) return;

    if (dll_is_empty((DoublyLinkedList*)loans)) {
        output_print_message(formatter, "No loans found.", MSG_TYPE_INFO);
        return;
    }

    output_print_header(formatter, "Loan Table");

    /* Table header */
    printf("%-12s %-12s %-15s %-12s %-12s %-8s %-6s\n",
           "Loan ID", "Member ID", "ISBN", "Loan Date", "Due Date", "Fine", "Status");
    output_print_separator(formatter);

    Iterator *iter = dll_iterator_create((DoublyLinkedList*)loans);
    if (!iter) return;

    int count = 0;
    while (iterator_has_next(iter)) {
        Loan *loan = (Loan*)iterator_next(iter);

        printf("%-12s %-12s %-15s %-12s %-12s $%-7.2f %-6c\n",
               loan->loan_id, loan->member_id, loan->isbn,
               loan->loan_date, loan->due_date,
               loan->fine_amount, loan->status);

        if (++count % formatter->page_size == 0) {
            output_pause(formatter);
        }
    }

    iterator_destroy(iter);
}

/* Print with paging */
void output_print_with_paging(OutputFormatter *formatter, const DoublyLinkedList *list,
                              void (*print_item)(OutputFormatter *formatter, const void *item)) {
    if (!formatter || !list || !print_item) return;

    if (dll_is_empty((DoublyLinkedList*)list)) {
        output_print_message(formatter, "No items found.", MSG_TYPE_INFO);
        return;
    }

    Iterator *iter = dll_iterator_create((DoublyLinkedList*)list);
    if (!iter) return;

    int count = 0;
    while (iterator_has_next(iter)) {
        void *item = iterator_next(iter);
        print_item(formatter, item);

        if (++count % formatter->page_size == 0) {
            output_pause(formatter);
        }
    }

    iterator_destroy(iter);
}

/* Print statistics */
void output_print_statistics(OutputFormatter *formatter, int total_books, int available_books,
                            int total_members, int active_members, int total_loans, int active_loans) {
    if (!formatter) return;

    output_print_header(formatter, "Library Statistics");

    printf("Book Statistics:\n");
    printf("  Total Books: %d\n", total_books);
    printf("  Available Books: %d\n", available_books);
    printf("  Books on Loan: %d\n", total_books - available_books);
    printf("\n");

    printf("Member Statistics:\n");
    printf("  Total Members: %d\n", total_members);
    printf("  Active Members: %d\n", active_members);
    printf("  Inactive Members: %d\n", total_members - active_members);
    printf("\n");

    printf("Loan Statistics:\n");
    printf("  Total Loans: %d\n", total_loans);
    printf("  Active Loans: %d\n", active_loans);
    printf("  Completed Loans: %d\n", total_loans - active_loans);
    printf("\n");

    if (total_books > 0) {
        double utilization = ((double)(total_books - available_books) / total_books) * 100;
        printf("Collection Utilization: %.1f%%\n", utilization);
    }
}

/* Set page size */
void output_set_page_size(OutputFormatter *formatter, int size) {
    if (formatter && size > 0) {
        formatter->page_size = size;
    }
}

/* Set column width */
void output_set_column_width(OutputFormatter *formatter, int width) {
    if (formatter && width > 0) {
        formatter->column_width = width;
    }
}

/* Set use colors */
void output_set_use_colors(OutputFormatter *formatter, bool use_colors) {
    if (formatter) {
        formatter->use_colors = use_colors;
    }
}

/* Clear screen */
void output_clear_screen(OutputFormatter *formatter) {
    if (!formatter) return;

    /* Simple clear screen for different platforms */
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/* Pause for user input */
void output_pause(OutputFormatter *formatter) {
    if (!formatter) return;

    printf("Press Enter to continue...");
    getchar();
}