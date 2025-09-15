#ifndef BOOK_SERVICE_H
#define BOOK_SERVICE_H

#include "../repositories/book_repository.h"
#include "../repositories/loan_repository.h"

/* Book Service structure */
typedef struct BookService {
    BookRepository *book_repo;
    LoanRepository *loan_repo;
} BookService;

/* Service management */
BookService* book_service_create(BookRepository *book_repo, LoanRepository *loan_repo);
void book_service_destroy(BookService *service);

/* Book management operations */
LMS_Result book_service_register_book(BookService *service, const Book *book);
LMS_Result book_service_update_book(BookService *service, const char *isbn, const Book *book);
LMS_Result book_service_remove_book(BookService *service, const char *isbn);

/* Search and query operations */
DoublyLinkedList* book_service_search(BookService *service, const BookSearchCriteria *criteria);
Book* book_service_find_by_isbn(BookService *service, const char *isbn);
DoublyLinkedList* book_service_find_by_title(BookService *service, const char *title);
DoublyLinkedList* book_service_find_by_author(BookService *service, const char *author);
DoublyLinkedList* book_service_find_by_category(BookService *service, const char *category);

/* Availability and inventory management */
bool book_service_is_available_for_loan(BookService *service, const char *isbn);
int book_service_get_available_count(BookService *service, const char *isbn);
LMS_Result book_service_reserve_book(BookService *service, const char *isbn);
LMS_Result book_service_release_reservation(BookService *service, const char *isbn);

/* Popular and recommended books */
DoublyLinkedList* book_service_get_popular_books(BookService *service, int limit);
DoublyLinkedList* book_service_get_new_arrivals(BookService *service, int days);
DoublyLinkedList* book_service_get_recommendations(BookService *service, const char *member_id);

/* Collection management */
DoublyLinkedList* book_service_get_all_books(BookService *service);
DoublyLinkedList* book_service_get_available_books(BookService *service);
int book_service_get_total_book_count(BookService *service);
int book_service_get_available_book_count(BookService *service);

/* Data validation */
bool book_service_validate_book_data(BookService *service, const Book *book);
bool book_service_is_isbn_duplicate(BookService *service, const char *isbn);

#endif /* BOOK_SERVICE_H */