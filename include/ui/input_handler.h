#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include "../common.h"
#include "../models/models.h"

/* Input validation types */
typedef enum {
    INPUT_TYPE_STRING,
    INPUT_TYPE_INTEGER,
    INPUT_TYPE_DOUBLE,
    INPUT_TYPE_ISBN,
    INPUT_TYPE_EMAIL,
    INPUT_TYPE_PHONE,
    INPUT_TYPE_DATE
} InputType;

/* Input handler structure */
typedef struct InputHandler {
    char input_buffer[1024];
} InputHandler;

/* Input handler management */
InputHandler* input_handler_create(void);
void input_handler_destroy(InputHandler *handler);

/* Basic input functions */
int input_get_integer(InputHandler *handler, const char *prompt, int min, int max);
double input_get_double(InputHandler *handler, const char *prompt, double min, double max);
char* input_get_string(InputHandler *handler, const char *prompt, int max_length);
bool input_get_confirmation(InputHandler *handler, const char *prompt);

/* Specialized input functions */
char* input_get_isbn(InputHandler *handler, const char *prompt);
char* input_get_email(InputHandler *handler, const char *prompt);
char* input_get_phone(InputHandler *handler, const char *prompt);
char* input_get_date(InputHandler *handler, const char *prompt);

/* Complex input functions */
Book* input_get_book_data(InputHandler *handler);
Member* input_get_member_data(InputHandler *handler);
BookSearchCriteria* input_get_book_search_criteria(InputHandler *handler);
MemberSearchCriteria* input_get_member_search_criteria(InputHandler *handler);

/* Input validation */
bool input_validate_input(InputHandler *handler, const char *input, InputType type);
void input_show_error(InputHandler *handler, const char *message);

/* Utility functions */
void input_clear_buffer(InputHandler *handler);
void input_wait_for_enter(InputHandler *handler);
void input_display_prompt(const char *prompt);

#endif /* INPUT_HANDLER_H */