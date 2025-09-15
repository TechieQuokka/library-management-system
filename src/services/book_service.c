#include "../../include/services/book_service.h"

/* Create a new book service */
BookService* book_service_create(BookRepository *book_repo, LoanRepository *loan_repo) {
    if (!book_repo || !loan_repo) return NULL;

    BookService *service = malloc(sizeof(BookService));
    if (!service) return NULL;

    service->book_repo = book_repo;
    service->loan_repo = loan_repo;

    return service;
}

/* Destroy the book service */
void book_service_destroy(BookService *service) {
    if (service) {
        free(service);
    }
}

/* Register a new book */
LMS_Result book_service_register_book(BookService *service, const Book *book) {
    CHECK_NULL(service);
    CHECK_NULL(book);

    /* Validate book data */
    if (!book_service_validate_book_data(service, book)) {
        return LMS_ERROR_INVALID_INPUT;
    }

    /* Check for duplicate ISBN */
    if (book_service_is_isbn_duplicate(service, book->isbn)) {
        return LMS_ERROR_DUPLICATE;
    }

    /* Add to repository */
    return book_repo_add(service->book_repo, book);
}

/* Update book information */
LMS_Result book_service_update_book(BookService *service, const char *isbn, const Book *book) {
    CHECK_NULL(service);
    CHECK_NULL(isbn);
    CHECK_NULL(book);

    /* Validate book data */
    if (!book_service_validate_book_data(service, book)) {
        return LMS_ERROR_INVALID_INPUT;
    }

    /* Update in repository */
    return book_repo_update(service->book_repo, isbn, book);
}

/* Remove a book */
LMS_Result book_service_remove_book(BookService *service, const char *isbn) {
    CHECK_NULL(service);
    CHECK_NULL(isbn);

    /* Check if book has active loans */
    DoublyLinkedList *active_loans = loan_repo_find_by_book(service->loan_repo, isbn);
    if (active_loans && !dll_is_empty(active_loans)) {
        dll_destroy(active_loans);
        return LMS_ERROR_BOOK_UNAVAILABLE;
    }
    if (active_loans) {
        dll_destroy(active_loans);
    }

    /* Remove from repository */
    return book_repo_delete(service->book_repo, isbn);
}

/* Search books with criteria */
DoublyLinkedList* book_service_search(BookService *service, const BookSearchCriteria *criteria) {
    if (!service || !criteria) return NULL;

    return book_repo_search(service->book_repo, criteria);
}

/* Find book by ISBN */
Book* book_service_find_by_isbn(BookService *service, const char *isbn) {
    if (!service || !isbn) return NULL;

    return book_repo_find_by_isbn(service->book_repo, isbn);
}

/* Find books by title */
DoublyLinkedList* book_service_find_by_title(BookService *service, const char *title) {
    if (!service || !title) return NULL;

    return book_repo_find_by_title(service->book_repo, title);
}

/* Find books by author */
DoublyLinkedList* book_service_find_by_author(BookService *service, const char *author) {
    if (!service || !author) return NULL;

    return book_repo_find_by_author(service->book_repo, author);
}

/* Find books by category */
DoublyLinkedList* book_service_find_by_category(BookService *service, const char *category) {
    if (!service || !category) return NULL;

    return book_repo_find_by_category(service->book_repo, category);
}

/* Check if book is available for loan */
bool book_service_is_available_for_loan(BookService *service, const char *isbn) {
    if (!service || !isbn) return false;

    Book *book = book_repo_find_by_isbn(service->book_repo, isbn);
    if (!book) return false;

    return (book->available_copies > 0 && book->status == 'A');
}

/* Get available copy count */
int book_service_get_available_count(BookService *service, const char *isbn) {
    if (!service || !isbn) return 0;

    Book *book = book_repo_find_by_isbn(service->book_repo, isbn);
    if (!book) return 0;

    return book->available_copies;
}

/* Reserve a book (decrease available count) */
LMS_Result book_service_reserve_book(BookService *service, const char *isbn) {
    CHECK_NULL(service);
    CHECK_NULL(isbn);

    if (!book_service_is_available_for_loan(service, isbn)) {
        return LMS_ERROR_BOOK_UNAVAILABLE;
    }

    return book_repo_update_availability(service->book_repo, isbn, -1);
}

/* Release a book reservation (increase available count) */
LMS_Result book_service_release_reservation(BookService *service, const char *isbn) {
    CHECK_NULL(service);
    CHECK_NULL(isbn);

    return book_repo_update_availability(service->book_repo, isbn, 1);
}

/* Helper function to count book loans */
static int count_book_loans(BookService *service, const char *isbn) {
    DoublyLinkedList *loans = loan_repo_find_by_book(service->loan_repo, isbn);
    if (!loans) return 0;

    int count = dll_size(loans);
    dll_destroy(loans);
    return count;
}

/* Get popular books based on loan count */
DoublyLinkedList* book_service_get_popular_books(BookService *service, int limit) {
    if (!service || limit <= 0) return NULL;

    DoublyLinkedList *all_books = book_repo_get_all(service->book_repo);
    if (!all_books) return NULL;

    DoublyLinkedList *popular_books = dll_create(sizeof(Book), compare_book_title, print_book);
    if (!popular_books) {
        dll_destroy(all_books);
        return NULL;
    }

    /* Simple implementation: just return first 'limit' books */
    /* In a real implementation, you would sort by loan count */
    Iterator *iter = dll_iterator_create(all_books);
    if (!iter) {
        dll_destroy(all_books);
        dll_destroy(popular_books);
        return NULL;
    }

    int added = 0;
    while (iterator_has_next(iter) && added < limit) {
        Book *book = (Book*)iterator_next(iter);
        dll_insert_rear(popular_books, book);
        added++;
    }

    iterator_destroy(iter);
    dll_destroy(all_books);
    return popular_books;
}

/* Get new arrivals (simplified implementation) */
DoublyLinkedList* book_service_get_new_arrivals(BookService *service, int days) {
    if (!service || days <= 0) return NULL;

    /* For this simplified implementation, just return recent books */
    /* In a real implementation, you would filter by acquisition date */
    return book_service_get_popular_books(service, 10);
}

/* Get book recommendations for a member */
DoublyLinkedList* book_service_get_recommendations(BookService *service, const char *member_id) {
    if (!service || !member_id) return NULL;

    /* Simple recommendation: popular books */
    /* In a real implementation, you would analyze member's reading history */
    return book_service_get_popular_books(service, 5);
}

/* Get all books */
DoublyLinkedList* book_service_get_all_books(BookService *service) {
    if (!service) return NULL;

    return book_repo_get_all(service->book_repo);
}

/* Get available books */
DoublyLinkedList* book_service_get_available_books(BookService *service) {
    if (!service) return NULL;

    return book_repo_get_available(service->book_repo);
}

/* Get total book count */
int book_service_get_total_book_count(BookService *service) {
    if (!service) return 0;

    return book_repo_get_total_count(service->book_repo);
}

/* Get available book count */
int book_service_get_available_book_count(BookService *service) {
    if (!service) return 0;

    return book_repo_get_available_count(service->book_repo);
}

/* Validate book data */
bool book_service_validate_book_data(BookService *service, const Book *book) {
    if (!service || !book) return false;

    return validate_book(book);
}

/* Check if ISBN is duplicate */
bool book_service_is_isbn_duplicate(BookService *service, const char *isbn) {
    if (!service || !isbn) return false;

    Book *existing = book_repo_find_by_isbn(service->book_repo, isbn);
    return (existing != NULL);
}