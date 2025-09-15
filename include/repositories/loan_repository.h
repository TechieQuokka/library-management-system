#ifndef LOAN_REPOSITORY_H
#define LOAN_REPOSITORY_H

#include "../models/models.h"
#include "../core/doubly_linked_list.h"

/* Loan Repository structure */
typedef struct LoanRepository {
    DoublyLinkedList *loans;        /* Main loan list */
    DoublyLinkedList *member_index; /* Member ID index */
    DoublyLinkedList *book_index;   /* Book ISBN index */
    DoublyLinkedList *date_index;   /* Date index */
} LoanRepository;

/* Repository management */
LoanRepository* loan_repository_create(void);
void loan_repository_destroy(LoanRepository *repo);

/* CRUD operations */
LMS_Result loan_repo_add(LoanRepository *repo, const Loan *loan);
Loan* loan_repo_find_by_id(LoanRepository *repo, const char *loan_id);
DoublyLinkedList* loan_repo_find_by_member(LoanRepository *repo, const char *member_id);
DoublyLinkedList* loan_repo_find_by_book(LoanRepository *repo, const char *isbn);
LMS_Result loan_repo_update(LoanRepository *repo, const char *loan_id, const Loan *updated_loan);
LMS_Result loan_repo_delete(LoanRepository *repo, const char *loan_id);

/* Loan status queries */
DoublyLinkedList* loan_repo_get_active(LoanRepository *repo);
DoublyLinkedList* loan_repo_get_overdue(LoanRepository *repo);
DoublyLinkedList* loan_repo_get_returned(LoanRepository *repo);

/* Loan management */
LMS_Result loan_repo_mark_returned(LoanRepository *repo, const char *loan_id, const char *return_date);
LMS_Result loan_repo_mark_overdue(LoanRepository *repo, const char *loan_id, int overdue_days, double fine);

/* Utility functions */
DoublyLinkedList* loan_repo_get_all(LoanRepository *repo);
DoublyLinkedList* loan_repo_get_by_date_range(LoanRepository *repo, const char *start_date, const char *end_date);
int loan_repo_get_total_count(LoanRepository *repo);
int loan_repo_get_active_count(LoanRepository *repo);
int loan_repo_get_overdue_count(LoanRepository *repo);

#endif /* LOAN_REPOSITORY_H */