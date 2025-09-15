#include "../../include/models/models.h"

/* Compare members by ID */
int compare_member_id(const void *a, const void *b) {
    const Member *member_a = (const Member *)a;
    const Member *member_b = (const Member *)b;
    return strcmp(member_a->member_id, member_b->member_id);
}

/* Compare members by name */
int compare_member_name(const void *a, const void *b) {
    const Member *member_a = (const Member *)a;
    const Member *member_b = (const Member *)b;
    return strcmp(member_a->name, member_b->name);
}

/* Print member information */
void print_member(const void *data) {
    const Member *member = (const Member *)data;
    printf("ID: %s | Name: %s | Email: %s | Loans: %d | Status: %c\n",
           member->member_id, member->name, member->email,
           member->loan_count, member->status);
}

/* Initialize member structure */
void member_init(Member *member) {
    if (!member) return;

    memset(member->member_id, 0, sizeof(member->member_id));
    memset(member->name, 0, sizeof(member->name));
    memset(member->phone, 0, sizeof(member->phone));
    memset(member->email, 0, sizeof(member->email));
    memset(member->address, 0, sizeof(member->address));
    memset(member->join_date, 0, sizeof(member->join_date));
    member->membership_type = 'R';
    member->loan_count = 0;
    member->status = 'A';
}

/* Validate member data */
bool validate_member(const Member *member) {
    if (!member) return false;

    /* Validate member ID */
    if (strlen(member->member_id) == 0 || strlen(member->member_id) > 10) return false;

    /* Validate name */
    if (strlen(member->name) == 0 || strlen(member->name) > 50) return false;

    /* Validate phone */
    if (strlen(member->phone) > 0 && !validate_phone(member->phone)) return false;

    /* Validate email */
    if (strlen(member->email) > 0 && !validate_email(member->email)) return false;

    /* Validate address */
    if (strlen(member->address) > 200) return false;

    /* Validate join date */
    if (strlen(member->join_date) > 0 && !validate_date(member->join_date)) return false;

    /* Validate membership type */
    if (member->membership_type != 'R' && member->membership_type != 'P') return false;

    /* Validate loan count */
    if (member->loan_count < 0) return false;

    /* Validate status */
    if (member->status != 'A' && member->status != 'S' && member->status != 'D') return false;

    return true;
}

/* Validate email format */
bool validate_email(const char *email) {
    if (!email) return false;

    int len = strlen(email);
    if (len == 0 || len > 100) return false;

    /* Simple email validation: must contain @ and . */
    char *at_pos = strchr(email, '@');
    if (!at_pos) return false;

    char *dot_pos = strrchr(email, '.');
    if (!dot_pos) return false;

    /* @ must come before . */
    if (at_pos >= dot_pos) return false;

    /* Must have characters before @, between @ and ., and after . */
    if (at_pos == email || dot_pos == at_pos + 1 || dot_pos == email + len - 1) return false;

    return true;
}

/* Validate phone number format */
bool validate_phone(const char *phone) {
    if (!phone) return false;

    int len = strlen(phone);
    if (len == 0 || len > 15) return false;

    /* Phone can contain digits, spaces, hyphens, parentheses, and plus sign */
    for (int i = 0; i < len; i++) {
        char c = phone[i];
        if (!(c >= '0' && c <= '9') && c != ' ' && c != '-' &&
            c != '(' && c != ')' && c != '+') {
            return false;
        }
    }

    return true;
}