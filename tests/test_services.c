#include "test_framework.h"
#include "../include/services/book_service.h"
#include "../include/services/member_service.h"
#include "../include/services/loan_service.h"

/* Test book service operations */
TestResult test_book_service_operations(void) {
    /* Create repositories */
    BookRepository *book_repo = book_repository_create();
    LoanRepository *loan_repo = loan_repository_create();
    TEST_ASSERT_NOT_NULL(book_repo);
    TEST_ASSERT_NOT_NULL(loan_repo);

    /* Create service */
    BookService *service = book_service_create(book_repo, loan_repo);
    TEST_ASSERT_NOT_NULL(service);

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

    /* Test register book */
    LMS_Result result = book_service_register_book(service, &book);
    TEST_ASSERT_EQUAL_INT(LMS_SUCCESS, result);

    /* Test duplicate registration */
    result = book_service_register_book(service, &book);
    TEST_ASSERT_EQUAL_INT(LMS_ERROR_DUPLICATE, result);

    /* Test find by ISBN */
    Book *found = book_service_find_by_isbn(service, "9780132350884");
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL_STRING("Clean Code", found->title);

    /* Test availability check */
    bool available = book_service_is_available_for_loan(service, "9780132350884");
    TEST_ASSERT(available, "Book should be available for loan");

    /* Test get available count */
    int count = book_service_get_available_count(service, "9780132350884");
    TEST_ASSERT_EQUAL_INT(5, count);

    /* Test reserve book */
    result = book_service_reserve_book(service, "9780132350884");
    TEST_ASSERT_EQUAL_INT(LMS_SUCCESS, result);

    count = book_service_get_available_count(service, "9780132350884");
    TEST_ASSERT_EQUAL_INT(4, count);

    /* Test release reservation */
    result = book_service_release_reservation(service, "9780132350884");
    TEST_ASSERT_EQUAL_INT(LMS_SUCCESS, result);

    count = book_service_get_available_count(service, "9780132350884");
    TEST_ASSERT_EQUAL_INT(5, count);

    /* Test get total count */
    count = book_service_get_total_book_count(service);
    TEST_ASSERT_EQUAL_INT(1, count);

    /* Cleanup */
    book_service_destroy(service);
    book_repository_destroy(book_repo);
    loan_repository_destroy(loan_repo);

    TEST_SUCCESS();
}

/* Test member service operations */
TestResult test_member_service_operations(void) {
    /* Create repositories */
    MemberRepository *member_repo = member_repository_create();
    LoanRepository *loan_repo = loan_repository_create();
    TEST_ASSERT_NOT_NULL(member_repo);
    TEST_ASSERT_NOT_NULL(loan_repo);

    /* Create service */
    MemberService *service = member_service_create(member_repo, loan_repo);
    TEST_ASSERT_NOT_NULL(service);

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

    /* Test register member */
    LMS_Result result = member_service_register_member(service, &member);
    TEST_ASSERT_EQUAL_INT(LMS_SUCCESS, result);

    /* Test duplicate registration */
    result = member_service_register_member(service, &member);
    TEST_ASSERT_EQUAL_INT(LMS_ERROR_DUPLICATE, result);

    /* Test find by ID */
    Member *found = member_service_find_by_id(service, "M001");
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL_STRING("John Doe", found->name);

    /* Test can borrow book */
    bool can_borrow = member_service_can_borrow_book(service, "M001");
    TEST_ASSERT(can_borrow, "Active member should be able to borrow");

    /* Test loan limit */
    int limit = member_service_get_max_loan_limit(service, "M001");
    TEST_ASSERT_EQUAL_INT(3, limit); /* Regular member limit */

    int remaining = member_service_get_remaining_loan_limit(service, "M001");
    TEST_ASSERT_EQUAL_INT(3, remaining);

    /* Test suspend member */
    result = member_service_suspend_member(service, "M001", "Test suspension");
    TEST_ASSERT_EQUAL_INT(LMS_SUCCESS, result);

    can_borrow = member_service_can_borrow_book(service, "M001");
    TEST_ASSERT(!can_borrow, "Suspended member should not be able to borrow");

    /* Test reactivate member */
    result = member_service_reactivate_member(service, "M001");
    TEST_ASSERT_EQUAL_INT(LMS_SUCCESS, result);

    can_borrow = member_service_can_borrow_book(service, "M001");
    TEST_ASSERT(can_borrow, "Reactivated member should be able to borrow");

    /* Test get total count */
    int count = member_service_get_total_member_count(service);
    TEST_ASSERT_EQUAL_INT(1, count);

    /* Cleanup */
    member_service_destroy(service);
    member_repository_destroy(member_repo);
    loan_repository_destroy(loan_repo);

    TEST_SUCCESS();
}

/* Test loan service operations */
TestResult test_loan_service_operations(void) {
    /* Create repositories */
    BookRepository *book_repo = book_repository_create();
    MemberRepository *member_repo = member_repository_create();
    LoanRepository *loan_repo = loan_repository_create();
    TEST_ASSERT_NOT_NULL(book_repo);
    TEST_ASSERT_NOT_NULL(member_repo);
    TEST_ASSERT_NOT_NULL(loan_repo);

    /* Create service */
    LoanService *service = loan_service_create(loan_repo, book_repo, member_repo);
    TEST_ASSERT_NOT_NULL(service);

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

    book_repo_add(book_repo, &book);

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

    member_repo_add(member_repo, &member);

    /* Test can borrow */
    bool can_borrow = loan_service_can_borrow(service, "M001", "9780132350884");
    TEST_ASSERT(can_borrow, "Member should be able to borrow available book");

    /* Test borrow book */
    LMS_Result result = loan_service_borrow_book(service, "M001", "9780132350884");
    TEST_ASSERT_EQUAL_INT(LMS_SUCCESS, result);

    /* Verify book availability decreased */
    Book *found_book = book_repo_find_by_isbn(book_repo, "9780132350884");
    TEST_ASSERT_NOT_NULL(found_book);
    TEST_ASSERT_EQUAL_INT(4, found_book->available_copies);

    /* Verify member loan count increased */
    Member *found_member = member_repo_find_by_id(member_repo, "M001");
    TEST_ASSERT_NOT_NULL(found_member);
    TEST_ASSERT_EQUAL_INT(1, found_member->loan_count);

    /* Test get active loans */
    DoublyLinkedList *active_loans = loan_service_get_active_loans(service);
    TEST_ASSERT_NOT_NULL(active_loans);
    TEST_ASSERT_EQUAL_INT(1, dll_size(active_loans));

    /* Get the loan ID for return test */
    Iterator *iter = dll_iterator_create(active_loans);
    TEST_ASSERT_NOT_NULL(iter);
    Loan *loan = (Loan*)iterator_next(iter);
    TEST_ASSERT_NOT_NULL(loan);
    char loan_id[11];
    strcpy(loan_id, loan->loan_id);
    iterator_destroy(iter);
    dll_destroy(active_loans);

    /* Test return book */
    result = loan_service_return_book(service, loan_id);
    TEST_ASSERT_EQUAL_INT(LMS_SUCCESS, result);

    /* Verify book availability increased */
    found_book = book_repo_find_by_isbn(book_repo, "9780132350884");
    TEST_ASSERT_NOT_NULL(found_book);
    TEST_ASSERT_EQUAL_INT(5, found_book->available_copies);

    /* Verify member loan count decreased */
    found_member = member_repo_find_by_id(member_repo, "M001");
    TEST_ASSERT_NOT_NULL(found_member);
    TEST_ASSERT_EQUAL_INT(0, found_member->loan_count);

    /* Test counts */
    int count = loan_service_get_total_loan_count(service);
    TEST_ASSERT_EQUAL_INT(1, count);

    count = loan_service_get_active_loan_count(service);
    TEST_ASSERT_EQUAL_INT(0, count);

    /* Cleanup */
    loan_service_destroy(service);
    book_repository_destroy(book_repo);
    member_repository_destroy(member_repo);
    loan_repository_destroy(loan_repo);

    TEST_SUCCESS();
}