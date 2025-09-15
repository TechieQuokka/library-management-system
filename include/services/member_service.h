#ifndef MEMBER_SERVICE_H
#define MEMBER_SERVICE_H

#include "../repositories/member_repository.h"
#include "../repositories/loan_repository.h"

/* Member Service structure */
typedef struct MemberService {
    MemberRepository *member_repo;
    LoanRepository *loan_repo;
} MemberService;

/* Loan limits */
#define MAX_LOANS_REGULAR 3
#define MAX_LOANS_PREMIUM 5

/* Service management */
MemberService* member_service_create(MemberRepository *member_repo, LoanRepository *loan_repo);
void member_service_destroy(MemberService *service);

/* Member management operations */
LMS_Result member_service_register_member(MemberService *service, const Member *member);
LMS_Result member_service_update_member(MemberService *service, const char *member_id, const Member *member);
LMS_Result member_service_deactivate_member(MemberService *service, const char *member_id);

/* Member status management */
LMS_Result member_service_suspend_member(MemberService *service, const char *member_id, const char *reason);
LMS_Result member_service_reactivate_member(MemberService *service, const char *member_id);
char member_service_get_member_status(MemberService *service, const char *member_id);

/* Loan authorization and limits */
bool member_service_can_borrow_book(MemberService *service, const char *member_id);
int member_service_get_remaining_loan_limit(MemberService *service, const char *member_id);
int member_service_get_max_loan_limit(MemberService *service, const char *member_id);
double member_service_get_outstanding_fines(MemberService *service, const char *member_id);

/* Search and query operations */
DoublyLinkedList* member_service_search(MemberService *service, const MemberSearchCriteria *criteria);
Member* member_service_find_by_id(MemberService *service, const char *member_id);
Member* member_service_find_by_email(MemberService *service, const char *email);
DoublyLinkedList* member_service_find_by_name(MemberService *service, const char *name);

/* Member collections */
DoublyLinkedList* member_service_get_all_members(MemberService *service);
DoublyLinkedList* member_service_get_active_members(MemberService *service);
DoublyLinkedList* member_service_get_suspended_members(MemberService *service);
DoublyLinkedList* member_service_get_members_with_overdues(MemberService *service);

/* Statistics */
int member_service_get_total_member_count(MemberService *service);
int member_service_get_active_member_count(MemberService *service);

/* Data validation */
bool member_service_validate_member_data(MemberService *service, const Member *member);
bool member_service_is_email_duplicate(MemberService *service, const char *email);
bool member_service_is_phone_duplicate(MemberService *service, const char *phone);

#endif /* MEMBER_SERVICE_H */