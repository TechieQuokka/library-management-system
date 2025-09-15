#include "../../include/models/models.h"

/* Compare books by ISBN */
int compare_book_isbn(const void *a, const void *b) {
    const Book *book_a = (const Book *)a;
    const Book *book_b = (const Book *)b;
    return strcmp(book_a->isbn, book_b->isbn);
}

/* Compare books by title */
int compare_book_title(const void *a, const void *b) {
    const Book *book_a = (const Book *)a;
    const Book *book_b = (const Book *)b;
    return strcmp(book_a->title, book_b->title);
}

/* Compare books by author */
int compare_book_author(const void *a, const void *b) {
    const Book *book_a = (const Book *)a;
    const Book *book_b = (const Book *)b;
    return strcmp(book_a->author, book_b->author);
}

/* Print book information */
void print_book(const void *data) {
    const Book *book = (const Book *)data;
    printf("ISBN: %s | Title: %s | Author: %s | Available: %d/%d\n",
           book->isbn, book->title, book->author,
           book->available_copies, book->total_copies);
}

/* Initialize book structure */
void book_init(Book *book) {
    if (!book) return;

    memset(book->isbn, 0, sizeof(book->isbn));
    memset(book->title, 0, sizeof(book->title));
    memset(book->author, 0, sizeof(book->author));
    memset(book->publisher, 0, sizeof(book->publisher));
    book->publication_year = 0;
    memset(book->category, 0, sizeof(book->category));
    book->total_copies = 0;
    book->available_copies = 0;
    book->price = 0.0;
    book->status = 'A';
}

/* Validate book data */
bool validate_book(const Book *book) {
    if (!book) return false;

    /* Validate ISBN (13 digits) */
    if (!validate_isbn(book->isbn)) return false;

    /* Validate title */
    if (strlen(book->title) == 0 || strlen(book->title) > 100) return false;

    /* Validate author */
    if (strlen(book->author) == 0 || strlen(book->author) > 50) return false;

    /* Validate publisher */
    if (strlen(book->publisher) > 50) return false;

    /* Validate publication year */
    if (book->publication_year < 1000 || book->publication_year > 2030) return false;

    /* Validate category */
    if (strlen(book->category) > 30) return false;

    /* Validate quantities */
    if (book->total_copies < 0 || book->available_copies < 0) return false;
    if (book->available_copies > book->total_copies) return false;

    /* Validate price */
    if (book->price < 0) return false;

    /* Validate status */
    if (book->status != 'A' && book->status != 'D') return false;

    return true;
}

/* Validate ISBN-13 format */
bool validate_isbn(const char *isbn) {
    if (!isbn) return false;

    /* Check length */
    if (strlen(isbn) != 13) return false;

    /* Check if all characters are digits */
    for (int i = 0; i < 13; i++) {
        if (isbn[i] < '0' || isbn[i] > '9') return false;
    }

    /* Calculate ISBN-13 checksum */
    int sum = 0;
    for (int i = 0; i < 12; i++) {
        int digit = isbn[i] - '0';
        sum += (i % 2 == 0) ? digit : digit * 3;
    }

    int check_digit = (10 - (sum % 10)) % 10;
    return (isbn[12] - '0') == check_digit;
}