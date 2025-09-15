#include "../../include/ui/menu_system.h"

#define INITIAL_STACK_CAPACITY 10

/* Create a new menu system */
MenuSystem* menu_system_create(void *context) {
    MenuSystem *system = malloc(sizeof(MenuSystem));
    if (!system) return NULL;

    system->menu_stack = malloc(sizeof(Menu*) * INITIAL_STACK_CAPACITY);
    if (!system->menu_stack) {
        free(system);
        return NULL;
    }

    system->current_menu = NULL;
    system->main_menu = create_main_menu();
    system->stack_size = 0;
    system->stack_capacity = INITIAL_STACK_CAPACITY;
    system->context = context;

    if (!system->main_menu) {
        menu_system_destroy(system);
        return NULL;
    }

    system->current_menu = system->main_menu;

    return system;
}

/* Destroy the menu system */
void menu_system_destroy(MenuSystem *system) {
    if (!system) return;

    /* Destroy all menus in stack */
    for (int i = 0; i < system->stack_size; i++) {
        menu_destroy(system->menu_stack[i]);
    }

    if (system->main_menu) {
        menu_destroy(system->main_menu);
    }

    free(system->menu_stack);
    free(system);
}

/* Create a new menu */
Menu* menu_create(const char *title, int max_items) {
    if (!title || max_items <= 0) return NULL;

    Menu *menu = malloc(sizeof(Menu));
    if (!menu) return NULL;

    menu->items = malloc(sizeof(MenuItem) * max_items);
    if (!menu->items) {
        free(menu);
        return NULL;
    }

    strncpy(menu->title, title, sizeof(menu->title) - 1);
    menu->title[sizeof(menu->title) - 1] = '\0';
    menu->item_count = 0;
    menu->max_items = max_items;

    return menu;
}

/* Destroy a menu */
void menu_destroy(Menu *menu) {
    if (!menu) return;

    free(menu->items);
    free(menu);
}

/* Add an item to a menu */
LMS_Result menu_add_item(Menu *menu, int id, const char *title, void (*action)(void *context)) {
    CHECK_NULL(menu);
    CHECK_NULL(title);

    if (menu->item_count >= menu->max_items) {
        return LMS_ERROR_INVALID_INPUT;
    }

    MenuItem *item = &menu->items[menu->item_count];
    item->id = id;
    strncpy(item->title, title, sizeof(item->title) - 1);
    item->title[sizeof(item->title) - 1] = '\0';
    item->action = action;
    item->enabled = true;

    menu->item_count++;
    return LMS_SUCCESS;
}

/* Set menu item enabled/disabled */
LMS_Result menu_set_item_enabled(Menu *menu, int id, bool enabled) {
    CHECK_NULL(menu);

    for (int i = 0; i < menu->item_count; i++) {
        if (menu->items[i].id == id) {
            menu->items[i].enabled = enabled;
            return LMS_SUCCESS;
        }
    }

    return LMS_ERROR_NOT_FOUND;
}

/* Display the current menu */
void menu_system_display_current(MenuSystem *system) {
    if (!system || !system->current_menu) return;

    Menu *menu = system->current_menu;

    printf("\n");
    printf("================================================\n");
    printf("  %s\n", menu->title);
    printf("================================================\n");

    for (int i = 0; i < menu->item_count; i++) {
        MenuItem *item = &menu->items[i];
        if (item->enabled) {
            printf("  %d. %s\n", item->id, item->title);
        } else {
            printf("  %d. %s (Disabled)\n", item->id, item->title);
        }
    }

    printf("================================================\n");
    printf("Enter your choice: ");
}

/* Handle user input */
LMS_Result menu_system_handle_input(MenuSystem *system) {
    CHECK_NULL(system);

    int choice;
    if (scanf("%d", &choice) != 1) {
        printf("Invalid input. Please enter a number.\n");
        /* Clear input buffer */
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return LMS_ERROR_INVALID_INPUT;
    }

    /* Find the menu item with the given choice */
    Menu *menu = system->current_menu;
    for (int i = 0; i < menu->item_count; i++) {
        MenuItem *item = &menu->items[i];
        if (item->id == choice && item->enabled) {
            if (item->action) {
                item->action(system->context);
            }
            return LMS_SUCCESS;
        }
    }

    printf("Invalid choice. Please try again.\n");
    return LMS_ERROR_INVALID_INPUT;
}

/* Push a menu onto the stack */
void menu_system_push(MenuSystem *system, Menu *menu) {
    if (!system || !menu) return;

    /* Resize stack if needed */
    if (system->stack_size >= system->stack_capacity) {
        system->stack_capacity *= 2;
        system->menu_stack = realloc(system->menu_stack,
                                   sizeof(Menu*) * system->stack_capacity);
        if (!system->menu_stack) return;
    }

    system->menu_stack[system->stack_size++] = system->current_menu;
    system->current_menu = menu;
}

/* Pop a menu from the stack */
Menu* menu_system_pop(MenuSystem *system) {
    if (!system || system->stack_size == 0) return NULL;

    Menu *previous = system->current_menu;
    system->current_menu = system->menu_stack[--system->stack_size];

    return previous;
}

/* Create the main menu */
Menu* create_main_menu(void) {
    Menu *menu = menu_create("Library Management System - Main Menu", 10);
    if (!menu) return NULL;

    menu_add_item(menu, 1, "Book Management", NULL);
    menu_add_item(menu, 2, "Member Management", NULL);
    menu_add_item(menu, 3, "Loan Management", NULL);
    menu_add_item(menu, 4, "Reports", NULL);
    menu_add_item(menu, 5, "Search", NULL);
    menu_add_item(menu, 6, "Statistics", NULL);
    menu_add_item(menu, 7, "Help", show_help);
    menu_add_item(menu, 8, "About", show_about);
    menu_add_item(menu, 0, "Exit", exit_application);

    return menu;
}

/* Create book management menu */
Menu* create_book_menu(void) {
    Menu *menu = menu_create("Book Management", 8);
    if (!menu) return NULL;

    menu_add_item(menu, 1, "Add New Book", NULL);
    menu_add_item(menu, 2, "Search Books", NULL);
    menu_add_item(menu, 3, "Update Book", NULL);
    menu_add_item(menu, 4, "Delete Book", NULL);
    menu_add_item(menu, 5, "List All Books", NULL);
    menu_add_item(menu, 6, "List Available Books", NULL);
    menu_add_item(menu, 0, "Back to Main Menu", NULL);

    return menu;
}

/* Create member management menu */
Menu* create_member_menu(void) {
    Menu *menu = menu_create("Member Management", 8);
    if (!menu) return NULL;

    menu_add_item(menu, 1, "Register New Member", NULL);
    menu_add_item(menu, 2, "Search Members", NULL);
    menu_add_item(menu, 3, "Update Member", NULL);
    menu_add_item(menu, 4, "Suspend Member", NULL);
    menu_add_item(menu, 5, "Activate Member", NULL);
    menu_add_item(menu, 6, "List All Members", NULL);
    menu_add_item(menu, 0, "Back to Main Menu", NULL);

    return menu;
}

/* Create loan management menu */
Menu* create_loan_menu(void) {
    Menu *menu = menu_create("Loan Management", 8);
    if (!menu) return NULL;

    menu_add_item(menu, 1, "Borrow Book", NULL);
    menu_add_item(menu, 2, "Return Book", NULL);
    menu_add_item(menu, 3, "Renew Loan", NULL);
    menu_add_item(menu, 4, "View Active Loans", NULL);
    menu_add_item(menu, 5, "View Overdue Loans", NULL);
    menu_add_item(menu, 6, "Process Fine Payment", NULL);
    menu_add_item(menu, 0, "Back to Main Menu", NULL);

    return menu;
}

/* Create reports menu */
Menu* create_report_menu(void) {
    Menu *menu = menu_create("Reports", 8);
    if (!menu) return NULL;

    menu_add_item(menu, 1, "Book Inventory Report", NULL);
    menu_add_item(menu, 2, "Member Activity Report", NULL);
    menu_add_item(menu, 3, "Loan Statistics", NULL);
    menu_add_item(menu, 4, "Overdue Report", NULL);
    menu_add_item(menu, 5, "Popular Books Report", NULL);
    menu_add_item(menu, 6, "Monthly Summary", NULL);
    menu_add_item(menu, 0, "Back to Main Menu", NULL);

    return menu;
}

/* Show help */
void show_help(void *context) {
    printf("\n");
    printf("================================================\n");
    printf("                    HELP\n");
    printf("================================================\n");
    printf("This is a Library Management System built with\n");
    printf("C programming language and doubly linked lists.\n");
    printf("\n");
    printf("Navigation:\n");
    printf("- Use numeric keys to select menu options\n");
    printf("- Use 0 to go back to previous menu\n");
    printf("- Follow on-screen prompts for data entry\n");
    printf("\n");
    printf("Features:\n");
    printf("- Book Management (Add, Update, Search, Delete)\n");
    printf("- Member Management (Register, Update, Suspend)\n");
    printf("- Loan Management (Borrow, Return, Renew)\n");
    printf("- Reports and Statistics\n");
    printf("\n");
    printf("For technical support, contact the administrator.\n");
    printf("================================================\n");
    printf("Press Enter to continue...");
    getchar();
    getchar(); /* Consume the newline */
}

/* Show about */
void show_about(void *context) {
    printf("\n");
    printf("================================================\n");
    printf("                   ABOUT\n");
    printf("================================================\n");
    printf("Library Management System v1.0\n");
    printf("\n");
    printf("Developed using:\n");
    printf("- C Programming Language\n");
    printf("- Doubly Linked List Data Structure\n");
    printf("- Modular Architecture\n");
    printf("\n");
    printf("Features:\n");
    printf("- Efficient book cataloging\n");
    printf("- Member registration and management\n");
    printf("- Automated loan tracking\n");
    printf("- Overdue management with fines\n");
    printf("- Comprehensive reporting\n");
    printf("\n");
    printf("Architecture:\n");
    printf("- Repository Pattern for data access\n");
    printf("- Service Layer for business logic\n");
    printf("- Clean separation of concerns\n");
    printf("\n");
    printf("Built with ASCII-only characters for compatibility\n");
    printf("================================================\n");
    printf("Press Enter to continue...");
    getchar();
    getchar(); /* Consume the newline */
}

/* Exit application */
void exit_application(void *context) {
    printf("\nThank you for using Library Management System!\n");
    printf("Goodbye!\n");
    exit(0);
}