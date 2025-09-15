#include "../../include/repositories/book_repository.h"

/* Helper function to update indexes */
static void update_book_indexes(BookRepository *repo, const Book *book) {
    if (!repo || !book) return;

    /* For now, we'll keep it simple and not maintain separate indexes */
    /* In a full implementation, you would maintain sorted indexes for faster searches */
}

/* Create a new book repository */
BookRepository* book_repository_create(void) {
    BookRepository *repo = malloc(sizeof(BookRepository));
    if (!repo) return NULL;

    repo->books = dll_create(sizeof(Book), compare_book_isbn, print_book);
    if (!repo->books) {
        free(repo);
        return NULL;
    }

    /* Initialize indexes (simplified - using same compare functions) */
    repo->isbn_index = dll_create(sizeof(Book*), NULL, NULL);
    repo->title_index = dll_create(sizeof(Book*), NULL, NULL);
    repo->author_index = dll_create(sizeof(Book*), NULL, NULL);

    if (!repo->isbn_index || !repo->title_index || !repo->author_index) {
        book_repository_destroy(repo);
        return NULL;
    }

    return repo;
}

/* Destroy the book repository */
void book_repository_destroy(BookRepository *repo) {
    if (!repo) return;

    dll_destroy(repo->books);
    dll_destroy(repo->isbn_index);
    dll_destroy(repo->title_index);
    dll_destroy(repo->author_index);
    free(repo);
}

/* Add a book to the repository */
LMS_Result book_repo_add(BookRepository *repo, const Book *book) {
    CHECK_NULL(repo);
    CHECK_NULL(book);

    if (!validate_book(book)) {
        return LMS_ERROR_INVALID_INPUT;
    }

    /* Check for duplicate ISBN */
    if (book_repo_find_by_isbn(repo, book->isbn)) {
        return LMS_ERROR_DUPLICATE;
    }

    /* Add to main list (sorted by ISBN) */
    LMS_Result result = dll_insert_sorted(repo->books, book);
    if (result != LMS_SUCCESS) {
        return result;
    }

    /* Update indexes */
    update_book_indexes(repo, book);

    return LMS_SUCCESS;
}

/* Find book by ISBN */
Book* book_repo_find_by_isbn(BookRepository *repo, const char *isbn) {
    if (!repo || !isbn) return NULL;

    Book search_book;
    book_init(&search_book);
    strncpy(search_book.isbn, isbn, sizeof(search_book.isbn) - 1);

    Node *found = dll_search(repo->books, &search_book);
    return found ? (Book*)found->data : NULL;
}

/* Helper function for title search */
static bool book_title_contains(const void *data, void *context) {
    const Book *book = (const Book *)data;
    const char *search_title = (const char *)context;

    /* Convert both to lowercase for case-insensitive search */
    char book_title_lower[101];
    char search_lower[101];

    strncpy(book_title_lower, book->title, sizeof(book_title_lower) - 1);
    strncpy(search_lower, search_title, sizeof(search_lower) - 1);

    /* Simple lowercase conversion */
    for (int i = 0; book_title_lower[i]; i++) {
        if (book_title_lower[i] >= 'A' && book_title_lower[i] <= 'Z') {
            book_title_lower[i] += 32;
        }
    }
    for (int i = 0; search_lower[i]; i++) {
        if (search_lower[i] >= 'A' && search_lower[i] <= 'Z') {
            search_lower[i] += 32;
        }
    }

    return strstr(book_title_lower, search_lower) != NULL;
}

/* Find books by title (partial match) */
DoublyLinkedList* book_repo_find_by_title(BookRepository *repo, const char *title) {
    if (!repo || !title) return NULL;

    DoublyLinkedList *results = dll_create(sizeof(Book), compare_book_title, print_book);
    if (!results) return NULL;

    Iterator *iter = dll_iterator_create(repo->books);
    if (!iter) {
        dll_destroy(results);
        return NULL;
    }

    while (iterator_has_next(iter)) {
        Book *book = (Book*)iterator_next(iter);
        if (book_title_contains(book, (void*)title)) {
            dll_insert_rear(results, book);
        }
    }

    iterator_destroy(iter);
    return results;
}

/* Helper function for author search */
static bool book_author_contains(const void *data, void *context) {
    const Book *book = (const Book *)data;
    const char *search_author = (const char *)context;

    /* Convert both to lowercase for case-insensitive search */
    char book_author_lower[51];
    char search_lower[51];

    strncpy(book_author_lower, book->author, sizeof(book_author_lower) - 1);
    strncpy(search_lower, search_author, sizeof(search_lower) - 1);

    /* Simple lowercase conversion */
    for (int i = 0; book_author_lower[i]; i++) {
        if (book_author_lower[i] >= 'A' && book_author_lower[i] <= 'Z') {
            book_author_lower[i] += 32;
        }
    }
    for (int i = 0; search_lower[i]; i++) {
        if (search_lower[i] >= 'A' && search_lower[i] <= 'Z') {
            search_lower[i] += 32;
        }
    }

    return strstr(book_author_lower, search_lower) != NULL;
}

/* Find books by author (partial match) */
DoublyLinkedList* book_repo_find_by_author(BookRepository *repo, const char *author) {
    if (!repo || !author) return NULL;

    DoublyLinkedList *results = dll_create(sizeof(Book), compare_book_author, print_book);
    if (!results) return NULL;

    Iterator *iter = dll_iterator_create(repo->books);
    if (!iter) {
        dll_destroy(results);
        return NULL;
    }

    while (iterator_has_next(iter)) {
        Book *book = (Book*)iterator_next(iter);
        if (book_author_contains(book, (void*)author)) {
            dll_insert_rear(results, book);
        }
    }

    iterator_destroy(iter);
    return results;
}

/* Helper function for category search */
static bool book_category_matches(const void *data, void *context) {
    const Book *book = (const Book *)data;
    const char *search_category = (const char *)context;
    return strcmp(book->category, search_category) == 0;
}

/* Find books by category */
DoublyLinkedList* book_repo_find_by_category(BookRepository *repo, const char *category) {
    if (!repo || !category) return NULL;

    DoublyLinkedList *results = dll_create(sizeof(Book), compare_book_title, print_book);
    if (!results) return NULL;

    Iterator *iter = dll_iterator_create(repo->books);
    if (!iter) {
        dll_destroy(results);
        return NULL;
    }

    while (iterator_has_next(iter)) {
        Book *book = (Book*)iterator_next(iter);
        if (book_category_matches(book, (void*)category)) {
            dll_insert_rear(results, book);
        }
    }

    iterator_destroy(iter);
    return results;
}

/* Update book information */
LMS_Result book_repo_update(BookRepository *repo, const char *isbn, const Book *updated_book) {
    CHECK_NULL(repo);
    CHECK_NULL(isbn);
    CHECK_NULL(updated_book);

    if (!validate_book(updated_book)) {
        return LMS_ERROR_INVALID_INPUT;
    }

    /* Find the existing book */
    Book *existing_book = book_repo_find_by_isbn(repo, isbn);
    if (!existing_book) {
        return LMS_ERROR_NOT_FOUND;
    }

    /* Update the book data */
    memcpy(existing_book, updated_book, sizeof(Book));

    return LMS_SUCCESS;
}

/* Delete a book */
LMS_Result book_repo_delete(BookRepository *repo, const char *isbn) {
    CHECK_NULL(repo);
    CHECK_NULL(isbn);

    Book search_book;
    book_init(&search_book);
    strncpy(search_book.isbn, isbn, sizeof(search_book.isbn) - 1);

    LMS_Result result = dll_delete_data(repo->books, &search_book);
    return result;
}

/* Advanced search with multiple criteria */
DoublyLinkedList* book_repo_search(BookRepository *repo, const BookSearchCriteria *criteria) {
    if (!repo || !criteria) return NULL;

    DoublyLinkedList *results = dll_create(sizeof(Book), compare_book_title, print_book);
    if (!results) return NULL;

    Iterator *iter = dll_iterator_create(repo->books);
    if (!iter) {
        dll_destroy(results);
        return NULL;
    }

    while (iterator_has_next(iter)) {
        Book *book = (Book*)iterator_next(iter);
        bool matches = true;

        /* Check ISBN criteria */
        if (criteria->search_by_isbn) {
            if (strcmp(book->isbn, criteria->isbn) != 0) {
                matches = false;
            }
        }

        /* Check title criteria */
        if (matches && criteria->search_by_title) {
            if (!book_title_contains(book, (void*)criteria->title)) {
                matches = false;
            }
        }

        /* Check author criteria */
        if (matches && criteria->search_by_author) {
            if (!book_author_contains(book, (void*)criteria->author)) {
                matches = false;
            }
        }

        /* Check category criteria */
        if (matches && criteria->search_by_category) {
            if (strcmp(book->category, criteria->category) != 0) {
                matches = false;
            }
        }

        /* Check availability criteria */
        if (matches && criteria->only_available) {
            if (book->available_copies <= 0) {
                matches = false;
            }
        }

        if (matches) {
            dll_insert_rear(results, book);
        }
    }

    iterator_destroy(iter);
    return results;
}

/* Get all books */
DoublyLinkedList* book_repo_get_all(BookRepository *repo) {
    if (!repo) return NULL;
    return dll_clone(repo->books);
}

/* Helper function for available books */
static bool book_is_available(const void *data, void *context) {
    const Book *book = (const Book *)data;
    return book->available_copies > 0 && book->status == 'A';
}

/* Get available books */
DoublyLinkedList* book_repo_get_available(BookRepository *repo) {
    if (!repo) return NULL;

    DoublyLinkedList *results = dll_create(sizeof(Book), compare_book_title, print_book);
    if (!results) return NULL;

    Iterator *iter = dll_iterator_create(repo->books);
    if (!iter) {
        dll_destroy(results);
        return NULL;
    }

    while (iterator_has_next(iter)) {
        Book *book = (Book*)iterator_next(iter);
        if (book_is_available(book, NULL)) {
            dll_insert_rear(results, book);
        }
    }

    iterator_destroy(iter);
    return results;
}

/* Update book availability */
LMS_Result book_repo_update_availability(BookRepository *repo, const char *isbn, int change) {
    CHECK_NULL(repo);
    CHECK_NULL(isbn);

    Book *book = book_repo_find_by_isbn(repo, isbn);
    if (!book) {
        return LMS_ERROR_NOT_FOUND;
    }

    int new_available = book->available_copies + change;
    if (new_available < 0 || new_available > book->total_copies) {
        return LMS_ERROR_INVALID_INPUT;
    }

    book->available_copies = new_available;
    return LMS_SUCCESS;
}

/* Get total book count */
int book_repo_get_total_count(BookRepository *repo) {
    return repo ? dll_size(repo->books) : 0;
}

/* Get available book count */
int book_repo_get_available_count(BookRepository *repo) {
    if (!repo) return 0;

    int count = 0;
    Iterator *iter = dll_iterator_create(repo->books);
    if (!iter) return 0;

    while (iterator_has_next(iter)) {
        Book *book = (Book*)iterator_next(iter);
        if (book_is_available(book, NULL)) {
            count++;
        }
    }

    iterator_destroy(iter);
    return count;
}