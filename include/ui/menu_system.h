#ifndef MENU_SYSTEM_H
#define MENU_SYSTEM_H

#include "../common.h"

/* Forward declarations */
typedef struct MenuItem MenuItem;
typedef struct Menu Menu;
typedef struct MenuSystem MenuSystem;

/* Menu item structure */
struct MenuItem {
    int id;
    char title[50];
    void (*action)(void *context);
    bool enabled;
};

/* Menu structure */
struct Menu {
    char title[100];
    MenuItem *items;
    int item_count;
    int max_items;
};

/* Menu system structure */
struct MenuSystem {
    Menu *current_menu;
    Menu *main_menu;
    Menu **menu_stack;
    int stack_size;
    int stack_capacity;
    void *context; /* Application context */
};

/* Menu system management */
MenuSystem* menu_system_create(void *context);
void menu_system_destroy(MenuSystem *system);

/* Menu management */
Menu* menu_create(const char *title, int max_items);
void menu_destroy(Menu *menu);
LMS_Result menu_add_item(Menu *menu, int id, const char *title, void (*action)(void *context));
LMS_Result menu_set_item_enabled(Menu *menu, int id, bool enabled);

/* Menu display and interaction */
void menu_system_display_current(MenuSystem *system);
LMS_Result menu_system_handle_input(MenuSystem *system);
void menu_system_push(MenuSystem *system, Menu *menu);
Menu* menu_system_pop(MenuSystem *system);

/* Predefined menu creators */
Menu* create_main_menu(void);
Menu* create_book_menu(void);
Menu* create_member_menu(void);
Menu* create_loan_menu(void);
Menu* create_report_menu(void);

/* Menu actions */
void show_help(void *context);
void show_about(void *context);
void exit_application(void *context);

#endif /* MENU_SYSTEM_H */