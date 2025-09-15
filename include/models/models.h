#ifndef MODELS_H
#define MODELS_H

#include "../common.h"

/* Book structure */
typedef struct {
    char isbn[14];          /* ISBN-13 + null terminator */
    char title[101];        /* Title (100 chars + null) */
    char author[51];        /* Author (50 chars + null) */
    char publisher[51];     /* Publisher (50 chars + null) */
    int publication_year;   /* Publication year */
    char category[31];      /* Category (30 chars + null) */
    int total_copies;       /* Total number of copies */
    int available_copies;   /* Available copies for loan */
    double price;           /* Price */
    char status;            /* Status: 'A' = Active, 'D' = Deleted */
} Book;

/* Member structure */
typedef struct {
    char member_id[11];     /* Member ID (10 chars + null) */
    char name[51];          /* Name (50 chars + null) */
    char phone[16];         /* Phone number (15 chars + null) */
    char email[101];        /* Email (100 chars + null) */
    char address[201];      /* Address (200 chars + null) */
    char join_date[11];     /* Join date (YYYY-MM-DD) */
    char membership_type;   /* Type: 'R' = Regular, 'P' = Premium */
    int loan_count;         /* Current number of loans */
    char status;            /* Status: 'A' = Active, 'S' = Suspended, 'D' = Deleted */
} Member;

/* Loan structure */
typedef struct {
    char loan_id[11];       /* Loan ID (10 chars + null) */
    char member_id[11];     /* Member ID */
    char isbn[14];          /* Book ISBN */
    char loan_date[11];     /* Loan date (YYYY-MM-DD) */
    char due_date[11];      /* Due date (YYYY-MM-DD) */
    char return_date[11];   /* Return date (YYYY-MM-DD, empty if not returned) */
    int overdue_days;       /* Number of overdue days */
    double fine_amount;     /* Fine amount */
    char status;            /* Status: 'L' = Loaned, 'R' = Returned, 'O' = Overdue */
} Loan;

/* Search criteria structures */
typedef struct {
    bool search_by_title;
    char title[101];
    bool search_by_author;
    char author[51];
    bool search_by_category;
    char category[31];
    bool search_by_isbn;
    char isbn[14];
    bool only_available;
} BookSearchCriteria;

typedef struct {
    bool search_by_name;
    char name[51];
    bool search_by_email;
    char email[101];
    bool search_by_phone;
    char phone[16];
    bool only_active;
} MemberSearchCriteria;

/* Compare functions */
int compare_book_isbn(const void *a, const void *b);
int compare_book_title(const void *a, const void *b);
int compare_book_author(const void *a, const void *b);
int compare_member_id(const void *a, const void *b);
int compare_member_name(const void *a, const void *b);
int compare_loan_id(const void *a, const void *b);

/* Print functions */
void print_book(const void *book);
void print_member(const void *member);
void print_loan(const void *loan);

/* Validation functions */
bool validate_book(const Book *book);
bool validate_member(const Member *member);
bool validate_loan(const Loan *loan);
bool validate_isbn(const char *isbn);
bool validate_email(const char *email);
bool validate_phone(const char *phone);
bool validate_date(const char *date);

/* Utility functions */
void book_init(Book *book);
void member_init(Member *member);
void loan_init(Loan *loan);

#endif /* MODELS_H */