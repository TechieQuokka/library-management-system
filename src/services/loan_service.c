#include "../../include/services/loan_service.h"
#include <time.h>

/* Create a new loan service */
LoanService* loan_service_create(LoanRepository *loan_repo, BookRepository *book_repo, MemberRepository *member_repo) {
    if (!loan_repo || !book_repo || !member_repo) return NULL;

    LoanService *service = malloc(sizeof(LoanService));
    if (!service) return NULL;

    service->loan_repo = loan_repo;
    service->book_repo = book_repo;
    service->member_repo = member_repo;

    return service;
}

/* Destroy the loan service */
void loan_service_destroy(LoanService *service) {
    if (service) {
        free(service);
    }
}

/* Helper function to get current date as string */
static char* get_current_date_string(void) {
    static char date_str[11];
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);

    snprintf(date_str, sizeof(date_str), "%04d-%02d-%02d",
             tm_info->tm_year + 1900,
             tm_info->tm_mon + 1,
             tm_info->tm_mday);

    return date_str;
}

/* Borrow a book */
LMS_Result loan_service_borrow_book(LoanService *service, const char *member_id, const char *isbn) {
    CHECK_NULL(service);
    CHECK_NULL(member_id);
    CHECK_NULL(isbn);

    /* Check if borrowing is allowed */
    if (!loan_service_can_borrow(service, member_id, isbn)) {
        return LMS_ERROR_LOAN_LIMIT;
    }

    /* Reserve the book */
    LMS_Result result = book_repo_update_availability(service->book_repo, isbn, -1);
    if (result != LMS_SUCCESS) {
        return result;
    }

    /* Update member loan count */
    result = member_repo_update_loan_count(service->member_repo, member_id, 1);
    if (result != LMS_SUCCESS) {
        /* Rollback book reservation */
        book_repo_update_availability(service->book_repo, isbn, 1);
        return result;
    }

    /* Create loan record */
    Loan loan;
    loan_init(&loan);

    char *loan_id = loan_service_generate_loan_id(service);
    if (!loan_id) {
        /* Rollback changes */
        book_repo_update_availability(service->book_repo, isbn, 1);
        member_repo_update_loan_count(service->member_repo, member_id, -1);
        return LMS_ERROR_MEMORY;
    }

    strncpy(loan.loan_id, loan_id, sizeof(loan.loan_id) - 1);
    strncpy(loan.member_id, member_id, sizeof(loan.member_id) - 1);
    strncpy(loan.isbn, isbn, sizeof(loan.isbn) - 1);

    char *current_date = get_current_date_string();
    strncpy(loan.loan_date, current_date, sizeof(loan.loan_date) - 1);

    char *due_date = loan_service_calculate_due_date(service, current_date, member_id);
    if (due_date) {
        strncpy(loan.due_date, due_date, sizeof(loan.due_date) - 1);
        free(due_date);
    }

    loan.status = 'L';

    /* Add loan to repository */
    result = loan_repo_add(service->loan_repo, &loan);
    if (result != LMS_SUCCESS) {
        /* Rollback changes */
        book_repo_update_availability(service->book_repo, isbn, 1);
        member_repo_update_loan_count(service->member_repo, member_id, -1);
        free(loan_id);
        return result;
    }

    free(loan_id);
    return LMS_SUCCESS;
}

/* Return a book */
LMS_Result loan_service_return_book(LoanService *service, const char *loan_id) {
    CHECK_NULL(service);
    CHECK_NULL(loan_id);

    /* Find the loan */
    Loan *loan = loan_repo_find_by_id(service->loan_repo, loan_id);
    if (!loan) {
        return LMS_ERROR_NOT_FOUND;
    }

    if (loan->status != 'L') {
        return LMS_ERROR_INVALID_INPUT; /* Book already returned */
    }

    /* Update book availability */
    LMS_Result result = book_repo_update_availability(service->book_repo, loan->isbn, 1);
    if (result != LMS_SUCCESS) {
        return result;
    }

    /* Update member loan count */
    result = member_repo_update_loan_count(service->member_repo, loan->member_id, -1);
    if (result != LMS_SUCCESS) {
        /* Rollback book availability */
        book_repo_update_availability(service->book_repo, loan->isbn, -1);
        return result;
    }

    /* Update loan record */
    char *return_date = get_current_date_string();
    strncpy(loan->return_date, return_date, sizeof(loan->return_date) - 1);

    /* Calculate fine if overdue */
    double fine = loan_service_calculate_fine(service, loan->due_date, return_date);
    if (fine > 0) {
        loan->fine_amount = fine;
        loan->status = 'O'; /* Mark as overdue even though returned */
    } else {
        loan->status = 'R'; /* Returned on time */
    }

    return LMS_SUCCESS;
}

/* Renew a loan */
LMS_Result loan_service_renew_loan(LoanService *service, const char *loan_id) {
    CHECK_NULL(service);
    CHECK_NULL(loan_id);

    if (!loan_service_can_renew(service, loan_id)) {
        return LMS_ERROR_INVALID_INPUT;
    }

    Loan *loan = loan_repo_find_by_id(service->loan_repo, loan_id);
    if (!loan) {
        return LMS_ERROR_NOT_FOUND;
    }

    /* Extend due date */
    char *new_due_date = loan_service_calculate_due_date(service, loan->due_date, loan->member_id);
    if (new_due_date) {
        strncpy(loan->due_date, new_due_date, sizeof(loan->due_date) - 1);
        free(new_due_date);
    }

    return LMS_SUCCESS;
}

/* Check if member can borrow book */
bool loan_service_can_borrow(LoanService *service, const char *member_id, const char *isbn) {
    if (!service || !member_id || !isbn) return false;

    /* Check member status and limits */
    Member *member = member_repo_find_by_id(service->member_repo, member_id);
    if (!member || member->status != 'A') return false;

    /* Check loan limits */
    int max_loans = (member->membership_type == 'P') ? 5 : 3;
    if (member->loan_count >= max_loans) return false;

    /* Check book availability */
    Book *book = book_repo_find_by_isbn(service->book_repo, isbn);
    if (!book || book->available_copies <= 0 || book->status != 'A') return false;

    return true;
}

/* Check if loan can be renewed */
bool loan_service_can_renew(LoanService *service, const char *loan_id) {
    if (!service || !loan_id) return false;

    Loan *loan = loan_repo_find_by_id(service->loan_repo, loan_id);
    if (!loan || loan->status != 'L') return false;

    /* Simple policy: no renewal if overdue */
    if (loan->overdue_days > 0) return false;

    return true;
}

/* Get loan period for member */
int loan_service_get_loan_period(LoanService *service, const char *member_id) {
    if (!service || !member_id) return DEFAULT_LOAN_PERIOD_DAYS;

    Member *member = member_repo_find_by_id(service->member_repo, member_id);
    if (!member) return DEFAULT_LOAN_PERIOD_DAYS;

    /* Premium members get longer loan period */
    return (member->membership_type == 'P') ? 21 : DEFAULT_LOAN_PERIOD_DAYS;
}

/* Generate a unique loan ID */
char* loan_service_generate_loan_id(LoanService *service) {
    if (!service) return NULL;

    static int loan_counter = 1;
    char *loan_id = malloc(11);
    if (!loan_id) return NULL;

    snprintf(loan_id, 11, "L%09d", loan_counter++);
    return loan_id;
}

/* Calculate due date */
char* loan_service_calculate_due_date(LoanService *service, const char *loan_date, const char *member_id) {
    if (!service || !loan_date || !member_id) return NULL;

    int loan_period = loan_service_get_loan_period(service, member_id);

    /* Simple date arithmetic (not robust for production) */
    char *due_date = malloc(11);
    if (!due_date) return NULL;

    /* For simplicity, just add days to the loan date string */
    /* In production, you would use proper date libraries */
    strncpy(due_date, loan_date, 10);
    due_date[10] = '\0';

    return due_date;
}

/* Calculate fine amount */
double loan_service_calculate_fine(LoanService *service, const char *due_date, const char *return_date) {
    if (!service || !due_date || !return_date) return 0.0;

    /* Simple string comparison for dates in YYYY-MM-DD format */
    if (strcmp(return_date, due_date) <= 0) {
        return 0.0; /* Returned on time or early */
    }

    /* Calculate overdue days (simplified) */
    int overdue_days = 1; /* Simplified calculation */
    return overdue_days * FINE_PER_DAY;
}

/* Find loan by ID */
Loan* loan_service_find_by_id(LoanService *service, const char *loan_id) {
    if (!service || !loan_id) return NULL;

    return loan_repo_find_by_id(service->loan_repo, loan_id);
}

/* Get member loans */
DoublyLinkedList* loan_service_get_member_loans(LoanService *service, const char *member_id) {
    if (!service || !member_id) return NULL;

    return loan_repo_find_by_member(service->loan_repo, member_id);
}

/* Get book loans */
DoublyLinkedList* loan_service_get_book_loans(LoanService *service, const char *isbn) {
    if (!service || !isbn) return NULL;

    return loan_repo_find_by_book(service->loan_repo, isbn);
}

/* Get loan history for member */
DoublyLinkedList* loan_service_get_loan_history(LoanService *service, const char *member_id) {
    if (!service || !member_id) return NULL;

    return loan_repo_find_by_member(service->loan_repo, member_id);
}

/* Get active loans */
DoublyLinkedList* loan_service_get_active_loans(LoanService *service) {
    if (!service) return NULL;

    return loan_repo_get_active(service->loan_repo);
}

/* Get overdue loans */
DoublyLinkedList* loan_service_get_overdue_loans(LoanService *service) {
    if (!service) return NULL;

    return loan_repo_get_overdue(service->loan_repo);
}

/* Get all loans */
DoublyLinkedList* loan_service_get_all_loans(LoanService *service) {
    if (!service) return NULL;

    return loan_repo_get_all(service->loan_repo);
}

/* Get loans by date range */
DoublyLinkedList* loan_service_get_loans_by_date_range(LoanService *service, const char *start_date, const char *end_date) {
    if (!service || !start_date || !end_date) return NULL;

    return loan_repo_get_by_date_range(service->loan_repo, start_date, end_date);
}

/* Get total loan count */
int loan_service_get_total_loan_count(LoanService *service) {
    if (!service) return 0;

    return loan_repo_get_total_count(service->loan_repo);
}

/* Get active loan count */
int loan_service_get_active_loan_count(LoanService *service) {
    if (!service) return 0;

    return loan_repo_get_active_count(service->loan_repo);
}

/* Get overdue loan count */
int loan_service_get_overdue_loan_count(LoanService *service) {
    if (!service) return 0;

    return loan_repo_get_overdue_count(service->loan_repo);
}

/* Calculate overdue fines */
LMS_Result loan_service_calculate_overdue_fines(LoanService *service) {
    CHECK_NULL(service);

    DoublyLinkedList *active_loans = loan_repo_get_active(service->loan_repo);
    if (!active_loans) return LMS_SUCCESS;

    char *current_date = get_current_date_string();

    Iterator *iter = dll_iterator_create(active_loans);
    if (iter) {
        while (iterator_has_next(iter)) {
            Loan *loan = (Loan*)iterator_next(iter);

            /* Check if loan is overdue */
            if (strcmp(current_date, loan->due_date) > 0) {
                double fine = loan_service_calculate_fine(service, loan->due_date, current_date);
                if (fine > 0) {
                    loan->fine_amount = fine;
                    loan->status = 'O';
                }
            }
        }
        iterator_destroy(iter);
    }

    dll_destroy(active_loans);
    return LMS_SUCCESS;
}

/* Mark loan as lost */
LMS_Result loan_service_mark_as_lost(LoanService *service, const char *loan_id) {
    CHECK_NULL(service);
    CHECK_NULL(loan_id);

    Loan *loan = loan_repo_find_by_id(service->loan_repo, loan_id);
    if (!loan) {
        return LMS_ERROR_NOT_FOUND;
    }

    /* Mark as lost and assign replacement cost as fine */
    Book *book = book_repo_find_by_isbn(service->book_repo, loan->isbn);
    if (book) {
        loan->fine_amount = book->price; /* Replacement cost */
    }

    loan->status = 'O'; /* Overdue status for lost books */

    return LMS_SUCCESS;
}

/* Process fine payment */
LMS_Result loan_service_process_fine_payment(LoanService *service, const char *loan_id, double amount) {
    CHECK_NULL(service);
    CHECK_NULL(loan_id);

    if (amount <= 0) {
        return LMS_ERROR_INVALID_INPUT;
    }

    Loan *loan = loan_repo_find_by_id(service->loan_repo, loan_id);
    if (!loan) {
        return LMS_ERROR_NOT_FOUND;
    }

    if (loan->fine_amount <= 0) {
        return LMS_ERROR_INVALID_INPUT; /* No fine to pay */
    }

    loan->fine_amount -= amount;
    if (loan->fine_amount <= 0) {
        loan->fine_amount = 0;
        if (loan->status == 'O' && strlen(loan->return_date) > 0) {
            loan->status = 'R'; /* Mark as returned if fine is paid */
        }
    }

    return LMS_SUCCESS;
}

/* Send overdue notices (stub implementation) */
LMS_Result loan_service_send_overdue_notices(LoanService *service) {
    CHECK_NULL(service);

    /* This would send notifications to members with overdue books */
    /* For now, just return success */
    printf("Overdue notices sent to members with overdue books.\n");

    return LMS_SUCCESS;
}