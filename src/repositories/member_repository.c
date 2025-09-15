#include "../../include/repositories/member_repository.h"

/* Create a new member repository */
MemberRepository* member_repository_create(void) {
    MemberRepository *repo = malloc(sizeof(MemberRepository));
    if (!repo) return NULL;

    repo->members = dll_create(sizeof(Member), compare_member_id, print_member);
    if (!repo->members) {
        free(repo);
        return NULL;
    }

    /* Initialize indexes */
    repo->id_index = dll_create(sizeof(Member*), NULL, NULL);
    repo->email_index = dll_create(sizeof(Member*), NULL, NULL);
    repo->phone_index = dll_create(sizeof(Member*), NULL, NULL);

    if (!repo->id_index || !repo->email_index || !repo->phone_index) {
        member_repository_destroy(repo);
        return NULL;
    }

    return repo;
}

/* Destroy the member repository */
void member_repository_destroy(MemberRepository *repo) {
    if (!repo) return;

    dll_destroy(repo->members);
    dll_destroy(repo->id_index);
    dll_destroy(repo->email_index);
    dll_destroy(repo->phone_index);
    free(repo);
}

/* Add a member to the repository */
LMS_Result member_repo_add(MemberRepository *repo, const Member *member) {
    CHECK_NULL(repo);
    CHECK_NULL(member);

    if (!validate_member(member)) {
        return LMS_ERROR_INVALID_INPUT;
    }

    /* Check for duplicate member ID */
    if (member_repo_find_by_id(repo, member->member_id)) {
        return LMS_ERROR_DUPLICATE;
    }

    /* Check for duplicate email if provided */
    if (strlen(member->email) > 0 && member_repo_find_by_email(repo, member->email)) {
        return LMS_ERROR_DUPLICATE;
    }

    /* Add to main list (sorted by member ID) */
    LMS_Result result = dll_insert_sorted(repo->members, member);
    if (result != LMS_SUCCESS) {
        return result;
    }

    return LMS_SUCCESS;
}

/* Find member by ID */
Member* member_repo_find_by_id(MemberRepository *repo, const char *member_id) {
    if (!repo || !member_id) return NULL;

    Member search_member;
    member_init(&search_member);
    strncpy(search_member.member_id, member_id, sizeof(search_member.member_id) - 1);

    Node *found = dll_search(repo->members, &search_member);
    return found ? (Member*)found->data : NULL;
}

/* Helper function for email search */
static bool member_email_matches(const void *data, void *context) {
    const Member *member = (const Member *)data;
    const char *search_email = (const char *)context;
    return strcmp(member->email, search_email) == 0;
}

/* Find member by email */
Member* member_repo_find_by_email(MemberRepository *repo, const char *email) {
    if (!repo || !email) return NULL;

    Node *found = dll_find_if(repo->members, member_email_matches, (void*)email);
    return found ? (Member*)found->data : NULL;
}

/* Helper function for phone search */
static bool member_phone_matches(const void *data, void *context) {
    const Member *member = (const Member *)data;
    const char *search_phone = (const char *)context;
    return strcmp(member->phone, search_phone) == 0;
}

/* Find member by phone */
Member* member_repo_find_by_phone(MemberRepository *repo, const char *phone) {
    if (!repo || !phone) return NULL;

    Node *found = dll_find_if(repo->members, member_phone_matches, (void*)phone);
    return found ? (Member*)found->data : NULL;
}

/* Helper function for name search */
static bool member_name_contains(const void *data, void *context) {
    const Member *member = (const Member *)data;
    const char *search_name = (const char *)context;

    /* Convert both to lowercase for case-insensitive search */
    char member_name_lower[51];
    char search_lower[51];

    strncpy(member_name_lower, member->name, sizeof(member_name_lower) - 1);
    strncpy(search_lower, search_name, sizeof(search_lower) - 1);

    /* Simple lowercase conversion */
    for (int i = 0; member_name_lower[i]; i++) {
        if (member_name_lower[i] >= 'A' && member_name_lower[i] <= 'Z') {
            member_name_lower[i] += 32;
        }
    }
    for (int i = 0; search_lower[i]; i++) {
        if (search_lower[i] >= 'A' && search_lower[i] <= 'Z') {
            search_lower[i] += 32;
        }
    }

    return strstr(member_name_lower, search_lower) != NULL;
}

/* Find members by name (partial match) */
DoublyLinkedList* member_repo_find_by_name(MemberRepository *repo, const char *name) {
    if (!repo || !name) return NULL;

    DoublyLinkedList *results = dll_create(sizeof(Member), compare_member_name, print_member);
    if (!results) return NULL;

    Iterator *iter = dll_iterator_create(repo->members);
    if (!iter) {
        dll_destroy(results);
        return NULL;
    }

    while (iterator_has_next(iter)) {
        Member *member = (Member*)iterator_next(iter);
        if (member_name_contains(member, (void*)name)) {
            dll_insert_rear(results, member);
        }
    }

    iterator_destroy(iter);
    return results;
}

/* Update member information */
LMS_Result member_repo_update(MemberRepository *repo, const char *member_id, const Member *updated_member) {
    CHECK_NULL(repo);
    CHECK_NULL(member_id);
    CHECK_NULL(updated_member);

    if (!validate_member(updated_member)) {
        return LMS_ERROR_INVALID_INPUT;
    }

    /* Find the existing member */
    Member *existing_member = member_repo_find_by_id(repo, member_id);
    if (!existing_member) {
        return LMS_ERROR_NOT_FOUND;
    }

    /* Update the member data */
    memcpy(existing_member, updated_member, sizeof(Member));

    return LMS_SUCCESS;
}

/* Delete a member */
LMS_Result member_repo_delete(MemberRepository *repo, const char *member_id) {
    CHECK_NULL(repo);
    CHECK_NULL(member_id);

    Member search_member;
    member_init(&search_member);
    strncpy(search_member.member_id, member_id, sizeof(search_member.member_id) - 1);

    LMS_Result result = dll_delete_data(repo->members, &search_member);
    return result;
}

/* Advanced search with multiple criteria */
DoublyLinkedList* member_repo_search(MemberRepository *repo, const MemberSearchCriteria *criteria) {
    if (!repo || !criteria) return NULL;

    DoublyLinkedList *results = dll_create(sizeof(Member), compare_member_name, print_member);
    if (!results) return NULL;

    Iterator *iter = dll_iterator_create(repo->members);
    if (!iter) {
        dll_destroy(results);
        return NULL;
    }

    while (iterator_has_next(iter)) {
        Member *member = (Member*)iterator_next(iter);
        bool matches = true;

        /* Check name criteria */
        if (criteria->search_by_name) {
            if (!member_name_contains(member, (void*)criteria->name)) {
                matches = false;
            }
        }

        /* Check email criteria */
        if (matches && criteria->search_by_email) {
            if (strcmp(member->email, criteria->email) != 0) {
                matches = false;
            }
        }

        /* Check phone criteria */
        if (matches && criteria->search_by_phone) {
            if (strcmp(member->phone, criteria->phone) != 0) {
                matches = false;
            }
        }

        /* Check active criteria */
        if (matches && criteria->only_active) {
            if (member->status != 'A') {
                matches = false;
            }
        }

        if (matches) {
            dll_insert_rear(results, member);
        }
    }

    iterator_destroy(iter);
    return results;
}

/* Suspend a member */
LMS_Result member_repo_suspend_member(MemberRepository *repo, const char *member_id) {
    CHECK_NULL(repo);
    CHECK_NULL(member_id);

    Member *member = member_repo_find_by_id(repo, member_id);
    if (!member) {
        return LMS_ERROR_NOT_FOUND;
    }

    member->status = 'S';
    return LMS_SUCCESS;
}

/* Activate a member */
LMS_Result member_repo_activate_member(MemberRepository *repo, const char *member_id) {
    CHECK_NULL(repo);
    CHECK_NULL(member_id);

    Member *member = member_repo_find_by_id(repo, member_id);
    if (!member) {
        return LMS_ERROR_NOT_FOUND;
    }

    member->status = 'A';
    return LMS_SUCCESS;
}

/* Get all members */
DoublyLinkedList* member_repo_get_all(MemberRepository *repo) {
    if (!repo) return NULL;
    return dll_clone(repo->members);
}

/* Helper function for active members */
static bool member_is_active(const void *data, void *context) {
    const Member *member = (const Member *)data;
    return member->status == 'A';
}

/* Get active members */
DoublyLinkedList* member_repo_get_active(MemberRepository *repo) {
    if (!repo) return NULL;

    DoublyLinkedList *results = dll_create(sizeof(Member), compare_member_name, print_member);
    if (!results) return NULL;

    Iterator *iter = dll_iterator_create(repo->members);
    if (!iter) {
        dll_destroy(results);
        return NULL;
    }

    while (iterator_has_next(iter)) {
        Member *member = (Member*)iterator_next(iter);
        if (member_is_active(member, NULL)) {
            dll_insert_rear(results, member);
        }
    }

    iterator_destroy(iter);
    return results;
}

/* Helper function for suspended members */
static bool member_is_suspended(const void *data, void *context) {
    const Member *member = (const Member *)data;
    return member->status == 'S';
}

/* Get suspended members */
DoublyLinkedList* member_repo_get_suspended(MemberRepository *repo) {
    if (!repo) return NULL;

    DoublyLinkedList *results = dll_create(sizeof(Member), compare_member_name, print_member);
    if (!results) return NULL;

    Iterator *iter = dll_iterator_create(repo->members);
    if (!iter) {
        dll_destroy(results);
        return NULL;
    }

    while (iterator_has_next(iter)) {
        Member *member = (Member*)iterator_next(iter);
        if (member_is_suspended(member, NULL)) {
            dll_insert_rear(results, member);
        }
    }

    iterator_destroy(iter);
    return results;
}

/* Update member loan count */
LMS_Result member_repo_update_loan_count(MemberRepository *repo, const char *member_id, int change) {
    CHECK_NULL(repo);
    CHECK_NULL(member_id);

    Member *member = member_repo_find_by_id(repo, member_id);
    if (!member) {
        return LMS_ERROR_NOT_FOUND;
    }

    int new_count = member->loan_count + change;
    if (new_count < 0) {
        return LMS_ERROR_INVALID_INPUT;
    }

    member->loan_count = new_count;
    return LMS_SUCCESS;
}

/* Get total member count */
int member_repo_get_total_count(MemberRepository *repo) {
    return repo ? dll_size(repo->members) : 0;
}

/* Get active member count */
int member_repo_get_active_count(MemberRepository *repo) {
    if (!repo) return 0;

    int count = 0;
    Iterator *iter = dll_iterator_create(repo->members);
    if (!iter) return 0;

    while (iterator_has_next(iter)) {
        Member *member = (Member*)iterator_next(iter);
        if (member_is_active(member, NULL)) {
            count++;
        }
    }

    iterator_destroy(iter);
    return count;
}