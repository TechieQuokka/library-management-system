#ifndef MEMBER_REPOSITORY_H
#define MEMBER_REPOSITORY_H

#include "../models/models.h"
#include "../core/doubly_linked_list.h"

/* Member Repository structure */
typedef struct MemberRepository {
    DoublyLinkedList *members;      /* Main member list */
    DoublyLinkedList *id_index;     /* Member ID index */
    DoublyLinkedList *email_index;  /* Email index */
    DoublyLinkedList *phone_index;  /* Phone index */
} MemberRepository;

/* Repository management */
MemberRepository* member_repository_create(void);
void member_repository_destroy(MemberRepository *repo);

/* CRUD operations */
LMS_Result member_repo_add(MemberRepository *repo, const Member *member);
Member* member_repo_find_by_id(MemberRepository *repo, const char *member_id);
Member* member_repo_find_by_email(MemberRepository *repo, const char *email);
Member* member_repo_find_by_phone(MemberRepository *repo, const char *phone);
DoublyLinkedList* member_repo_find_by_name(MemberRepository *repo, const char *name);
LMS_Result member_repo_update(MemberRepository *repo, const char *member_id, const Member *updated_member);
LMS_Result member_repo_delete(MemberRepository *repo, const char *member_id);

/* Advanced search */
DoublyLinkedList* member_repo_search(MemberRepository *repo, const MemberSearchCriteria *criteria);

/* Member status management */
LMS_Result member_repo_suspend_member(MemberRepository *repo, const char *member_id);
LMS_Result member_repo_activate_member(MemberRepository *repo, const char *member_id);

/* Utility functions */
DoublyLinkedList* member_repo_get_all(MemberRepository *repo);
DoublyLinkedList* member_repo_get_active(MemberRepository *repo);
DoublyLinkedList* member_repo_get_suspended(MemberRepository *repo);
LMS_Result member_repo_update_loan_count(MemberRepository *repo, const char *member_id, int change);
int member_repo_get_total_count(MemberRepository *repo);
int member_repo_get_active_count(MemberRepository *repo);

#endif /* MEMBER_REPOSITORY_H */