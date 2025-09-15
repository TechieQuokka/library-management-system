#include "../../include/services/member_service.h"

/* Create a new member service */
MemberService* member_service_create(MemberRepository *member_repo, LoanRepository *loan_repo) {
    if (!member_repo || !loan_repo) return NULL;

    MemberService *service = malloc(sizeof(MemberService));
    if (!service) return NULL;

    service->member_repo = member_repo;
    service->loan_repo = loan_repo;

    return service;
}

/* Destroy the member service */
void member_service_destroy(MemberService *service) {
    if (service) {
        free(service);
    }
}

/* Register a new member */
LMS_Result member_service_register_member(MemberService *service, const Member *member) {
    CHECK_NULL(service);
    CHECK_NULL(member);

    /* Validate member data */
    if (!member_service_validate_member_data(service, member)) {
        return LMS_ERROR_INVALID_INPUT;
    }

    /* Check for duplicate email */
    if (strlen(member->email) > 0 && member_service_is_email_duplicate(service, member->email)) {
        return LMS_ERROR_DUPLICATE;
    }

    /* Check for duplicate phone */
    if (strlen(member->phone) > 0 && member_service_is_phone_duplicate(service, member->phone)) {
        return LMS_ERROR_DUPLICATE;
    }

    /* Add to repository */
    return member_repo_add(service->member_repo, member);
}

/* Update member information */
LMS_Result member_service_update_member(MemberService *service, const char *member_id, const Member *member) {
    CHECK_NULL(service);
    CHECK_NULL(member_id);
    CHECK_NULL(member);

    /* Validate member data */
    if (!member_service_validate_member_data(service, member)) {
        return LMS_ERROR_INVALID_INPUT;
    }

    /* Check if member exists */
    Member *existing = member_repo_find_by_id(service->member_repo, member_id);
    if (!existing) {
        return LMS_ERROR_NOT_FOUND;
    }

    /* Check for duplicate email (if changed) */
    if (strlen(member->email) > 0 && strcmp(existing->email, member->email) != 0) {
        if (member_service_is_email_duplicate(service, member->email)) {
            return LMS_ERROR_DUPLICATE;
        }
    }

    /* Update in repository */
    return member_repo_update(service->member_repo, member_id, member);
}

/* Deactivate a member */
LMS_Result member_service_deactivate_member(MemberService *service, const char *member_id) {
    CHECK_NULL(service);
    CHECK_NULL(member_id);

    /* Check if member has active loans */
    DoublyLinkedList *active_loans = loan_repo_find_by_member(service->loan_repo, member_id);
    if (active_loans && !dll_is_empty(active_loans)) {
        /* Count active loans */
        int active_count = 0;
        Iterator *iter = dll_iterator_create(active_loans);
        if (iter) {
            while (iterator_has_next(iter)) {
                Loan *loan = (Loan*)iterator_next(iter);
                if (loan->status == 'L') {
                    active_count++;
                }
            }
            iterator_destroy(iter);
        }
        dll_destroy(active_loans);

        if (active_count > 0) {
            return LMS_ERROR_LOAN_LIMIT; /* Member has active loans */
        }
    }
    if (active_loans) {
        dll_destroy(active_loans);
    }

    /* Mark member as deleted */
    Member *member = member_repo_find_by_id(service->member_repo, member_id);
    if (!member) {
        return LMS_ERROR_NOT_FOUND;
    }

    member->status = 'D';
    return LMS_SUCCESS;
}

/* Suspend a member */
LMS_Result member_service_suspend_member(MemberService *service, const char *member_id, const char *reason) {
    CHECK_NULL(service);
    CHECK_NULL(member_id);
    /* reason can be NULL */

    return member_repo_suspend_member(service->member_repo, member_id);
}

/* Reactivate a member */
LMS_Result member_service_reactivate_member(MemberService *service, const char *member_id) {
    CHECK_NULL(service);
    CHECK_NULL(member_id);

    return member_repo_activate_member(service->member_repo, member_id);
}

/* Get member status */
char member_service_get_member_status(MemberService *service, const char *member_id) {
    if (!service || !member_id) return '\0';

    Member *member = member_repo_find_by_id(service->member_repo, member_id);
    if (!member) return '\0';

    return member->status;
}

/* Check if member can borrow books */
bool member_service_can_borrow_book(MemberService *service, const char *member_id) {
    if (!service || !member_id) return false;

    Member *member = member_repo_find_by_id(service->member_repo, member_id);
    if (!member) return false;

    /* Check if member is active */
    if (member->status != 'A') return false;

    /* Check loan limit */
    int remaining_limit = member_service_get_remaining_loan_limit(service, member_id);
    if (remaining_limit <= 0) return false;

    /* Check outstanding fines */
    double fines = member_service_get_outstanding_fines(service, member_id);
    if (fines > 0) return false; /* No borrowing with outstanding fines */

    return true;
}

/* Get remaining loan limit */
int member_service_get_remaining_loan_limit(MemberService *service, const char *member_id) {
    if (!service || !member_id) return 0;

    Member *member = member_repo_find_by_id(service->member_repo, member_id);
    if (!member) return 0;

    int max_limit = member_service_get_max_loan_limit(service, member_id);
    return max_limit - member->loan_count;
}

/* Get maximum loan limit based on membership type */
int member_service_get_max_loan_limit(MemberService *service, const char *member_id) {
    if (!service || !member_id) return 0;

    Member *member = member_repo_find_by_id(service->member_repo, member_id);
    if (!member) return 0;

    return (member->membership_type == 'P') ? MAX_LOANS_PREMIUM : MAX_LOANS_REGULAR;
}

/* Get outstanding fines for a member */
double member_service_get_outstanding_fines(MemberService *service, const char *member_id) {
    if (!service || !member_id) return 0.0;

    DoublyLinkedList *member_loans = loan_repo_find_by_member(service->loan_repo, member_id);
    if (!member_loans) return 0.0;

    double total_fines = 0.0;
    Iterator *iter = dll_iterator_create(member_loans);
    if (iter) {
        while (iterator_has_next(iter)) {
            Loan *loan = (Loan*)iterator_next(iter);
            if (loan->status == 'O' || loan->fine_amount > 0) {
                total_fines += loan->fine_amount;
            }
        }
        iterator_destroy(iter);
    }

    dll_destroy(member_loans);
    return total_fines;
}

/* Search members with criteria */
DoublyLinkedList* member_service_search(MemberService *service, const MemberSearchCriteria *criteria) {
    if (!service || !criteria) return NULL;

    return member_repo_search(service->member_repo, criteria);
}

/* Find member by ID */
Member* member_service_find_by_id(MemberService *service, const char *member_id) {
    if (!service || !member_id) return NULL;

    return member_repo_find_by_id(service->member_repo, member_id);
}

/* Find member by email */
Member* member_service_find_by_email(MemberService *service, const char *email) {
    if (!service || !email) return NULL;

    return member_repo_find_by_email(service->member_repo, email);
}

/* Find members by name */
DoublyLinkedList* member_service_find_by_name(MemberService *service, const char *name) {
    if (!service || !name) return NULL;

    return member_repo_find_by_name(service->member_repo, name);
}

/* Get all members */
DoublyLinkedList* member_service_get_all_members(MemberService *service) {
    if (!service) return NULL;

    return member_repo_get_all(service->member_repo);
}

/* Get active members */
DoublyLinkedList* member_service_get_active_members(MemberService *service) {
    if (!service) return NULL;

    return member_repo_get_active(service->member_repo);
}

/* Get suspended members */
DoublyLinkedList* member_service_get_suspended_members(MemberService *service) {
    if (!service) return NULL;

    return member_repo_get_suspended(service->member_repo);
}

/* Get members with overdue loans */
DoublyLinkedList* member_service_get_members_with_overdues(MemberService *service) {
    if (!service) return NULL;

    DoublyLinkedList *overdue_loans = loan_repo_get_overdue(service->loan_repo);
    if (!overdue_loans) return NULL;

    DoublyLinkedList *members_with_overdues = dll_create(sizeof(Member), compare_member_id, print_member);
    if (!members_with_overdues) {
        dll_destroy(overdue_loans);
        return NULL;
    }

    Iterator *iter = dll_iterator_create(overdue_loans);
    if (iter) {
        while (iterator_has_next(iter)) {
            Loan *loan = (Loan*)iterator_next(iter);
            Member *member = member_repo_find_by_id(service->member_repo, loan->member_id);
            if (member) {
                /* Check if member is already in the result list */
                if (!dll_search(members_with_overdues, member)) {
                    dll_insert_rear(members_with_overdues, member);
                }
            }
        }
        iterator_destroy(iter);
    }

    dll_destroy(overdue_loans);
    return members_with_overdues;
}

/* Get total member count */
int member_service_get_total_member_count(MemberService *service) {
    if (!service) return 0;

    return member_repo_get_total_count(service->member_repo);
}

/* Get active member count */
int member_service_get_active_member_count(MemberService *service) {
    if (!service) return 0;

    return member_repo_get_active_count(service->member_repo);
}

/* Validate member data */
bool member_service_validate_member_data(MemberService *service, const Member *member) {
    if (!service || !member) return false;

    return validate_member(member);
}

/* Check if email is duplicate */
bool member_service_is_email_duplicate(MemberService *service, const char *email) {
    if (!service || !email) return false;

    Member *existing = member_repo_find_by_email(service->member_repo, email);
    return (existing != NULL);
}

/* Check if phone is duplicate */
bool member_service_is_phone_duplicate(MemberService *service, const char *phone) {
    if (!service || !phone) return false;

    Member *existing = member_repo_find_by_phone(service->member_repo, phone);
    return (existing != NULL);
}