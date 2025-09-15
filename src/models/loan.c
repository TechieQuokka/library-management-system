#include "../../include/models/models.h"

/* Compare loans by ID */
int compare_loan_id(const void *a, const void *b) {
    const Loan *loan_a = (const Loan *)a;
    const Loan *loan_b = (const Loan *)b;
    return strcmp(loan_a->loan_id, loan_b->loan_id);
}

/* Print loan information */
void print_loan(const void *data) {
    const Loan *loan = (const Loan *)data;
    printf("Loan ID: %s | Member: %s | ISBN: %s | Due: %s | Status: %c\n",
           loan->loan_id, loan->member_id, loan->isbn,
           loan->due_date, loan->status);
}

/* Initialize loan structure */
void loan_init(Loan *loan) {
    if (!loan) return;

    memset(loan->loan_id, 0, sizeof(loan->loan_id));
    memset(loan->member_id, 0, sizeof(loan->member_id));
    memset(loan->isbn, 0, sizeof(loan->isbn));
    memset(loan->loan_date, 0, sizeof(loan->loan_date));
    memset(loan->due_date, 0, sizeof(loan->due_date));
    memset(loan->return_date, 0, sizeof(loan->return_date));
    loan->overdue_days = 0;
    loan->fine_amount = 0.0;
    loan->status = 'L';
}

/* Validate loan data */
bool validate_loan(const Loan *loan) {
    if (!loan) return false;

    /* Validate loan ID */
    if (strlen(loan->loan_id) == 0 || strlen(loan->loan_id) > 10) return false;

    /* Validate member ID */
    if (strlen(loan->member_id) == 0 || strlen(loan->member_id) > 10) return false;

    /* Validate ISBN */
    if (!validate_isbn(loan->isbn)) return false;

    /* Validate loan date */
    if (!validate_date(loan->loan_date)) return false;

    /* Validate due date */
    if (!validate_date(loan->due_date)) return false;

    /* Validate return date if provided */
    if (strlen(loan->return_date) > 0 && !validate_date(loan->return_date)) return false;

    /* Validate overdue days */
    if (loan->overdue_days < 0) return false;

    /* Validate fine amount */
    if (loan->fine_amount < 0) return false;

    /* Validate status */
    if (loan->status != 'L' && loan->status != 'R' && loan->status != 'O') return false;

    return true;
}

/* Validate date format (YYYY-MM-DD) */
bool validate_date(const char *date) {
    if (!date) return false;

    /* Check length */
    if (strlen(date) != 10) return false;

    /* Check format YYYY-MM-DD */
    if (date[4] != '-' || date[7] != '-') return false;

    /* Check if year, month, day are digits */
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue; /* Skip hyphens */
        if (date[i] < '0' || date[i] > '9') return false;
    }

    /* Extract year, month, day */
    int year = (date[0] - '0') * 1000 + (date[1] - '0') * 100 +
               (date[2] - '0') * 10 + (date[3] - '0');
    int month = (date[5] - '0') * 10 + (date[6] - '0');
    int day = (date[8] - '0') * 10 + (date[9] - '0');

    /* Validate ranges */
    if (year < 1900 || year > 2100) return false;
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > 31) return false;

    /* Simple month-day validation */
    if (month == 2 && day > 29) return false;
    if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) return false;

    return true;
}