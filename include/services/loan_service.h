#ifndef LOAN_SERVICE_H
#define LOAN_SERVICE_H

#include "../repositories/loan_repository.h"
#include "../repositories/book_repository.h"
#include "../repositories/member_repository.h"

/* Loan Service structure */
typedef struct LoanService {
    LoanRepository *loan_repo;
    BookRepository *book_repo;
    MemberRepository *member_repo;
} LoanService;

/* Loan policies */
#define DEFAULT_LOAN_PERIOD_DAYS 14
#define FINE_PER_DAY 1.0

/* Service management */
LoanService* loan_service_create(LoanRepository *loan_repo, BookRepository *book_repo, MemberRepository *member_repo);
void loan_service_destroy(LoanService *service);

/* Loan processing operations */
LMS_Result loan_service_borrow_book(LoanService *service, const char *member_id, const char *isbn);
LMS_Result loan_service_return_book(LoanService *service, const char *loan_id);
LMS_Result loan_service_renew_loan(LoanService *service, const char *loan_id);

/* Loan status management */
LMS_Result loan_service_mark_as_lost(LoanService *service, const char *loan_id);
LMS_Result loan_service_process_fine_payment(LoanService *service, const char *loan_id, double amount);

/* Overdue management */
DoublyLinkedList* loan_service_get_overdue_loans(LoanService *service);
LMS_Result loan_service_calculate_overdue_fines(LoanService *service);
LMS_Result loan_service_send_overdue_notices(LoanService *service);

/* Loan validation */
bool loan_service_can_borrow(LoanService *service, const char *member_id, const char *isbn);
bool loan_service_can_renew(LoanService *service, const char *loan_id);
int loan_service_get_loan_period(LoanService *service, const char *member_id);

/* Search and query operations */
Loan* loan_service_find_by_id(LoanService *service, const char *loan_id);
DoublyLinkedList* loan_service_get_member_loans(LoanService *service, const char *member_id);
DoublyLinkedList* loan_service_get_book_loans(LoanService *service, const char *isbn);
DoublyLinkedList* loan_service_get_loan_history(LoanService *service, const char *member_id);

/* Loan collections */
DoublyLinkedList* loan_service_get_active_loans(LoanService *service);
DoublyLinkedList* loan_service_get_all_loans(LoanService *service);
DoublyLinkedList* loan_service_get_loans_by_date_range(LoanService *service, const char *start_date, const char *end_date);

/* Statistics */
int loan_service_get_total_loan_count(LoanService *service);
int loan_service_get_active_loan_count(LoanService *service);
int loan_service_get_overdue_loan_count(LoanService *service);

/* Utility functions */
char* loan_service_generate_loan_id(LoanService *service);
char* loan_service_calculate_due_date(LoanService *service, const char *loan_date, const char *member_id);
double loan_service_calculate_fine(LoanService *service, const char *due_date, const char *return_date);

#endif /* LOAN_SERVICE_H */