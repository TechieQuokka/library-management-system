#include "test_framework.h"
#include "../include/repositories/book_repository.h"
#include "../include/repositories/member_repository.h"
#include "../include/repositories/loan_repository.h"

/* Test book repository CRUD operations */
TestResult test_book_repository_crud(void) {
    BookRepository *repo = book_repository_create();
    TEST_ASSERT_NOT_NULL(repo);

    /* Create test book */
    Book book;
    book_init(&book);
    strcpy(book.isbn, "9780132350884");
    strcpy(book.title, "Clean Code");
    strcpy(book.author, "Robert C. Martin");
    strcpy(book.publisher, "Prentice Hall");
    book.publication_year = 2008;
    strcpy(book.category, "Programming");
    book.total_copies = 5;
    book.available_copies = 5;
    book.price = 49.99;
    book.status = 'A';

    /* Test add book */
    LMS_Result result = book_repo_add(repo, &book);
    TEST_ASSERT_EQUAL_INT(LMS_SUCCESS, result);
    TEST_ASSERT_EQUAL_INT(1, book_repo_get_total_count(repo));

    /* Test find by ISBN */
    Book *found = book_repo_find_by_isbn(repo, "9780132350884");
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL_STRING("Clean Code", found->title);

    /* Test duplicate add */
    result = book_repo_add(repo, &book);
    TEST_ASSERT_EQUAL_INT(LMS_ERROR_DUPLICATE, result);

    /* Test update book */
    strcpy(book.title, "Clean Code - Updated");
    result = book_repo_update(repo, "9780132350884", &book);
    TEST_ASSERT_EQUAL_INT(LMS_SUCCESS, result);

    found = book_repo_find_by_isbn(repo, "9780132350884");
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL_STRING("Clean Code - Updated", found->title);

    /* Test delete book */
    result = book_repo_delete(repo, "9780132350884");
    TEST_ASSERT_EQUAL_INT(LMS_SUCCESS, result);
    TEST_ASSERT_EQUAL_INT(0, book_repo_get_total_count(repo));

    /* Test find after delete */
    found = book_repo_find_by_isbn(repo, "9780132350884");
    TEST_ASSERT_NULL(found);

    book_repository_destroy(repo);
    TEST_SUCCESS();
}

/* Test member repository CRUD operations */
TestResult test_member_repository_crud(void) {
    MemberRepository *repo = member_repository_create();
    TEST_ASSERT_NOT_NULL(repo);

    /* Create test member */
    Member member;
    member_init(&member);
    strcpy(member.member_id, "M001");
    strcpy(member.name, "John Doe");
    strcpy(member.phone, "555-0123");
    strcpy(member.email, "john@example.com");
    strcpy(member.address, "123 Main St");
    strcpy(member.join_date, "2024-01-01");
    member.membership_type = 'R';
    member.loan_count = 0;
    member.status = 'A';

    /* Test add member */
    LMS_Result result = member_repo_add(repo, &member);
    TEST_ASSERT_EQUAL_INT(LMS_SUCCESS, result);
    TEST_ASSERT_EQUAL_INT(1, member_repo_get_total_count(repo));

    /* Test find by ID */
    Member *found = member_repo_find_by_id(repo, "M001");
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL_STRING("John Doe", found->name);

    /* Test find by email */
    found = member_repo_find_by_email(repo, "john@example.com");
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL_STRING("M001", found->member_id);

    /* Test duplicate add */
    result = member_repo_add(repo, &member);
    TEST_ASSERT_EQUAL_INT(LMS_ERROR_DUPLICATE, result);

    /* Test update member */
    strcpy(member.name, "John Doe - Updated");
    result = member_repo_update(repo, "M001", &member);
    TEST_ASSERT_EQUAL_INT(LMS_SUCCESS, result);

    found = member_repo_find_by_id(repo, "M001");
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL_STRING("John Doe - Updated", found->name);

    /* Test suspend member */
    result = member_repo_suspend_member(repo, "M001");
    TEST_ASSERT_EQUAL_INT(LMS_SUCCESS, result);

    found = member_repo_find_by_id(repo, "M001");
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL_INT('S', found->status);

    /* Test activate member */
    result = member_repo_activate_member(repo, "M001");
    TEST_ASSERT_EQUAL_INT(LMS_SUCCESS, result);

    found = member_repo_find_by_id(repo, "M001");
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL_INT('A', found->status);

    /* Test delete member */
    result = member_repo_delete(repo, "M001");
    TEST_ASSERT_EQUAL_INT(LMS_SUCCESS, result);
    TEST_ASSERT_EQUAL_INT(0, member_repo_get_total_count(repo));

    member_repository_destroy(repo);
    TEST_SUCCESS();
}

/* Test loan repository CRUD operations */
TestResult test_loan_repository_crud(void) {
    LoanRepository *repo = loan_repository_create();
    TEST_ASSERT_NOT_NULL(repo);

    /* Create test loan */
    Loan loan;
    loan_init(&loan);
    strcpy(loan.loan_id, "L001");
    strcpy(loan.member_id, "M001");
    strcpy(loan.isbn, "9780132350884");
    strcpy(loan.loan_date, "2024-01-01");
    strcpy(loan.due_date, "2024-01-15");
    strcpy(loan.return_date, "");
    loan.overdue_days = 0;
    loan.fine_amount = 0.0;
    loan.status = 'L';

    /* Test add loan */
    LMS_Result result = loan_repo_add(repo, &loan);
    TEST_ASSERT_EQUAL_INT(LMS_SUCCESS, result);
    TEST_ASSERT_EQUAL_INT(1, loan_repo_get_total_count(repo));

    /* Test find by ID */
    Loan *found = loan_repo_find_by_id(repo, "L001");
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL_STRING("M001", found->member_id);

    /* Test find by member */
    DoublyLinkedList *loans = loan_repo_find_by_member(repo, "M001");
    TEST_ASSERT_NOT_NULL(loans);
    TEST_ASSERT_EQUAL_INT(1, dll_size(loans));
    dll_destroy(loans);

    /* Test find by book */
    loans = loan_repo_find_by_book(repo, "9780132350884");
    TEST_ASSERT_NOT_NULL(loans);
    TEST_ASSERT_EQUAL_INT(1, dll_size(loans));
    dll_destroy(loans);

    /* Test duplicate add */
    result = loan_repo_add(repo, &loan);
    TEST_ASSERT_EQUAL_INT(LMS_ERROR_DUPLICATE, result);

    /* Test mark as returned */
    result = loan_repo_mark_returned(repo, "L001", "2024-01-10");
    TEST_ASSERT_EQUAL_INT(LMS_SUCCESS, result);

    found = loan_repo_find_by_id(repo, "L001");
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL_STRING("2024-01-10", found->return_date);
    TEST_ASSERT_EQUAL_INT('R', found->status);

    /* Test mark as overdue */
    strcpy(found->return_date, "");
    found->status = 'L';
    result = loan_repo_mark_overdue(repo, "L001", 5, 5.0);
    TEST_ASSERT_EQUAL_INT(LMS_SUCCESS, result);

    found = loan_repo_find_by_id(repo, "L001");
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL_INT(5, found->overdue_days);
    TEST_ASSERT_EQUAL_INT('O', found->status);

    /* Test delete loan */
    result = loan_repo_delete(repo, "L001");
    TEST_ASSERT_EQUAL_INT(LMS_SUCCESS, result);
    TEST_ASSERT_EQUAL_INT(0, loan_repo_get_total_count(repo));

    loan_repository_destroy(repo);
    TEST_SUCCESS();
}