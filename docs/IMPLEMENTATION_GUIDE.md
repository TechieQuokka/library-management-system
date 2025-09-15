# 구현 가이드 (Implementation Guide)

## 1. 개발 환경 설정

### 1.1 필수 도구

#### 컴파일러
- **GCC**: 버전 7.0 이상 (C11 표준 지원)
- **Clang**: 버전 6.0 이상 (대안)
- **MSVC**: Visual Studio 2017 이상 (Windows)

#### 빌드 도구
- **Make**: GNU Make 4.0 이상
- **CMake**: 3.10 이상 (선택사항)

#### 디버깅 도구
- **GDB**: GNU Debugger
- **Valgrind**: 메모리 누수 검증 (Linux/macOS)
- **AddressSanitizer**: 메모리 에러 검출

### 1.2 프로젝트 구조 생성

```bash
# 프로젝트 디렉토리 생성
mkdir -p Doubly-Linked-List/{src/{core,models,services,repositories,ui,utils},include,tests,examples,docs}

# 기본 파일 생성
touch Doubly-Linked-List/{Makefile,README.md,.gitignore}
```

### 1.3 Makefile 기본 설정

```makefile
# Makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -O0
LDFLAGS =
SRCDIR = src
INCDIR = include
OBJDIR = obj
TESTDIR = tests
TARGET = library_system

# 소스 파일 자동 검색
SOURCES = $(shell find $(SRCDIR) -name "*.c")
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
HEADERS = $(shell find $(INCDIR) -name "*.h")

# 기본 타겟
all: $(TARGET)

# 실행 파일 생성
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

# 오브젝트 파일 생성
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

# 테스트 빌드
test: CFLAGS += -DTEST_MODE
test: $(OBJECTS)
	$(CC) $(OBJECTS) $(TESTDIR)/*.c -I$(INCDIR) -o test_runner $(LDFLAGS)
	./test_runner

# 디버그 빌드
debug: CFLAGS += -DDEBUG -fsanitize=address
debug: LDFLAGS += -fsanitize=address
debug: $(TARGET)

# 메모리 검사
memcheck: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

# 정리
clean:
	rm -rf $(OBJDIR) $(TARGET) test_runner

.PHONY: all test debug memcheck clean
```

## 2. 단계별 구현 가이드

### 2.1 Phase 1: Core Infrastructure (1-2주)

#### 2.1.1 기본 데이터 구조 정의

**include/common.h**
```c
#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <assert.h>

// 에러 코드 정의
typedef enum {
    LMS_SUCCESS = 0,
    LMS_ERROR_NULL_POINTER,
    LMS_ERROR_MEMORY,
    LMS_ERROR_NOT_FOUND,
    LMS_ERROR_DUPLICATE,
    LMS_ERROR_INVALID_INPUT,
    LMS_ERROR_LOAN_LIMIT,
    LMS_ERROR_BOOK_UNAVAILABLE,
    LMS_ERROR_FILE_IO,
    LMS_ERROR_PERMISSION_DENIED,
    LMS_ERROR_SYSTEM
} LMS_Result;

// 로그 레벨
typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
} LogLevel;

// 유틸리티 매크로
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

// 메모리 안전 매크로
#define SAFE_FREE(ptr) do { \
    if ((ptr) != NULL) { \
        free(ptr); \
        (ptr) = NULL; \
    } \
} while(0)

#define CHECK_NULL(ptr) do { \
    if ((ptr) == NULL) { \
        log_error(LMS_ERROR_NULL_POINTER, __FUNCTION__, __LINE__); \
        return LMS_ERROR_NULL_POINTER; \
    } \
} while(0)

#endif // COMMON_H
```

#### 2.1.2 Doubly Linked List 핵심 구현

**include/doubly_linked_list.h**
```c
#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

#include "common.h"

// 전방 선언
typedef struct Node Node;
typedef struct DoublyLinkedList DoublyLinkedList;
typedef struct Iterator Iterator;

// 함수 포인터 타입
typedef int (*CompareFunc)(const void *a, const void *b);
typedef void (*PrintFunc)(const void *data);
typedef bool (*ConditionFunc)(const void *data, void *context);
typedef void (*FreeFunc)(void *data);
typedef void* (*CopyFunc)(const void *data);

// 노드 구조체
struct Node {
    void *data;
    Node *prev;
    Node *next;
};

// 리스트 구조체
struct DoublyLinkedList {
    Node *head;
    Node *tail;
    int size;
    size_t data_size;
    CompareFunc compare;
    PrintFunc print;
    FreeFunc free_data;
    CopyFunc copy_data;
};

// 반복자 구조체
struct Iterator {
    Node *current;
    DoublyLinkedList *list;
    int direction; // 1: forward, -1: backward
};

// 핵심 함수 선언
DoublyLinkedList* dll_create(size_t data_size, CompareFunc compare, PrintFunc print);
LMS_Result dll_init(DoublyLinkedList *list, size_t data_size, CompareFunc compare, PrintFunc print);

// 삽입/삭제
LMS_Result dll_insert_front(DoublyLinkedList *list, const void *data);
LMS_Result dll_insert_rear(DoublyLinkedList *list, const void *data);
LMS_Result dll_insert_at(DoublyLinkedList *list, int index, const void *data);
LMS_Result dll_insert_sorted(DoublyLinkedList *list, const void *data);

LMS_Result dll_delete_front(DoublyLinkedList *list);
LMS_Result dll_delete_rear(DoublyLinkedList *list);
LMS_Result dll_delete_at(DoublyLinkedList *list, int index);
LMS_Result dll_delete_data(DoublyLinkedList *list, const void *data);
LMS_Result dll_delete_node(DoublyLinkedList *list, Node *node);

// 검색/접근
Node* dll_search(DoublyLinkedList *list, const void *data);
Node* dll_find_if(DoublyLinkedList *list, ConditionFunc condition, void *context);
Node* dll_get_node_at(DoublyLinkedList *list, int index);
void* dll_get_at(DoublyLinkedList *list, int index);
int dll_index_of(DoublyLinkedList *list, const void *data);

// 정렬/조작
LMS_Result dll_sort(DoublyLinkedList *list);
LMS_Result dll_reverse(DoublyLinkedList *list);
DoublyLinkedList* dll_clone(DoublyLinkedList *list);

// 유틸리티
int dll_size(DoublyLinkedList *list);
bool dll_is_empty(DoublyLinkedList *list);
bool dll_is_sorted(DoublyLinkedList *list);
void dll_clear(DoublyLinkedList *list);
void dll_destroy(DoublyLinkedList *list);

// 반복자
Iterator* dll_iterator_create(DoublyLinkedList *list);
Iterator* dll_reverse_iterator_create(DoublyLinkedList *list);
void* iterator_next(Iterator *iter);
bool iterator_has_next(Iterator *iter);
void iterator_destroy(Iterator *iter);

#endif // DOUBLY_LINKED_LIST_H
```

#### 2.1.3 구현 시작하기

**src/core/doubly_linked_list.c** (일부 핵심 함수)
```c
#include "doubly_linked_list.h"

// 노드 생성 (내부 함수)
static Node* node_create(const void *data, size_t data_size) {
    Node *node = malloc(sizeof(Node));
    if (!node) return NULL;

    node->data = malloc(data_size);
    if (!node->data) {
        free(node);
        return NULL;
    }

    memcpy(node->data, data, data_size);
    node->prev = NULL;
    node->next = NULL;

    return node;
}

// 리스트 생성
DoublyLinkedList* dll_create(size_t data_size, CompareFunc compare, PrintFunc print) {
    DoublyLinkedList *list = malloc(sizeof(DoublyLinkedList));
    if (!list) return NULL;

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    list->data_size = data_size;
    list->compare = compare;
    list->print = print;
    list->free_data = NULL;
    list->copy_data = NULL;

    return list;
}

// 뒤쪽에 삽입
LMS_Result dll_insert_rear(DoublyLinkedList *list, const void *data) {
    CHECK_NULL(list);
    CHECK_NULL(data);

    Node *new_node = node_create(data, list->data_size);
    if (!new_node) return LMS_ERROR_MEMORY;

    if (list->size == 0) {
        // 첫 번째 노드
        list->head = list->tail = new_node;
    } else {
        // 기존 tail 뒤에 추가
        new_node->prev = list->tail;
        list->tail->next = new_node;
        list->tail = new_node;
    }

    list->size++;
    return LMS_SUCCESS;
}

// 검색
Node* dll_search(DoublyLinkedList *list, const void *data) {
    if (!list || !data || !list->compare) return NULL;

    Node *current = list->head;
    while (current) {
        if (list->compare(current->data, data) == 0) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

// 메모리 해제
void dll_destroy(DoublyLinkedList *list) {
    if (!list) return;

    dll_clear(list);
    free(list);
}

void dll_clear(DoublyLinkedList *list) {
    if (!list) return;

    Node *current = list->head;
    while (current) {
        Node *next = current->next;
        if (list->free_data) {
            list->free_data(current->data);
        } else {
            free(current->data);
        }
        free(current);
        current = next;
    }

    list->head = list->tail = NULL;
    list->size = 0;
}
```

### 2.2 Phase 2: Data Models (1주)

#### 2.2.1 데이터 모델 정의

**include/models.h**
```c
#ifndef MODELS_H
#define MODELS_H

#include "common.h"

// 도서 구조체
typedef struct {
    char isbn[14];          // ISBN-13 + null terminator
    char title[101];        // 제목
    char author[51];        // 저자
    char publisher[51];     // 출판사
    int publication_year;   // 출판년도
    char category[31];      // 카테고리
    int total_copies;       // 총 보유 수량
    int available_copies;   // 대출 가능 수량
    double price;           // 가격
    char status;            // 상태 ('A': 활성, 'D': 삭제)
} Book;

// 회원 구조체
typedef struct {
    char member_id[11];     // 회원 ID
    char name[51];          // 이름
    char phone[16];         // 전화번호
    char email[101];        // 이메일
    char address[201];      // 주소
    char join_date[11];     // 가입일 (YYYY-MM-DD)
    char membership_type;   // 회원 유형 ('R': 일반, 'P': 프리미엄)
    int loan_count;         // 현재 대출 수량
    char status;            // 상태 ('A': 활성, 'S': 정지, 'D': 삭제)
} Member;

// 대출 구조체
typedef struct {
    char loan_id[11];       // 대출 ID
    char member_id[11];     // 회원 ID
    char isbn[14];          // 도서 ISBN
    char loan_date[11];     // 대출일
    char due_date[11];      // 반납 예정일
    char return_date[11];   // 실제 반납일 (빈 문자열이면 미반납)
    int overdue_days;       // 연체 일수
    double fine_amount;     // 연체료
    char status;            // 상태 ('L': 대출중, 'R': 반납, 'O': 연체)
} Loan;

// 비교 함수들
int compare_book_isbn(const void *a, const void *b);
int compare_book_title(const void *a, const void *b);
int compare_member_id(const void *a, const void *b);
int compare_loan_id(const void *a, const void *b);

// 출력 함수들
void print_book(const void *book);
void print_member(const void *member);
void print_loan(const void *loan);

// 검증 함수들
bool validate_book(const Book *book);
bool validate_member(const Member *member);
bool validate_loan(const Loan *loan);

#endif // MODELS_H
```

#### 2.2.2 모델 구현

**src/models/book.c**
```c
#include "models.h"

int compare_book_isbn(const void *a, const void *b) {
    const Book *book_a = (const Book *)a;
    const Book *book_b = (const Book *)b;
    return strcmp(book_a->isbn, book_b->isbn);
}

int compare_book_title(const void *a, const void *b) {
    const Book *book_a = (const Book *)a;
    const Book *book_b = (const Book *)b;
    return strcmp(book_a->title, book_b->title);
}

void print_book(const void *data) {
    const Book *book = (const Book *)data;
    printf("ISBN: %s, 제목: %s, 저자: %s, 재고: %d/%d\n",
           book->isbn, book->title, book->author,
           book->available_copies, book->total_copies);
}

bool validate_book(const Book *book) {
    if (!book) return false;

    // ISBN 검증 (13자리 숫자)
    if (strlen(book->isbn) != 13) return false;
    for (int i = 0; i < 13; i++) {
        if (book->isbn[i] < '0' || book->isbn[i] > '9') return false;
    }

    // 제목 검증
    if (strlen(book->title) == 0 || strlen(book->title) > 100) return false;

    // 저자 검증
    if (strlen(book->author) == 0 || strlen(book->author) > 50) return false;

    // 수량 검증
    if (book->total_copies < 0 || book->available_copies < 0) return false;
    if (book->available_copies > book->total_copies) return false;

    // 가격 검증
    if (book->price < 0) return false;

    return true;
}
```

### 2.3 Phase 3: Repository Layer (2주)

#### 2.3.1 Repository 인터페이스 설계

**include/repositories/book_repository.h**
```c
#ifndef BOOK_REPOSITORY_H
#define BOOK_REPOSITORY_H

#include "models.h"
#include "doubly_linked_list.h"

typedef struct BookRepository {
    DoublyLinkedList *books;        // 메인 도서 리스트
    DoublyLinkedList *isbn_index;   // ISBN 인덱스
    DoublyLinkedList *title_index;  // 제목 인덱스
    DoublyLinkedList *author_index; // 저자 인덱스
} BookRepository;

// 생성/소멸
BookRepository* book_repository_create(void);
void book_repository_destroy(BookRepository *repo);

// CRUD 연산
LMS_Result book_repo_add(BookRepository *repo, const Book *book);
Book* book_repo_find_by_isbn(BookRepository *repo, const char *isbn);
DoublyLinkedList* book_repo_find_by_title(BookRepository *repo, const char *title);
DoublyLinkedList* book_repo_find_by_author(BookRepository *repo, const char *author);
LMS_Result book_repo_update(BookRepository *repo, const char *isbn, const Book *updated_book);
LMS_Result book_repo_delete(BookRepository *repo, const char *isbn);

// 유틸리티
DoublyLinkedList* book_repo_get_all(BookRepository *repo);
DoublyLinkedList* book_repo_get_available(BookRepository *repo);
LMS_Result book_repo_update_availability(BookRepository *repo, const char *isbn, int change);

#endif // BOOK_REPOSITORY_H
```

#### 2.3.2 Repository 구현

**src/repositories/book_repository.c**
```c
#include "repositories/book_repository.h"

BookRepository* book_repository_create(void) {
    BookRepository *repo = malloc(sizeof(BookRepository));
    if (!repo) return NULL;

    repo->books = dll_create(sizeof(Book), compare_book_isbn, print_book);
    if (!repo->books) {
        free(repo);
        return NULL;
    }

    repo->isbn_index = dll_create(sizeof(Book*), NULL, NULL);
    repo->title_index = dll_create(sizeof(Book*), NULL, NULL);
    repo->author_index = dll_create(sizeof(Book*), NULL, NULL);

    if (!repo->isbn_index || !repo->title_index || !repo->author_index) {
        book_repository_destroy(repo);
        return NULL;
    }

    return repo;
}

LMS_Result book_repo_add(BookRepository *repo, const Book *book) {
    CHECK_NULL(repo);
    CHECK_NULL(book);

    if (!validate_book(book)) {
        return LMS_ERROR_INVALID_INPUT;
    }

    // 중복 검사
    if (book_repo_find_by_isbn(repo, book->isbn)) {
        return LMS_ERROR_DUPLICATE;
    }

    // 메인 리스트에 추가
    LMS_Result result = dll_insert_sorted(repo->books, book);
    if (result != LMS_SUCCESS) {
        return result;
    }

    // 인덱스 업데이트는 별도 함수로 처리
    update_indexes(repo, book);

    return LMS_SUCCESS;
}

Book* book_repo_find_by_isbn(BookRepository *repo, const char *isbn) {
    if (!repo || !isbn) return NULL;

    Book search_book;
    strncpy(search_book.isbn, isbn, sizeof(search_book.isbn) - 1);
    search_book.isbn[sizeof(search_book.isbn) - 1] = '\0';

    Node *found = dll_search(repo->books, &search_book);
    return found ? (Book*)found->data : NULL;
}

void book_repository_destroy(BookRepository *repo) {
    if (!repo) return;

    dll_destroy(repo->books);
    dll_destroy(repo->isbn_index);
    dll_destroy(repo->title_index);
    dll_destroy(repo->author_index);
    free(repo);
}
```

### 2.4 Phase 4: Service Layer (2주)

#### 2.4.1 서비스 계층 설계

서비스 계층은 비즈니스 로직을 담당하며, Repository 계층과 UI 계층 사이의 중간 역할을 합니다.

**include/services/book_service.h**
```c
#ifndef BOOK_SERVICE_H
#define BOOK_SERVICE_H

#include "repositories/book_repository.h"
#include "repositories/loan_repository.h"

typedef struct BookService {
    BookRepository *book_repo;
    LoanRepository *loan_repo;
} BookService;

// 검색 조건 구조체
typedef struct BookSearchCriteria {
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

// 서비스 생성/소멸
BookService* book_service_create(BookRepository *book_repo, LoanRepository *loan_repo);
void book_service_destroy(BookService *service);

// 비즈니스 로직
LMS_Result book_service_register_book(BookService *service, const Book *book);
LMS_Result book_service_update_book(BookService *service, const char *isbn, const Book *book);
LMS_Result book_service_remove_book(BookService *service, const char *isbn);

// 고급 검색
DoublyLinkedList* book_service_search(BookService *service, const BookSearchCriteria *criteria);
DoublyLinkedList* book_service_get_recommendations(BookService *service, const char *member_id);
DoublyLinkedList* book_service_get_popular_books(BookService *service, int limit);

// 재고 관리
bool book_service_is_available_for_loan(BookService *service, const char *isbn);
int book_service_get_available_count(BookService *service, const char *isbn);
LMS_Result book_service_reserve_book(BookService *service, const char *isbn);
LMS_Result book_service_release_reservation(BookService *service, const char *isbn);

#endif // BOOK_SERVICE_H
```

### 2.5 Phase 5: UI Layer (1-2주)

#### 2.5.1 메뉴 시스템 구현

**include/ui/menu_system.h**
```c
#ifndef MENU_SYSTEM_H
#define MENU_SYSTEM_H

#include "common.h"

typedef struct MenuItem {
    int id;
    char title[50];
    void (*action)(void);
    bool enabled;
} MenuItem;

typedef struct Menu {
    char title[100];
    MenuItem *items;
    int item_count;
    int max_items;
} Menu;

typedef struct MenuSystem {
    Menu *current_menu;
    Menu *main_menu;
    Menu **menu_stack;
    int stack_size;
    int stack_capacity;
} MenuSystem;

// 메뉴 시스템 함수들
MenuSystem* menu_system_create(void);
void menu_system_destroy(MenuSystem *system);

Menu* menu_create(const char *title, int max_items);
void menu_destroy(Menu *menu);
LMS_Result menu_add_item(Menu *menu, int id, const char *title, void (*action)(void));

void menu_system_display_current(MenuSystem *system);
LMS_Result menu_system_handle_input(MenuSystem *system);
void menu_system_push(MenuSystem *system, Menu *menu);
Menu* menu_system_pop(MenuSystem *system);

#endif // MENU_SYSTEM_H
```

### 2.6 Phase 6: Integration & Testing (1-2주)

#### 2.6.1 통합 테스트 프레임워크

**tests/test_framework.h**
```c
#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include "common.h"

#define TEST_ASSERT(condition) do { \
    if (!(condition)) { \
        printf("FAIL: %s:%d - %s\n", __FILE__, __LINE__, #condition); \
        return false; \
    } \
} while(0)

#define RUN_TEST(test_func) do { \
    printf("Running %s... ", #test_func); \
    if (test_func()) { \
        printf("PASS\n"); \
        passed_tests++; \
    } else { \
        printf("FAIL\n"); \
        failed_tests++; \
    } \
    total_tests++; \
} while(0)

// 테스트 통계 변수
extern int total_tests;
extern int passed_tests;
extern int failed_tests;

// 테스트 함수들
bool test_dll_basic_operations(void);
bool test_dll_sorting(void);
bool test_book_repository(void);
bool test_member_repository(void);
bool test_loan_service(void);

void run_all_tests(void);

#endif // TEST_FRAMEWORK_H
```

## 3. 빌드 및 배포

### 3.1 자동화된 빌드 스크립트

**build.sh**
```bash
#!/bin/bash

set -e  # 에러 발생시 스크립트 중단

echo "=== Library Management System Build Script ==="

# 변수 설정
BUILD_TYPE=${1:-release}  # debug 또는 release
CC=${CC:-gcc}
PROJECT_ROOT=$(pwd)

# 디렉토리 생성
mkdir -p obj bin

echo "Building in $BUILD_TYPE mode..."

case $BUILD_TYPE in
    debug)
        CFLAGS="-Wall -Wextra -std=c11 -g -O0 -DDEBUG -fsanitize=address"
        LDFLAGS="-fsanitize=address"
        ;;
    release)
        CFLAGS="-Wall -Wextra -std=c11 -O2 -DNDEBUG"
        LDFLAGS=""
        ;;
    *)
        echo "Error: Unknown build type '$BUILD_TYPE'"
        echo "Usage: $0 [debug|release]"
        exit 1
        ;;
esac

# 빌드 실행
echo "Compiling..."
make clean
make CC=$CC CFLAGS="$CFLAGS" LDFLAGS="$LDFLAGS"

# 테스트 실행 (debug 모드에서만)
if [ "$BUILD_TYPE" = "debug" ]; then
    echo "Running tests..."
    make test
fi

echo "Build completed successfully!"
```

### 3.2 지속적 통합 설정

**.github/workflows/ci.yml**
```yaml
name: CI

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

jobs:
  test:
    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v2

    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y valgrind

    - name: Build debug
      run: ./build.sh debug

    - name: Run tests
      run: make test

    - name: Memory check
      run: make memcheck

    - name: Build release
      run: ./build.sh release
```

## 4. 디버깅 및 최적화

### 4.1 디버깅 도구 활용

#### GDB 사용법
```bash
# 디버그 모드로 컴파일
make debug

# GDB로 실행
gdb ./library_system

# GDB 내에서 유용한 명령어들
(gdb) break main          # main 함수에 브레이크포인트
(gdb) break dll_insert_rear  # 특정 함수에 브레이크포인트
(gdb) run                 # 프로그램 실행
(gdb) next                # 다음 줄로 이동
(gdb) step                # 함수 내부로 들어가기
(gdb) print variable_name # 변수 값 확인
(gdb) backtrace          # 호출 스택 확인
```

#### Valgrind 메모리 검사
```bash
# 메모리 누수 검사
valgrind --leak-check=full --show-leak-kinds=all ./library_system

# 메모리 에러 검사
valgrind --tool=memcheck --track-origins=yes ./library_system
```

### 4.2 성능 최적화

#### 프로파일링
```bash
# gprof를 사용한 프로파일링
gcc -pg -O2 src/*.c -o library_system
./library_system
gprof library_system gmon.out > profile_report.txt
```

#### 최적화 기법

1. **메모리 풀 사용**
```c
// 메모리 풀 구현 예시
typedef struct MemoryPool {
    void *pool;
    size_t block_size;
    size_t total_blocks;
    void **free_list;
    size_t free_count;
} MemoryPool;

MemoryPool* memory_pool_create(size_t block_size, size_t block_count);
void* memory_pool_alloc(MemoryPool *pool);
void memory_pool_free(MemoryPool *pool, void *ptr);
```

2. **캐시 활용**
```c
// 최근 검색 결과 캐싱
typedef struct SearchCache {
    char query[256];
    DoublyLinkedList *results;
    time_t timestamp;
} SearchCache;
```

## 5. 문서화 및 배포

### 5.1 API 문서 자동 생성

**Doxygen 설정 (Doxyfile)**
```
PROJECT_NAME = "Library Management System"
OUTPUT_DIRECTORY = docs/api
INPUT = include/ src/
RECURSIVE = YES
GENERATE_HTML = YES
GENERATE_LATEX = NO
EXTRACT_ALL = YES
```

### 5.2 사용자 매뉴얼

**docs/USER_MANUAL.md**에 다음 내용 포함:
- 설치 방법
- 기본 사용법
- 주요 기능 설명
- 문제 해결 가이드
- FAQ

### 5.3 배포 패키지 생성

**package.sh**
```bash
#!/bin/bash

VERSION=$(git describe --tags --always)
PACKAGE_NAME="library_management_system_${VERSION}"

echo "Creating package: $PACKAGE_NAME"

# 릴리스 빌드
./build.sh release

# 패키지 디렉토리 생성
mkdir -p "dist/$PACKAGE_NAME"

# 필요한 파일들 복사
cp library_system "dist/$PACKAGE_NAME/"
cp -r docs/ "dist/$PACKAGE_NAME/"
cp README.md "dist/$PACKAGE_NAME/"
cp LICENSE "dist/$PACKAGE_NAME/"

# 압축
cd dist
tar -czf "${PACKAGE_NAME}.tar.gz" "$PACKAGE_NAME"
zip -r "${PACKAGE_NAME}.zip" "$PACKAGE_NAME"

echo "Package created successfully!"
```

## 6. 마무리 체크리스트

### 6.1 코드 품질 검증
- [ ] 모든 함수에 적절한 에러 처리 구현
- [ ] 메모리 누수 없음 (Valgrind 검증 완료)
- [ ] 모든 공개 함수에 문서화 주석 작성
- [ ] 단위 테스트 커버리지 80% 이상
- [ ] 코딩 스타일 가이드 준수

### 6.2 기능 완성도
- [ ] 모든 핵심 기능 구현 완료
- [ ] 사용자 인터페이스 완성
- [ ] 에러 처리 및 로깅 시스템 구현
- [ ] 데이터 무결성 보장

### 6.3 문서화
- [ ] README 파일 작성
- [ ] API 레퍼런스 완성
- [ ] 사용자 매뉴얼 작성
- [ ] 설치 가이드 작성

### 6.4 테스트
- [ ] 단위 테스트 작성 및 실행
- [ ] 통합 테스트 실행
- [ ] 성능 테스트 실행
- [ ] 사용자 시나리오 테스트

이 구현 가이드를 따라 단계별로 진행하면 견고하고 확장 가능한 도서관 관리 시스템을 완성할 수 있습니다.