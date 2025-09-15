#include "include/common.h"
#include "include/core/doubly_linked_list.h"
#include "include/models/models.h"
#include "include/repositories/book_repository.h"
#include "include/repositories/member_repository.h"
#include "include/repositories/loan_repository.h"
#include "include/services/book_service.h"
#include "include/services/member_service.h"
#include "include/services/loan_service.h"
#include "include/ui/menu_system.h"
#include "include/ui/input_handler.h"
#include "include/ui/output_formatter.h"

/* Application context structure */
typedef struct AppContext {
    /* Repositories */
    BookRepository *book_repo;
    MemberRepository *member_repo;
    LoanRepository *loan_repo;

    /* Services */
    BookService *book_service;
    MemberService *member_service;
    LoanService *loan_service;

    /* UI Components */
    MenuSystem *menu_system;
    InputHandler *input_handler;
    OutputFormatter *output_formatter;

    /* Application state */
    bool running;
} AppContext;

/* Function prototypes */
static AppContext* app_context_create(void);
static void app_context_destroy(AppContext *ctx);
static void initialize_sample_data(AppContext *ctx);
static void run_application(AppContext *ctx);

/* Menu action functions */
static void action_book_management(void *context);
static void action_member_management(void *context);
static void action_loan_management(void *context);
static void action_reports(void *context);
static void action_search(void *context);
static void action_statistics(void *context);

/* Book management functions */
static void action_add_book(void *context);
static void action_search_books(void *context);
static void action_list_books(void *context);

/* Member management functions */
static void action_register_member(void *context);
static void action_search_members(void *context);
static void action_list_members(void *context);

/* Loan management functions */
static void action_borrow_book(void *context);
static void action_return_book(void *context);
static void action_list_active_loans(void *context);

/* Main function */
int main(void) {
    printf("Starting Library Management System...\n");

    /* Create application context */
    AppContext *ctx = app_context_create();
    if (!ctx) {
        printf("Failed to initialize application context\n");
        return 1;
    }

    /* Initialize sample data */
    initialize_sample_data(ctx);

    /* Run the application */
    run_application(ctx);

    /* Cleanup */
    app_context_destroy(ctx);

    printf("Application terminated successfully.\n");
    return 0;
}

/* Create application context */
static AppContext* app_context_create(void) {
    AppContext *ctx = malloc(sizeof(AppContext));
    if (!ctx) return NULL;

    /* Initialize all pointers to NULL */
    memset(ctx, 0, sizeof(AppContext));

    /* Create repositories */
    ctx->book_repo = book_repository_create();
    ctx->member_repo = member_repository_create();
    ctx->loan_repo = loan_repository_create();

    if (!ctx->book_repo || !ctx->member_repo || !ctx->loan_repo) {
        app_context_destroy(ctx);
        return NULL;
    }

    /* Create services */
    ctx->book_service = book_service_create(ctx->book_repo, ctx->loan_repo);
    ctx->member_service = member_service_create(ctx->member_repo, ctx->loan_repo);
    ctx->loan_service = loan_service_create(ctx->loan_repo, ctx->book_repo, ctx->member_repo);

    if (!ctx->book_service || !ctx->member_service || !ctx->loan_service) {
        app_context_destroy(ctx);
        return NULL;
    }

    /* Create UI components */
    ctx->input_handler = input_handler_create();
    ctx->output_formatter = output_formatter_create();
    ctx->menu_system = menu_system_create(ctx);

    if (!ctx->input_handler || !ctx->output_formatter || !ctx->menu_system) {
        app_context_destroy(ctx);
        return NULL;
    }

    /* Set up menu actions */
    Menu *main_menu = ctx->menu_system->main_menu;
    if (main_menu) {
        /* Update menu actions to point to our functions */
        for (int i = 0; i < main_menu->item_count; i++) {
            MenuItem *item = &main_menu->items[i];
            switch (item->id) {
                case 1:
                    item->action = action_book_management;
                    break;
                case 2:
                    item->action = action_member_management;
                    break;
                case 3:
                    item->action = action_loan_management;
                    break;
                case 4:
                    item->action = action_reports;
                    break;
                case 5:
                    item->action = action_search;
                    break;
                case 6:
                    item->action = action_statistics;
                    break;
            }
        }
    }

    ctx->running = true;
    return ctx;
}

/* Destroy application context */
static void app_context_destroy(AppContext *ctx) {
    if (!ctx) return;

    /* Destroy UI components */
    input_handler_destroy(ctx->input_handler);
    output_formatter_destroy(ctx->output_formatter);
    menu_system_destroy(ctx->menu_system);

    /* Destroy services */
    book_service_destroy(ctx->book_service);
    member_service_destroy(ctx->member_service);
    loan_service_destroy(ctx->loan_service);

    /* Destroy repositories */
    book_repository_destroy(ctx->book_repo);
    member_repository_destroy(ctx->member_repo);
    loan_repository_destroy(ctx->loan_repo);

    free(ctx);
}

/* Initialize sample data */
static void initialize_sample_data(AppContext *ctx) {
    if (!ctx) return;

    printf("Initializing sample data...\n");

    /* Sample books */
    Book book1;
    book_init(&book1);
    strcpy(book1.isbn, "9780132350884");
    strcpy(book1.title, "Clean Code");
    strcpy(book1.author, "Robert C. Martin");
    strcpy(book1.publisher, "Prentice Hall");
    book1.publication_year = 2008;
    strcpy(book1.category, "Programming");
    book1.total_copies = 3;
    book1.available_copies = 3;
    book1.price = 49.99;
    book1.status = 'A';
    book_service_register_book(ctx->book_service, &book1);

    Book book2;
    book_init(&book2);
    strcpy(book2.isbn, "9780134685991");
    strcpy(book2.title, "Effective C++");
    strcpy(book2.author, "Scott Meyers");
    strcpy(book2.publisher, "Addison-Wesley");
    book2.publication_year = 2005;
    strcpy(book2.category, "Programming");
    book2.total_copies = 2;
    book2.available_copies = 2;
    book2.price = 54.99;
    book2.status = 'A';
    book_service_register_book(ctx->book_service, &book2);

    /* Sample members */
    Member member1;
    member_init(&member1);
    strcpy(member1.member_id, "M001");
    strcpy(member1.name, "John Smith");
    strcpy(member1.phone, "555-0123");
    strcpy(member1.email, "john.smith@email.com");
    strcpy(member1.address, "123 Main St, City, State");
    strcpy(member1.join_date, "2024-01-01");
    member1.membership_type = 'R';
    member1.loan_count = 0;
    member1.status = 'A';
    member_service_register_member(ctx->member_service, &member1);

    Member member2;
    member_init(&member2);
    strcpy(member2.member_id, "M002");
    strcpy(member2.name, "Jane Doe");
    strcpy(member2.phone, "555-0456");
    strcpy(member2.email, "jane.doe@email.com");
    strcpy(member2.address, "456 Oak Ave, City, State");
    strcpy(member2.join_date, "2024-01-15");
    member2.membership_type = 'P';
    member2.loan_count = 0;
    member2.status = 'A';
    member_service_register_member(ctx->member_service, &member2);

    printf("Sample data initialized successfully.\n");
}

/* Run the application */
static void run_application(AppContext *ctx) {
    if (!ctx) return;

    output_print_header(ctx->output_formatter, "Welcome to Library Management System");
    printf("System initialized successfully!\n");
    printf("Use the menu to navigate through the application.\n");

    while (ctx->running) {
        menu_system_display_current(ctx->menu_system);
        menu_system_handle_input(ctx->menu_system);
    }
}

/* Book management menu */
static void action_book_management(void *context) {
    AppContext *ctx = (AppContext*)context;
    if (!ctx) return;

    Menu *book_menu = create_book_menu();
    if (!book_menu) return;

    /* Set up book menu actions */
    for (int i = 0; i < book_menu->item_count; i++) {
        MenuItem *item = &book_menu->items[i];
        switch (item->id) {
            case 1:
                item->action = action_add_book;
                break;
            case 2:
                item->action = action_search_books;
                break;
            case 5:
                item->action = action_list_books;
                break;
            case 0:
                item->action = NULL; /* Back to main menu */
                break;
        }
    }

    menu_system_push(ctx->menu_system, book_menu);
}

/* Member management menu */
static void action_member_management(void *context) {
    AppContext *ctx = (AppContext*)context;
    if (!ctx) return;

    Menu *member_menu = create_member_menu();
    if (!member_menu) return;

    /* Set up member menu actions */
    for (int i = 0; i < member_menu->item_count; i++) {
        MenuItem *item = &member_menu->items[i];
        switch (item->id) {
            case 1:
                item->action = action_register_member;
                break;
            case 2:
                item->action = action_search_members;
                break;
            case 6:
                item->action = action_list_members;
                break;
            case 0:
                item->action = NULL; /* Back to main menu */
                break;
        }
    }

    menu_system_push(ctx->menu_system, member_menu);
}

/* Loan management menu */
static void action_loan_management(void *context) {
    AppContext *ctx = (AppContext*)context;
    if (!ctx) return;

    Menu *loan_menu = create_loan_menu();
    if (!loan_menu) return;

    /* Set up loan menu actions */
    for (int i = 0; i < loan_menu->item_count; i++) {
        MenuItem *item = &loan_menu->items[i];
        switch (item->id) {
            case 1:
                item->action = action_borrow_book;
                break;
            case 2:
                item->action = action_return_book;
                break;
            case 4:
                item->action = action_list_active_loans;
                break;
            case 0:
                item->action = NULL; /* Back to main menu */
                break;
        }
    }

    menu_system_push(ctx->menu_system, loan_menu);
}

/* Add book action */
static void action_add_book(void *context) {
    AppContext *ctx = (AppContext*)context;
    if (!ctx) return;

    output_print_header(ctx->output_formatter, "Add New Book");

    Book *book = input_get_book_data(ctx->input_handler);
    if (!book) {
        output_print_error(ctx->output_formatter, "Failed to get book data");
        return;
    }

    LMS_Result result = book_service_register_book(ctx->book_service, book);
    if (result == LMS_SUCCESS) {
        output_print_success(ctx->output_formatter, "Book added successfully!");
    } else {
        output_print_error(ctx->output_formatter, lms_get_error_string(result));
    }

    free(book);
    input_wait_for_enter(ctx->input_handler);
}

/* Search books action */
static void action_search_books(void *context) {
    AppContext *ctx = (AppContext*)context;
    if (!ctx) return;

    output_print_header(ctx->output_formatter, "Search Books");

    BookSearchCriteria *criteria = input_get_book_search_criteria(ctx->input_handler);
    if (!criteria) {
        output_print_error(ctx->output_formatter, "Failed to get search criteria");
        return;
    }

    DoublyLinkedList *results = book_service_search(ctx->book_service, criteria);
    if (results) {
        output_print_book_table(ctx->output_formatter, results);
        dll_destroy(results);
    } else {
        output_print_message(ctx->output_formatter, "No books found", MSG_TYPE_INFO);
    }

    free(criteria);
    input_wait_for_enter(ctx->input_handler);
}

/* List all books action */
static void action_list_books(void *context) {
    AppContext *ctx = (AppContext*)context;
    if (!ctx) return;

    output_print_header(ctx->output_formatter, "All Books");

    DoublyLinkedList *books = book_service_get_all_books(ctx->book_service);
    if (books) {
        output_print_book_table(ctx->output_formatter, books);
        dll_destroy(books);
    } else {
        output_print_message(ctx->output_formatter, "No books found", MSG_TYPE_INFO);
    }

    input_wait_for_enter(ctx->input_handler);
}

/* Register member action */
static void action_register_member(void *context) {
    AppContext *ctx = (AppContext*)context;
    if (!ctx) return;

    output_print_header(ctx->output_formatter, "Register New Member");

    Member *member = input_get_member_data(ctx->input_handler);
    if (!member) {
        output_print_error(ctx->output_formatter, "Failed to get member data");
        return;
    }

    LMS_Result result = member_service_register_member(ctx->member_service, member);
    if (result == LMS_SUCCESS) {
        output_print_success(ctx->output_formatter, "Member registered successfully!");
    } else {
        output_print_error(ctx->output_formatter, lms_get_error_string(result));
    }

    free(member);
    input_wait_for_enter(ctx->input_handler);
}

/* Search members action */
static void action_search_members(void *context) {
    AppContext *ctx = (AppContext*)context;
    if (!ctx) return;

    output_print_header(ctx->output_formatter, "Search Members");

    MemberSearchCriteria *criteria = input_get_member_search_criteria(ctx->input_handler);
    if (!criteria) {
        output_print_error(ctx->output_formatter, "Failed to get search criteria");
        return;
    }

    DoublyLinkedList *results = member_service_search(ctx->member_service, criteria);
    if (results) {
        output_print_member_table(ctx->output_formatter, results);
        dll_destroy(results);
    } else {
        output_print_message(ctx->output_formatter, "No members found", MSG_TYPE_INFO);
    }

    free(criteria);
    input_wait_for_enter(ctx->input_handler);
}

/* List all members action */
static void action_list_members(void *context) {
    AppContext *ctx = (AppContext*)context;
    if (!ctx) return;

    output_print_header(ctx->output_formatter, "All Members");

    DoublyLinkedList *members = member_service_get_all_members(ctx->member_service);
    if (members) {
        output_print_member_table(ctx->output_formatter, members);
        dll_destroy(members);
    } else {
        output_print_message(ctx->output_formatter, "No members found", MSG_TYPE_INFO);
    }

    input_wait_for_enter(ctx->input_handler);
}

/* Borrow book action */
static void action_borrow_book(void *context) {
    AppContext *ctx = (AppContext*)context;
    if (!ctx) return;

    output_print_header(ctx->output_formatter, "Borrow Book");

    char *member_id = input_get_string(ctx->input_handler, "Enter Member ID", 10);
    if (!member_id) return;

    char *isbn = input_get_isbn(ctx->input_handler, "Enter Book ISBN");
    if (!isbn) {
        free(member_id);
        return;
    }

    LMS_Result result = loan_service_borrow_book(ctx->loan_service, member_id, isbn);
    if (result == LMS_SUCCESS) {
        output_print_success(ctx->output_formatter, "Book borrowed successfully!");
    } else {
        output_print_error(ctx->output_formatter, lms_get_error_string(result));
    }

    free(member_id);
    free(isbn);
    input_wait_for_enter(ctx->input_handler);
}

/* Return book action */
static void action_return_book(void *context) {
    AppContext *ctx = (AppContext*)context;
    if (!ctx) return;

    output_print_header(ctx->output_formatter, "Return Book");

    char *loan_id = input_get_string(ctx->input_handler, "Enter Loan ID", 10);
    if (!loan_id) return;

    LMS_Result result = loan_service_return_book(ctx->loan_service, loan_id);
    if (result == LMS_SUCCESS) {
        output_print_success(ctx->output_formatter, "Book returned successfully!");
    } else {
        output_print_error(ctx->output_formatter, lms_get_error_string(result));
    }

    free(loan_id);
    input_wait_for_enter(ctx->input_handler);
}

/* List active loans action */
static void action_list_active_loans(void *context) {
    AppContext *ctx = (AppContext*)context;
    if (!ctx) return;

    output_print_header(ctx->output_formatter, "Active Loans");

    DoublyLinkedList *loans = loan_service_get_active_loans(ctx->loan_service);
    if (loans) {
        output_print_loan_table(ctx->output_formatter, loans);
        dll_destroy(loans);
    } else {
        output_print_message(ctx->output_formatter, "No active loans found", MSG_TYPE_INFO);
    }

    input_wait_for_enter(ctx->input_handler);
}

/* Reports action */
static void action_reports(void *context) {
    AppContext *ctx = (AppContext*)context;
    if (!ctx) return;

    output_print_message(ctx->output_formatter, "Reports feature coming soon!", MSG_TYPE_INFO);
    input_wait_for_enter(ctx->input_handler);
}

/* Search action */
static void action_search(void *context) {
    AppContext *ctx = (AppContext*)context;
    if (!ctx) return;

    output_print_message(ctx->output_formatter, "Global search feature coming soon!", MSG_TYPE_INFO);
    input_wait_for_enter(ctx->input_handler);
}

/* Statistics action */
static void action_statistics(void *context) {
    AppContext *ctx = (AppContext*)context;
    if (!ctx) return;

    output_print_header(ctx->output_formatter, "Library Statistics");

    int total_books = book_service_get_total_book_count(ctx->book_service);
    int available_books = book_service_get_available_book_count(ctx->book_service);
    int total_members = member_service_get_total_member_count(ctx->member_service);
    int active_members = member_service_get_active_member_count(ctx->member_service);
    int total_loans = loan_service_get_total_loan_count(ctx->loan_service);
    int active_loans = loan_service_get_active_loan_count(ctx->loan_service);

    output_print_statistics(ctx->output_formatter, total_books, available_books,
                           total_members, active_members, total_loans, active_loans);

    input_wait_for_enter(ctx->input_handler);
}