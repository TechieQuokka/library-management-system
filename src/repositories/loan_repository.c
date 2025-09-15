#include "../../include/repositories/loan_repository.h"

/* Create a new loan repository */
LoanRepository* loan_repository_create(void) {
    LoanRepository *repo = malloc(sizeof(LoanRepository));
    if (!repo) return NULL;

    repo->loans = dll_create(sizeof(Loan), compare_loan_id, print_loan);
    if (!repo->loans) {
        free(repo);
        return NULL;
    }

    /* Initialize indexes */
    repo->member_index = dll_create(sizeof(Loan*), NULL, NULL);
    repo->book_index = dll_create(sizeof(Loan*), NULL, NULL);
    repo->date_index = dll_create(sizeof(Loan*), NULL, NULL);

    if (!repo->member_index || !repo->book_index || !repo->date_index) {
        loan_repository_destroy(repo);
        return NULL;
    }

    return repo;
}

/* Destroy the loan repository */
void loan_repository_destroy(LoanRepository *repo) {
    if (!repo) return;

    dll_destroy(repo->loans);
    dll_destroy(repo->member_index);
    dll_destroy(repo->book_index);
    dll_destroy(repo->date_index);
    free(repo);
}

/* Add a loan to the repository */
LMS_Result loan_repo_add(LoanRepository *repo, const Loan *loan) {
    CHECK_NULL(repo);
    CHECK_NULL(loan);

    if (!validate_loan(loan)) {
        return LMS_ERROR_INVALID_INPUT;
    }

    /* Check for duplicate loan ID */
    if (loan_repo_find_by_id(repo, loan->loan_id)) {
        return LMS_ERROR_DUPLICATE;
    }

    /* Add to main list (sorted by loan ID) */
    LMS_Result result = dll_insert_sorted(repo->loans, loan);
    if (result != LMS_SUCCESS) {
        return result;
    }

    return LMS_SUCCESS;
}

/* Find loan by ID */
Loan* loan_repo_find_by_id(LoanRepository *repo, const char *loan_id) {
    if (!repo || !loan_id) return NULL;

    Loan search_loan;
    loan_init(&search_loan);
    strncpy(search_loan.loan_id, loan_id, sizeof(search_loan.loan_id) - 1);

    Node *found = dll_search(repo->loans, &search_loan);
    return found ? (Loan*)found->data : NULL;
}

/* Helper function for member search */
static bool loan_member_matches(const void *data, void *context) {
    const Loan *loan = (const Loan *)data;
    const char *search_member_id = (const char *)context;
    return strcmp(loan->member_id, search_member_id) == 0;
}

/* Find loans by member ID */
DoublyLinkedList* loan_repo_find_by_member(LoanRepository *repo, const char *member_id) {
    if (!repo || !member_id) return NULL;

    DoublyLinkedList *results = dll_create(sizeof(Loan), compare_loan_id, print_loan);
    if (!results) return NULL;

    Iterator *iter = dll_iterator_create(repo->loans);
    if (!iter) {
        dll_destroy(results);
        return NULL;
    }

    while (iterator_has_next(iter)) {
        Loan *loan = (Loan*)iterator_next(iter);
        if (loan_member_matches(loan, (void*)member_id)) {
            dll_insert_rear(results, loan);
        }
    }

    iterator_destroy(iter);
    return results;
}

/* Helper function for book search */
static bool loan_book_matches(const void *data, void *context) {
    const Loan *loan = (const Loan *)data;
    const char *search_isbn = (const char *)context;
    return strcmp(loan->isbn, search_isbn) == 0;
}

/* Find loans by book ISBN */
DoublyLinkedList* loan_repo_find_by_book(LoanRepository *repo, const char *isbn) {
    if (!repo || !isbn) return NULL;

    DoublyLinkedList *results = dll_create(sizeof(Loan), compare_loan_id, print_loan);
    if (!results) return NULL;

    Iterator *iter = dll_iterator_create(repo->loans);
    if (!iter) {
        dll_destroy(results);
        return NULL;
    }

    while (iterator_has_next(iter)) {
        Loan *loan = (Loan*)iterator_next(iter);
        if (loan_book_matches(loan, (void*)isbn)) {
            dll_insert_rear(results, loan);
        }
    }

    iterator_destroy(iter);
    return results;
}

/* Update loan information */
LMS_Result loan_repo_update(LoanRepository *repo, const char *loan_id, const Loan *updated_loan) {
    CHECK_NULL(repo);
    CHECK_NULL(loan_id);
    CHECK_NULL(updated_loan);

    if (!validate_loan(updated_loan)) {
        return LMS_ERROR_INVALID_INPUT;
    }

    /* Find the existing loan */
    Loan *existing_loan = loan_repo_find_by_id(repo, loan_id);
    if (!existing_loan) {
        return LMS_ERROR_NOT_FOUND;
    }

    /* Update the loan data */
    memcpy(existing_loan, updated_loan, sizeof(Loan));

    return LMS_SUCCESS;
}

/* Delete a loan */
LMS_Result loan_repo_delete(LoanRepository *repo, const char *loan_id) {
    CHECK_NULL(repo);
    CHECK_NULL(loan_id);

    Loan search_loan;
    loan_init(&search_loan);
    strncpy(search_loan.loan_id, loan_id, sizeof(search_loan.loan_id) - 1);

    LMS_Result result = dll_delete_data(repo->loans, &search_loan);
    return result;
}

/* Helper function for active loans */
static bool loan_is_active(const void *data, void *context) {
    const Loan *loan = (const Loan *)data;
    return loan->status == 'L';
}

/* Get active loans */
DoublyLinkedList* loan_repo_get_active(LoanRepository *repo) {
    if (!repo) return NULL;

    DoublyLinkedList *results = dll_create(sizeof(Loan), compare_loan_id, print_loan);
    if (!results) return NULL;

    Iterator *iter = dll_iterator_create(repo->loans);
    if (!iter) {
        dll_destroy(results);
        return NULL;
    }

    while (iterator_has_next(iter)) {
        Loan *loan = (Loan*)iterator_next(iter);
        if (loan_is_active(loan, NULL)) {
            dll_insert_rear(results, loan);
        }
    }

    iterator_destroy(iter);
    return results;
}

/* Helper function for overdue loans */
static bool loan_is_overdue(const void *data, void *context) {
    const Loan *loan = (const Loan *)data;
    return loan->status == 'O' || loan->overdue_days > 0;
}

/* Get overdue loans */
DoublyLinkedList* loan_repo_get_overdue(LoanRepository *repo) {
    if (!repo) return NULL;

    DoublyLinkedList *results = dll_create(sizeof(Loan), compare_loan_id, print_loan);
    if (!results) return NULL;

    Iterator *iter = dll_iterator_create(repo->loans);
    if (!iter) {
        dll_destroy(results);
        return NULL;
    }

    while (iterator_has_next(iter)) {
        Loan *loan = (Loan*)iterator_next(iter);
        if (loan_is_overdue(loan, NULL)) {
            dll_insert_rear(results, loan);
        }
    }

    iterator_destroy(iter);
    return results;
}

/* Helper function for returned loans */
static bool loan_is_returned(const void *data, void *context) {
    const Loan *loan = (const Loan *)data;
    return loan->status == 'R';
}

/* Get returned loans */
DoublyLinkedList* loan_repo_get_returned(LoanRepository *repo) {
    if (!repo) return NULL;

    DoublyLinkedList *results = dll_create(sizeof(Loan), compare_loan_id, print_loan);
    if (!results) return NULL;

    Iterator *iter = dll_iterator_create(repo->loans);
    if (!iter) {
        dll_destroy(results);
        return NULL;
    }

    while (iterator_has_next(iter)) {
        Loan *loan = (Loan*)iterator_next(iter);
        if (loan_is_returned(loan, NULL)) {
            dll_insert_rear(results, loan);
        }
    }

    iterator_destroy(iter);
    return results;
}

/* Mark loan as returned */
LMS_Result loan_repo_mark_returned(LoanRepository *repo, const char *loan_id, const char *return_date) {
    CHECK_NULL(repo);
    CHECK_NULL(loan_id);
    CHECK_NULL(return_date);

    if (!validate_date(return_date)) {
        return LMS_ERROR_INVALID_INPUT;
    }

    Loan *loan = loan_repo_find_by_id(repo, loan_id);
    if (!loan) {
        return LMS_ERROR_NOT_FOUND;
    }

    strncpy(loan->return_date, return_date, sizeof(loan->return_date) - 1);
    loan->status = 'R';

    return LMS_SUCCESS;
}

/* Mark loan as overdue */
LMS_Result loan_repo_mark_overdue(LoanRepository *repo, const char *loan_id, int overdue_days, double fine) {
    CHECK_NULL(repo);
    CHECK_NULL(loan_id);

    if (overdue_days < 0 || fine < 0) {
        return LMS_ERROR_INVALID_INPUT;
    }

    Loan *loan = loan_repo_find_by_id(repo, loan_id);
    if (!loan) {
        return LMS_ERROR_NOT_FOUND;
    }

    loan->overdue_days = overdue_days;
    loan->fine_amount = fine;
    loan->status = 'O';

    return LMS_SUCCESS;
}

/* Get all loans */
DoublyLinkedList* loan_repo_get_all(LoanRepository *repo) {
    if (!repo) return NULL;
    return dll_clone(repo->loans);
}

/* Helper function for date range search */
static bool loan_in_date_range(const void *data, void *context) {
    const Loan *loan = (const Loan *)data;
    char **dates = (char **)context;
    const char *start_date = dates[0];
    const char *end_date = dates[1];

    /* Simple string comparison for dates in YYYY-MM-DD format */
    return (strcmp(loan->loan_date, start_date) >= 0 &&
            strcmp(loan->loan_date, end_date) <= 0);
}

/* Get loans by date range */
DoublyLinkedList* loan_repo_get_by_date_range(LoanRepository *repo, const char *start_date, const char *end_date) {
    if (!repo || !start_date || !end_date) return NULL;

    if (!validate_date(start_date) || !validate_date(end_date)) return NULL;

    DoublyLinkedList *results = dll_create(sizeof(Loan), compare_loan_id, print_loan);
    if (!results) return NULL;

    char *dates[2] = {(char*)start_date, (char*)end_date};

    Iterator *iter = dll_iterator_create(repo->loans);
    if (!iter) {
        dll_destroy(results);
        return NULL;
    }

    while (iterator_has_next(iter)) {
        Loan *loan = (Loan*)iterator_next(iter);
        if (loan_in_date_range(loan, dates)) {
            dll_insert_rear(results, loan);
        }
    }

    iterator_destroy(iter);
    return results;
}

/* Get total loan count */
int loan_repo_get_total_count(LoanRepository *repo) {
    return repo ? dll_size(repo->loans) : 0;
}

/* Get active loan count */
int loan_repo_get_active_count(LoanRepository *repo) {
    if (!repo) return 0;

    int count = 0;
    Iterator *iter = dll_iterator_create(repo->loans);
    if (!iter) return 0;

    while (iterator_has_next(iter)) {
        Loan *loan = (Loan*)iterator_next(iter);
        if (loan_is_active(loan, NULL)) {
            count++;
        }
    }

    iterator_destroy(iter);
    return count;
}

/* Get overdue loan count */
int loan_repo_get_overdue_count(LoanRepository *repo) {
    if (!repo) return 0;

    int count = 0;
    Iterator *iter = dll_iterator_create(repo->loans);
    if (!iter) return 0;

    while (iterator_has_next(iter)) {
        Loan *loan = (Loan*)iterator_next(iter);
        if (loan_is_overdue(loan, NULL)) {
            count++;
        }
    }

    iterator_destroy(iter);
    return count;
}