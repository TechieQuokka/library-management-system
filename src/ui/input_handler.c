#include "../../include/ui/input_handler.h"

/* Create a new input handler */
InputHandler* input_handler_create(void) {
    InputHandler *handler = malloc(sizeof(InputHandler));
    if (!handler) return NULL;

    memset(handler->input_buffer, 0, sizeof(handler->input_buffer));
    return handler;
}

/* Destroy the input handler */
void input_handler_destroy(InputHandler *handler) {
    if (handler) {
        free(handler);
    }
}

/* Clear input buffer */
void input_clear_buffer(InputHandler *handler) {
    if (!handler) return;

    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* Display prompt */
void input_display_prompt(const char *prompt) {
    if (prompt) {
        printf("%s: ", prompt);
    }
}

/* Wait for Enter key */
void input_wait_for_enter(InputHandler *handler) {
    if (!handler) return;

    printf("Press Enter to continue...");
    getchar();
}

/* Show error message */
void input_show_error(InputHandler *handler, const char *message) {
    if (!handler || !message) return;

    printf("ERROR: %s\n", message);
}

/* Get integer input with validation */
int input_get_integer(InputHandler *handler, const char *prompt, int min, int max) {
    if (!handler) return 0;

    int value;
    int valid = 0;

    do {
        input_display_prompt(prompt);

        if (scanf("%d", &value) != 1) {
            input_show_error(handler, "Invalid integer input");
            input_clear_buffer(handler);
            continue;
        }

        if (value < min || value > max) {
            printf("Value must be between %d and %d\n", min, max);
            continue;
        }

        valid = 1;
    } while (!valid);

    input_clear_buffer(handler);
    return value;
}

/* Get double input with validation */
double input_get_double(InputHandler *handler, const char *prompt, double min, double max) {
    if (!handler) return 0.0;

    double value;
    int valid = 0;

    do {
        input_display_prompt(prompt);

        if (scanf("%lf", &value) != 1) {
            input_show_error(handler, "Invalid decimal input");
            input_clear_buffer(handler);
            continue;
        }

        if (value < min || value > max) {
            printf("Value must be between %.2f and %.2f\n", min, max);
            continue;
        }

        valid = 1;
    } while (!valid);

    input_clear_buffer(handler);
    return value;
}

/* Get string input with validation */
char* input_get_string(InputHandler *handler, const char *prompt, int max_length) {
    if (!handler || max_length <= 0) return NULL;

    char *result = malloc(max_length + 1);
    if (!result) return NULL;

    int valid = 0;

    do {
        input_display_prompt(prompt);

        if (!fgets(handler->input_buffer, sizeof(handler->input_buffer), stdin)) {
            input_show_error(handler, "Failed to read input");
            continue;
        }

        /* Remove trailing newline */
        size_t len = strlen(handler->input_buffer);
        if (len > 0 && handler->input_buffer[len - 1] == '\n') {
            handler->input_buffer[len - 1] = '\0';
            len--;
        }

        if (len == 0) {
            input_show_error(handler, "Input cannot be empty");
            continue;
        }

        if (len > max_length) {
            printf("Input too long. Maximum %d characters allowed\n", max_length);
            continue;
        }

        valid = 1;
    } while (!valid);

    strncpy(result, handler->input_buffer, max_length);
    result[max_length] = '\0';

    return result;
}

/* Get confirmation input */
bool input_get_confirmation(InputHandler *handler, const char *prompt) {
    if (!handler) return false;

    char response;
    int valid = 0;

    do {
        printf("%s (y/n): ", prompt ? prompt : "Confirm");

        if (scanf(" %c", &response) != 1) {
            input_show_error(handler, "Invalid input");
            input_clear_buffer(handler);
            continue;
        }

        if (response == 'y' || response == 'Y') {
            input_clear_buffer(handler);
            return true;
        } else if (response == 'n' || response == 'N') {
            input_clear_buffer(handler);
            return false;
        } else {
            input_show_error(handler, "Please enter 'y' or 'n'");
            input_clear_buffer(handler);
        }
    } while (!valid);

    return false;
}

/* Get ISBN input */
char* input_get_isbn(InputHandler *handler, const char *prompt) {
    if (!handler) return NULL;

    char *isbn;
    int valid = 0;

    do {
        isbn = input_get_string(handler, prompt ? prompt : "Enter ISBN", 13);
        if (!isbn) return NULL;

        if (!validate_isbn(isbn)) {
            input_show_error(handler, "Invalid ISBN-13 format. Must be 13 digits with valid checksum");
            printf("  Example: 9780132350884 (valid ISBN-13)\n");
            printf("  Try: 1234567890128 (corrected checksum)\n");
            free(isbn);
            continue;
        }

        valid = 1;
    } while (!valid);

    return isbn;
}

/* Get email input */
char* input_get_email(InputHandler *handler, const char *prompt) {
    if (!handler) return NULL;

    char *email;
    int valid = 0;

    do {
        email = input_get_string(handler, prompt ? prompt : "Enter email", 100);
        if (!email) return NULL;

        if (!validate_email(email)) {
            input_show_error(handler, "Invalid email format");
            free(email);
            continue;
        }

        valid = 1;
    } while (!valid);

    return email;
}

/* Get phone input */
char* input_get_phone(InputHandler *handler, const char *prompt) {
    if (!handler) return NULL;

    char *phone;
    int valid = 0;

    do {
        phone = input_get_string(handler, prompt ? prompt : "Enter phone number", 15);
        if (!phone) return NULL;

        if (!validate_phone(phone)) {
            input_show_error(handler, "Invalid phone number format");
            free(phone);
            continue;
        }

        valid = 1;
    } while (!valid);

    return phone;
}

/* Get date input */
char* input_get_date(InputHandler *handler, const char *prompt) {
    if (!handler) return NULL;

    char *date;
    int valid = 0;

    do {
        date = input_get_string(handler, prompt ? prompt : "Enter date (YYYY-MM-DD)", 10);
        if (!date) return NULL;

        if (!validate_date(date)) {
            input_show_error(handler, "Invalid date format. Use YYYY-MM-DD");
            free(date);
            continue;
        }

        valid = 1;
    } while (!valid);

    return date;
}

/* Get book data input */
Book* input_get_book_data(InputHandler *handler) {
    if (!handler) return NULL;

    Book *book = malloc(sizeof(Book));
    if (!book) return NULL;

    book_init(book);

    printf("\n--- Enter Book Information ---\n");

    /* Get ISBN */
    char *isbn = input_get_isbn(handler, "ISBN");
    if (isbn) {
        strncpy(book->isbn, isbn, sizeof(book->isbn) - 1);
        free(isbn);
    }

    /* Get title */
    char *title = input_get_string(handler, "Title", 100);
    if (title) {
        strncpy(book->title, title, sizeof(book->title) - 1);
        free(title);
    }

    /* Get author */
    char *author = input_get_string(handler, "Author", 50);
    if (author) {
        strncpy(book->author, author, sizeof(book->author) - 1);
        free(author);
    }

    /* Get publisher */
    char *publisher = input_get_string(handler, "Publisher", 50);
    if (publisher) {
        strncpy(book->publisher, publisher, sizeof(book->publisher) - 1);
        free(publisher);
    }

    /* Get publication year */
    book->publication_year = input_get_integer(handler, "Publication Year", 1900, 2030);

    /* Get category */
    char *category = input_get_string(handler, "Category", 30);
    if (category) {
        strncpy(book->category, category, sizeof(book->category) - 1);
        free(category);
    }

    /* Get total copies */
    book->total_copies = input_get_integer(handler, "Total Copies", 1, 1000);
    book->available_copies = book->total_copies;

    /* Get price */
    book->price = input_get_double(handler, "Price", 0.0, 10000.0);

    book->status = 'A';

    if (!validate_book(book)) {
        input_show_error(handler, "Invalid book data");
        free(book);
        return NULL;
    }

    return book;
}

/* Get member data input */
Member* input_get_member_data(InputHandler *handler) {
    if (!handler) return NULL;

    Member *member = malloc(sizeof(Member));
    if (!member) return NULL;

    member_init(member);

    printf("\n--- Enter Member Information ---\n");

    /* Get member ID */
    char *member_id = input_get_string(handler, "Member ID", 10);
    if (member_id) {
        strncpy(member->member_id, member_id, sizeof(member->member_id) - 1);
        free(member_id);
    }

    /* Get name */
    char *name = input_get_string(handler, "Full Name", 50);
    if (name) {
        strncpy(member->name, name, sizeof(member->name) - 1);
        free(name);
    }

    /* Get phone */
    char *phone = input_get_phone(handler, "Phone Number (optional)");
    if (phone) {
        strncpy(member->phone, phone, sizeof(member->phone) - 1);
        free(phone);
    }

    /* Get email */
    char *email = input_get_email(handler, "Email (optional)");
    if (email) {
        strncpy(member->email, email, sizeof(member->email) - 1);
        free(email);
    }

    /* Get address */
    char *address = input_get_string(handler, "Address", 200);
    if (address) {
        strncpy(member->address, address, sizeof(member->address) - 1);
        free(address);
    }

    /* Get membership type */
    printf("Membership Type:\n");
    printf("1. Regular (R)\n");
    printf("2. Premium (P)\n");
    int type_choice = input_get_integer(handler, "Choose membership type", 1, 2);
    member->membership_type = (type_choice == 2) ? 'P' : 'R';

    /* Set join date to current date (simplified) */
    strncpy(member->join_date, "2024-01-01", sizeof(member->join_date) - 1);

    member->loan_count = 0;
    member->status = 'A';

    if (!validate_member(member)) {
        input_show_error(handler, "Invalid member data");
        free(member);
        return NULL;
    }

    return member;
}

/* Get book search criteria */
BookSearchCriteria* input_get_book_search_criteria(InputHandler *handler) {
    if (!handler) return NULL;

    BookSearchCriteria *criteria = malloc(sizeof(BookSearchCriteria));
    if (!criteria) return NULL;

    memset(criteria, 0, sizeof(BookSearchCriteria));

    printf("\n--- Book Search Criteria ---\n");

    /* Search by title */
    if (input_get_confirmation(handler, "Search by title")) {
        criteria->search_by_title = true;
        char *title = input_get_string(handler, "Title (partial match)", 100);
        if (title) {
            strncpy(criteria->title, title, sizeof(criteria->title) - 1);
            free(title);
        }
    }

    /* Search by author */
    if (input_get_confirmation(handler, "Search by author")) {
        criteria->search_by_author = true;
        char *author = input_get_string(handler, "Author (partial match)", 50);
        if (author) {
            strncpy(criteria->author, author, sizeof(criteria->author) - 1);
            free(author);
        }
    }

    /* Search by category */
    if (input_get_confirmation(handler, "Search by category")) {
        criteria->search_by_category = true;
        char *category = input_get_string(handler, "Category", 30);
        if (category) {
            strncpy(criteria->category, category, sizeof(criteria->category) - 1);
            free(category);
        }
    }

    /* Search by ISBN */
    if (input_get_confirmation(handler, "Search by ISBN")) {
        criteria->search_by_isbn = true;
        char *isbn = input_get_isbn(handler, "ISBN");
        if (isbn) {
            strncpy(criteria->isbn, isbn, sizeof(criteria->isbn) - 1);
            free(isbn);
        }
    }

    /* Only available books */
    criteria->only_available = input_get_confirmation(handler, "Show only available books");

    return criteria;
}

/* Get member search criteria */
MemberSearchCriteria* input_get_member_search_criteria(InputHandler *handler) {
    if (!handler) return NULL;

    MemberSearchCriteria *criteria = malloc(sizeof(MemberSearchCriteria));
    if (!criteria) return NULL;

    memset(criteria, 0, sizeof(MemberSearchCriteria));

    printf("\n--- Member Search Criteria ---\n");

    /* Search by name */
    if (input_get_confirmation(handler, "Search by name")) {
        criteria->search_by_name = true;
        char *name = input_get_string(handler, "Name (partial match)", 50);
        if (name) {
            strncpy(criteria->name, name, sizeof(criteria->name) - 1);
            free(name);
        }
    }

    /* Search by email */
    if (input_get_confirmation(handler, "Search by email")) {
        criteria->search_by_email = true;
        char *email = input_get_email(handler, "Email");
        if (email) {
            strncpy(criteria->email, email, sizeof(criteria->email) - 1);
            free(email);
        }
    }

    /* Search by phone */
    if (input_get_confirmation(handler, "Search by phone")) {
        criteria->search_by_phone = true;
        char *phone = input_get_phone(handler, "Phone");
        if (phone) {
            strncpy(criteria->phone, phone, sizeof(criteria->phone) - 1);
            free(phone);
        }
    }

    /* Only active members */
    criteria->only_active = input_get_confirmation(handler, "Show only active members");

    return criteria;
}

/* Validate input based on type */
bool input_validate_input(InputHandler *handler, const char *input, InputType type) {
    if (!handler || !input) return false;

    switch (type) {
        case INPUT_TYPE_STRING:
            return strlen(input) > 0;

        case INPUT_TYPE_INTEGER: {
            char *endptr;
            strtol(input, &endptr, 10);
            return *endptr == '\0';
        }

        case INPUT_TYPE_DOUBLE: {
            char *endptr;
            strtod(input, &endptr);
            return *endptr == '\0';
        }

        case INPUT_TYPE_ISBN:
            return validate_isbn(input);

        case INPUT_TYPE_EMAIL:
            return validate_email(input);

        case INPUT_TYPE_PHONE:
            return validate_phone(input);

        case INPUT_TYPE_DATE:
            return validate_date(input);

        default:
            return false;
    }
}