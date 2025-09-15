#ifndef BOOK_REPOSITORY_H
#define BOOK_REPOSITORY_H

#include "../models/models.h"
#include "../core/doubly_linked_list.h"

/* Book Repository structure */
typedef struct BookRepository {
    DoublyLinkedList *books;        /* Main book list */
    DoublyLinkedList *isbn_index;   /* ISBN index */
    DoublyLinkedList *title_index;  /* Title index */
    DoublyLinkedList *author_index; /* Author index */
} BookRepository;

/* Repository management */
BookRepository* book_repository_create(void);
void book_repository_destroy(BookRepository *repo);

/* CRUD operations */
LMS_Result book_repo_add(BookRepository *repo, const Book *book);
Book* book_repo_find_by_isbn(BookRepository *repo, const char *isbn);
DoublyLinkedList* book_repo_find_by_title(BookRepository *repo, const char *title);
DoublyLinkedList* book_repo_find_by_author(BookRepository *repo, const char *author);
DoublyLinkedList* book_repo_find_by_category(BookRepository *repo, const char *category);
LMS_Result book_repo_update(BookRepository *repo, const char *isbn, const Book *updated_book);
LMS_Result book_repo_delete(BookRepository *repo, const char *isbn);

/* Advanced search */
DoublyLinkedList* book_repo_search(BookRepository *repo, const BookSearchCriteria *criteria);

/* Utility functions */
DoublyLinkedList* book_repo_get_all(BookRepository *repo);
DoublyLinkedList* book_repo_get_available(BookRepository *repo);
LMS_Result book_repo_update_availability(BookRepository *repo, const char *isbn, int change);
int book_repo_get_total_count(BookRepository *repo);
int book_repo_get_available_count(BookRepository *repo);

#endif /* BOOK_REPOSITORY_H */