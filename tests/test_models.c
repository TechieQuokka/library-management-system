#include "test_framework.h"
#include "../include/models/models.h"

/* Test book validation */
TestResult test_book_validation(void) {
    Book book;
    book_init(&book);

    /* Test valid book */
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

    bool result = validate_book(&book);
    TEST_ASSERT(result, "Valid book should pass validation");

    /* Test invalid ISBN */
    strcpy(book.isbn, "123456789012"); /* Invalid checksum */
    result = validate_book(&book);
    TEST_ASSERT(!result, "Invalid ISBN should fail validation");

    /* Reset to valid ISBN */
    strcpy(book.isbn, "9780132350884");

    /* Test empty title */
    strcpy(book.title, "");
    result = validate_book(&book);
    TEST_ASSERT(!result, "Empty title should fail validation");

    /* Test negative price */
    strcpy(book.title, "Valid Title");
    book.price = -10.0;
    result = validate_book(&book);
    TEST_ASSERT(!result, "Negative price should fail validation");

    /* Test invalid year */
    book.price = 49.99;
    book.publication_year = 2050;
    result = validate_book(&book);
    TEST_ASSERT(!result, "Future year should fail validation");

    TEST_SUCCESS();
}

/* Test member validation */
TestResult test_member_validation(void) {
    Member member;
    member_init(&member);

    /* Test valid member */
    strcpy(member.member_id, "M001");
    strcpy(member.name, "John Doe");
    strcpy(member.phone, "555-0123");
    strcpy(member.email, "john@example.com");
    strcpy(member.address, "123 Main St");
    strcpy(member.join_date, "2024-01-01");
    member.membership_type = 'R';
    member.loan_count = 0;
    member.status = 'A';

    bool result = validate_member(&member);
    TEST_ASSERT(result, "Valid member should pass validation");

    /* Test empty member ID */
    strcpy(member.member_id, "");
    result = validate_member(&member);
    TEST_ASSERT(!result, "Empty member ID should fail validation");

    /* Reset member ID */
    strcpy(member.member_id, "M001");

    /* Test invalid email */
    strcpy(member.email, "invalid-email");
    result = validate_member(&member);
    TEST_ASSERT(!result, "Invalid email should fail validation");

    /* Test invalid membership type */
    strcpy(member.email, "john@example.com");
    member.membership_type = 'X';
    result = validate_member(&member);
    TEST_ASSERT(!result, "Invalid membership type should fail validation");

    /* Test negative loan count */
    member.membership_type = 'R';
    member.loan_count = -1;
    result = validate_member(&member);
    TEST_ASSERT(!result, "Negative loan count should fail validation");

    TEST_SUCCESS();
}

/* Test loan validation */
TestResult test_loan_validation(void) {
    Loan loan;
    loan_init(&loan);

    /* Test valid loan */
    strcpy(loan.loan_id, "L001");
    strcpy(loan.member_id, "M001");
    strcpy(loan.isbn, "9780132350884");
    strcpy(loan.loan_date, "2024-01-01");
    strcpy(loan.due_date, "2024-01-15");
    strcpy(loan.return_date, "");
    loan.overdue_days = 0;
    loan.fine_amount = 0.0;
    loan.status = 'L';

    bool result = validate_loan(&loan);
    TEST_ASSERT(result, "Valid loan should pass validation");

    /* Test empty loan ID */
    strcpy(loan.loan_id, "");
    result = validate_loan(&loan);
    TEST_ASSERT(!result, "Empty loan ID should fail validation");

    /* Reset loan ID */
    strcpy(loan.loan_id, "L001");

    /* Test invalid ISBN */
    strcpy(loan.isbn, "invalid-isbn");
    result = validate_loan(&loan);
    TEST_ASSERT(!result, "Invalid ISBN should fail validation");

    /* Test invalid date format */
    strcpy(loan.isbn, "9780132350884");
    strcpy(loan.loan_date, "2024-13-01"); /* Invalid month */
    result = validate_loan(&loan);
    TEST_ASSERT(!result, "Invalid date should fail validation");

    /* Test negative fine */
    strcpy(loan.loan_date, "2024-01-01");
    loan.fine_amount = -5.0;
    result = validate_loan(&loan);
    TEST_ASSERT(!result, "Negative fine should fail validation");

    /* Test invalid status */
    loan.fine_amount = 0.0;
    loan.status = 'X';
    result = validate_loan(&loan);
    TEST_ASSERT(!result, "Invalid status should fail validation");

    TEST_SUCCESS();
}