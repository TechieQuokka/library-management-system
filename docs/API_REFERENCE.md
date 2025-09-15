# API 레퍼런스 가이드

## 1. 개요

이 문서는 도서관 관리 시스템의 모든 API 함수와 데이터 구조에 대한 상세한 레퍼런스를 제공합니다.

## 2. 공통 데이터 타입

### 2.1 반환 코드 (Return Codes)

```c
typedef enum {
    LMS_SUCCESS = 0,                // 성공
    LMS_ERROR_NULL_POINTER,         // 널 포인터 에러
    LMS_ERROR_MEMORY,              // 메모리 할당 에러
    LMS_ERROR_NOT_FOUND,           // 데이터 찾을 수 없음
    LMS_ERROR_DUPLICATE,           // 중복 데이터
    LMS_ERROR_INVALID_INPUT,       // 잘못된 입력
    LMS_ERROR_LOAN_LIMIT,          // 대출 한도 초과
    LMS_ERROR_BOOK_UNAVAILABLE,    // 도서 대출 불가
    LMS_ERROR_FILE_IO,             // 파일 입출력 에러
    LMS_ERROR_PERMISSION_DENIED,   // 권한 없음
    LMS_ERROR_SYSTEM               // 시스템 에러
} LMS_Result;
```

### 2.2 함수 포인터 타입

```c
// 비교 함수 포인터
typedef int (*CompareFunc)(const void *a, const void *b);

// 출력 함수 포인터
typedef void (*PrintFunc)(const void *data);

// 조건 함수 포인터
typedef bool (*ConditionFunc)(const void *data, void *context);

// 변환 함수 포인터
typedef void* (*TransformFunc)(const void *data, void *context);

// 축약 함수 포인터
typedef void* (*ReduceFunc)(void *accumulator, const void *data, void *context);

// 데이터 해제 함수 포인터
typedef void (*FreeFunc)(void *data);

// 데이터 복사 함수 포인터
typedef void* (*CopyFunc)(const void *data);
```

## 3. Core Module API

### 3.1 Doubly Linked List API

#### 3.1.1 생성 및 초기화

```c
/**
 * @brief 새로운 이중 연결 리스트 생성
 * @param data_size 저장할 데이터의 크기 (바이트)
 * @param compare 비교 함수 포인터 (NULL 가능)
 * @param print 출력 함수 포인터 (NULL 가능)
 * @return 생성된 리스트 포인터, 실패시 NULL
 *
 * @example
 * DoublyLinkedList *list = dll_create(sizeof(int), compare_int, print_int);
 */
DoublyLinkedList* dll_create(size_t data_size, CompareFunc compare, PrintFunc print);

/**
 * @brief 리스트 초기화 (이미 할당된 메모리에 대해)
 * @param list 초기화할 리스트 포인터
 * @param data_size 데이터 크기
 * @param compare 비교 함수
 * @param print 출력 함수
 * @return 성공시 LMS_SUCCESS
 */
LMS_Result dll_init(DoublyLinkedList *list, size_t data_size,
                    CompareFunc compare, PrintFunc print);
```

#### 3.1.2 삽입 연산

```c
/**
 * @brief 리스트 앞쪽에 데이터 삽입
 * @param list 대상 리스트
 * @param data 삽입할 데이터 포인터
 * @return 성공시 LMS_SUCCESS, 실패시 에러 코드
 *
 * @note 시간복잡도: O(1)
 * @example
 * int value = 42;
 * LMS_Result result = dll_insert_front(list, &value);
 */
LMS_Result dll_insert_front(DoublyLinkedList *list, const void *data);

/**
 * @brief 리스트 뒤쪽에 데이터 삽입
 * @param list 대상 리스트
 * @param data 삽입할 데이터 포인터
 * @return 성공시 LMS_SUCCESS, 실패시 에러 코드
 *
 * @note 시간복잡도: O(1)
 */
LMS_Result dll_insert_rear(DoublyLinkedList *list, const void *data);

/**
 * @brief 지정된 위치에 데이터 삽입
 * @param list 대상 리스트
 * @param index 삽입할 위치 (0부터 시작, 음수는 뒤에서부터)
 * @param data 삽입할 데이터 포인터
 * @return 성공시 LMS_SUCCESS, 실패시 에러 코드
 *
 * @note 시간복잡도: O(n)
 * @warning index가 범위를 벗어나면 LMS_ERROR_INVALID_INPUT 반환
 */
LMS_Result dll_insert_at(DoublyLinkedList *list, int index, const void *data);

/**
 * @brief 정렬된 위치에 데이터 삽입
 * @param list 대상 리스트 (정렬된 상태여야 함)
 * @param data 삽입할 데이터 포인터
 * @return 성공시 LMS_SUCCESS, 실패시 에러 코드
 *
 * @note 시간복잡도: O(n)
 * @warning 리스트에 비교 함수가 설정되어 있어야 함
 */
LMS_Result dll_insert_sorted(DoublyLinkedList *list, const void *data);
```

#### 3.1.3 삭제 연산

```c
/**
 * @brief 리스트 앞쪽 데이터 삭제
 * @param list 대상 리스트
 * @return 성공시 LMS_SUCCESS, 실패시 에러 코드
 *
 * @note 시간복잡도: O(1)
 */
LMS_Result dll_delete_front(DoublyLinkedList *list);

/**
 * @brief 리스트 뒤쪽 데이터 삭제
 * @param list 대상 리스트
 * @return 성공시 LMS_SUCCESS, 실패시 에러 코드
 *
 * @note 시간복잡도: O(1)
 */
LMS_Result dll_delete_rear(DoublyLinkedList *list);

/**
 * @brief 지정된 위치의 데이터 삭제
 * @param list 대상 리스트
 * @param index 삭제할 위치 (0부터 시작)
 * @return 성공시 LMS_SUCCESS, 실패시 에러 코드
 *
 * @note 시간복잡도: O(n)
 */
LMS_Result dll_delete_at(DoublyLinkedList *list, int index);

/**
 * @brief 특정 데이터를 찾아서 삭제
 * @param list 대상 리스트
 * @param data 삭제할 데이터 포인터
 * @return 성공시 LMS_SUCCESS, 실패시 에러 코드
 *
 * @note 시간복잡도: O(n)
 */
LMS_Result dll_delete_data(DoublyLinkedList *list, const void *data);

/**
 * @brief 노드 직접 삭제
 * @param list 대상 리스트
 * @param node 삭제할 노드 포인터
 * @return 성공시 LMS_SUCCESS, 실패시 에러 코드
 *
 * @note 시간복잡도: O(1)
 * @warning 노드가 해당 리스트에 속해있는지 확인 필요
 */
LMS_Result dll_delete_node(DoublyLinkedList *list, Node *node);
```

#### 3.1.4 검색 연산

```c
/**
 * @brief 데이터 검색
 * @param list 대상 리스트
 * @param data 찾을 데이터 포인터
 * @return 찾은 노드 포인터, 못찾으면 NULL
 *
 * @note 시간복잡도: O(n)
 */
Node* dll_search(DoublyLinkedList *list, const void *data);

/**
 * @brief 조건에 맞는 첫 번째 데이터 검색
 * @param list 대상 리스트
 * @param condition 조건 함수
 * @param context 조건 함수에 전달할 컨텍스트
 * @return 찾은 노드 포인터, 못찾으면 NULL
 *
 * @note 시간복잡도: O(n)
 */
Node* dll_find_if(DoublyLinkedList *list, ConditionFunc condition, void *context);

/**
 * @brief 지정된 위치의 노드 가져오기
 * @param list 대상 리스트
 * @param index 위치 (0부터 시작, 음수는 뒤에서부터)
 * @return 노드 포인터, 실패시 NULL
 *
 * @note 시간복잡도: O(n)
 */
Node* dll_get_node_at(DoublyLinkedList *list, int index);

/**
 * @brief 지정된 위치의 데이터 가져오기
 * @param list 대상 리스트
 * @param index 위치 (0부터 시작)
 * @return 데이터 포인터, 실패시 NULL
 *
 * @note 시간복잡도: O(n)
 */
void* dll_get_at(DoublyLinkedList *list, int index);

/**
 * @brief 데이터의 인덱스 찾기
 * @param list 대상 리스트
 * @param data 찾을 데이터 포인터
 * @return 인덱스 (0부터 시작), 못찾으면 -1
 *
 * @note 시간복잡도: O(n)
 */
int dll_index_of(DoublyLinkedList *list, const void *data);
```

#### 3.1.5 정렬 및 조작

```c
/**
 * @brief 병합 정렬을 사용한 리스트 정렬
 * @param list 대상 리스트
 * @return 성공시 LMS_SUCCESS, 실패시 에러 코드
 *
 * @note 시간복잡도: O(n log n)
 * @warning 리스트에 비교 함수가 설정되어 있어야 함
 */
LMS_Result dll_sort(DoublyLinkedList *list);

/**
 * @brief 사용자 정의 비교 함수로 정렬
 * @param list 대상 리스트
 * @param compare 비교 함수
 * @return 성공시 LMS_SUCCESS, 실패시 에러 코드
 *
 * @note 시간복잡도: O(n log n)
 */
LMS_Result dll_sort_with(DoublyLinkedList *list, CompareFunc compare);

/**
 * @brief 리스트 뒤집기
 * @param list 대상 리스트
 * @return 성공시 LMS_SUCCESS, 실패시 에러 코드
 *
 * @note 시간복잡도: O(n)
 */
LMS_Result dll_reverse(DoublyLinkedList *list);

/**
 * @brief 리스트 복사
 * @param list 복사할 리스트
 * @return 복사된 새 리스트, 실패시 NULL
 *
 * @note 시간복잡도: O(n)
 */
DoublyLinkedList* dll_clone(DoublyLinkedList *list);
```

#### 3.1.6 유틸리티 함수

```c
/**
 * @brief 리스트 크기 반환
 * @param list 대상 리스트
 * @return 리스트 크기, 에러시 -1
 */
int dll_size(DoublyLinkedList *list);

/**
 * @brief 리스트가 비어있는지 확인
 * @param list 대상 리스트
 * @return 비어있으면 true, 아니면 false
 */
bool dll_is_empty(DoublyLinkedList *list);

/**
 * @brief 리스트가 정렬되어 있는지 확인
 * @param list 대상 리스트
 * @return 정렬되어 있으면 true, 아니면 false
 */
bool dll_is_sorted(DoublyLinkedList *list);

/**
 * @brief 리스트 내용 비우기
 * @param list 대상 리스트
 */
void dll_clear(DoublyLinkedList *list);

/**
 * @brief 리스트 메모리 해제
 * @param list 해제할 리스트
 */
void dll_destroy(DoublyLinkedList *list);
```

### 3.2 반복자 API

```c
/**
 * @brief 정방향 반복자 생성
 * @param list 대상 리스트
 * @return 반복자 포인터, 실패시 NULL
 */
Iterator* dll_iterator_create(DoublyLinkedList *list);

/**
 * @brief 역방향 반복자 생성
 * @param list 대상 리스트
 * @return 반복자 포인터, 실패시 NULL
 */
Iterator* dll_reverse_iterator_create(DoublyLinkedList *list);

/**
 * @brief 다음 요소로 이동
 * @param iter 반복자
 * @return 다음 데이터 포인터, 끝에 도달하면 NULL
 */
void* iterator_next(Iterator *iter);

/**
 * @brief 이전 요소로 이동
 * @param iter 반복자
 * @return 이전 데이터 포인터, 시작에 도달하면 NULL
 */
void* iterator_prev(Iterator *iter);

/**
 * @brief 반복자가 유효한지 확인
 * @param iter 반복자
 * @return 유효하면 true, 아니면 false
 */
bool iterator_has_next(Iterator *iter);

/**
 * @brief 현재 요소 가져오기
 * @param iter 반복자
 * @return 현재 데이터 포인터, 무효한 위치면 NULL
 */
void* iterator_current(Iterator *iter);

/**
 * @brief 반복자 재시작
 * @param iter 반복자
 */
void iterator_reset(Iterator *iter);

/**
 * @brief 반복자 해제
 * @param iter 반복자
 */
void iterator_destroy(Iterator *iter);
```

### 3.3 메모리 관리 API

```c
/**
 * @brief 메모리 매니저 초기화
 * @param pool_size 메모리 풀 크기
 * @return 성공시 LMS_SUCCESS
 */
LMS_Result memory_manager_init(size_t pool_size);

/**
 * @brief 메모리 할당
 * @param size 할당할 크기
 * @return 할당된 메모리 포인터, 실패시 NULL
 */
void* mem_alloc(size_t size);

/**
 * @brief 메모리 재할당
 * @param ptr 기존 메모리 포인터
 * @param new_size 새로운 크기
 * @return 재할당된 메모리 포인터, 실패시 NULL
 */
void* mem_realloc(void *ptr, size_t new_size);

/**
 * @brief 메모리 해제
 * @param ptr 해제할 메모리 포인터
 */
void mem_free(void *ptr);

/**
 * @brief 메모리 사용량 통계 출력
 */
void memory_print_stats(void);

/**
 * @brief 메모리 매니저 정리
 */
void memory_manager_cleanup(void);
```

## 4. Repository API

### 4.1 Book Repository API

```c
/**
 * @brief 도서 저장소 생성
 * @return 생성된 저장소 포인터, 실패시 NULL
 */
BookRepository* book_repository_create(void);

/**
 * @brief 도서 추가
 * @param repo 도서 저장소
 * @param book 추가할 도서
 * @return 성공시 LMS_SUCCESS
 */
LMS_Result book_repo_add(BookRepository *repo, const Book *book);

/**
 * @brief ISBN으로 도서 검색
 * @param repo 도서 저장소
 * @param isbn 검색할 ISBN
 * @return 찾은 도서 포인터, 못찾으면 NULL
 */
Book* book_repo_find_by_isbn(BookRepository *repo, const char *isbn);

/**
 * @brief 제목으로 도서 검색
 * @param repo 도서 저장소
 * @param title 검색할 제목
 * @return 검색 결과 리스트
 */
DoublyLinkedList* book_repo_find_by_title(BookRepository *repo, const char *title);

/**
 * @brief 저자로 도서 검색
 * @param repo 도서 저장소
 * @param author 검색할 저자
 * @return 검색 결과 리스트
 */
DoublyLinkedList* book_repo_find_by_author(BookRepository *repo, const char *author);

/**
 * @brief 카테고리로 도서 검색
 * @param repo 도서 저장소
 * @param category 검색할 카테고리
 * @return 검색 결과 리스트
 */
DoublyLinkedList* book_repo_find_by_category(BookRepository *repo, const char *category);

/**
 * @brief 도서 정보 수정
 * @param repo 도서 저장소
 * @param isbn 수정할 도서의 ISBN
 * @param updated_book 수정된 도서 정보
 * @return 성공시 LMS_SUCCESS
 */
LMS_Result book_repo_update(BookRepository *repo, const char *isbn, const Book *updated_book);

/**
 * @brief 도서 삭제
 * @param repo 도서 저장소
 * @param isbn 삭제할 도서의 ISBN
 * @return 성공시 LMS_SUCCESS
 */
LMS_Result book_repo_delete(BookRepository *repo, const char *isbn);

/**
 * @brief 도서 재고 수량 업데이트
 * @param repo 도서 저장소
 * @param isbn 도서 ISBN
 * @param change 변경량 (양수: 증가, 음수: 감소)
 * @return 성공시 LMS_SUCCESS
 */
LMS_Result book_repo_update_availability(BookRepository *repo, const char *isbn, int change);

/**
 * @brief 전체 도서 목록 가져오기
 * @param repo 도서 저장소
 * @return 전체 도서 리스트
 */
DoublyLinkedList* book_repo_get_all(BookRepository *repo);

/**
 * @brief 대출 가능한 도서 목록 가져오기
 * @param repo 도서 저장소
 * @return 대출 가능한 도서 리스트
 */
DoublyLinkedList* book_repo_get_available(BookRepository *repo);

/**
 * @brief 도서 저장소 해제
 * @param repo 해제할 저장소
 */
void book_repository_destroy(BookRepository *repo);
```

### 4.2 Member Repository API

```c
/**
 * @brief 회원 저장소 생성
 * @return 생성된 저장소 포인터, 실패시 NULL
 */
MemberRepository* member_repository_create(void);

/**
 * @brief 회원 등록
 * @param repo 회원 저장소
 * @param member 등록할 회원
 * @return 성공시 LMS_SUCCESS
 */
LMS_Result member_repo_add(MemberRepository *repo, const Member *member);

/**
 * @brief 회원 ID로 회원 검색
 * @param repo 회원 저장소
 * @param member_id 검색할 회원 ID
 * @return 찾은 회원 포인터, 못찾으면 NULL
 */
Member* member_repo_find_by_id(MemberRepository *repo, const char *member_id);

/**
 * @brief 이메일로 회원 검색
 * @param repo 회원 저장소
 * @param email 검색할 이메일
 * @return 찾은 회원 포인터, 못찾으면 NULL
 */
Member* member_repo_find_by_email(MemberRepository *repo, const char *email);

/**
 * @brief 전화번호로 회원 검색
 * @param repo 회원 저장소
 * @param phone 검색할 전화번호
 * @return 찾은 회원 포인터, 못찾으면 NULL
 */
Member* member_repo_find_by_phone(MemberRepository *repo, const char *phone);

/**
 * @brief 이름으로 회원 검색
 * @param repo 회원 저장소
 * @param name 검색할 이름
 * @return 검색 결과 리스트
 */
DoublyLinkedList* member_repo_find_by_name(MemberRepository *repo, const char *name);

/**
 * @brief 회원 정보 수정
 * @param repo 회원 저장소
 * @param member_id 수정할 회원 ID
 * @param updated_member 수정된 회원 정보
 * @return 성공시 LMS_SUCCESS
 */
LMS_Result member_repo_update(MemberRepository *repo, const char *member_id,
                             const Member *updated_member);

/**
 * @brief 회원 삭제
 * @param repo 회원 저장소
 * @param member_id 삭제할 회원 ID
 * @return 성공시 LMS_SUCCESS
 */
LMS_Result member_repo_delete(MemberRepository *repo, const char *member_id);

/**
 * @brief 회원 대출 수량 업데이트
 * @param repo 회원 저장소
 * @param member_id 회원 ID
 * @param change 변경량
 * @return 성공시 LMS_SUCCESS
 */
LMS_Result member_repo_update_loan_count(MemberRepository *repo, const char *member_id, int change);

/**
 * @brief 활성 회원 목록 가져오기
 * @param repo 회원 저장소
 * @return 활성 회원 리스트
 */
DoublyLinkedList* member_repo_get_active(MemberRepository *repo);

/**
 * @brief 전체 회원 목록 가져오기
 * @param repo 회원 저장소
 * @return 전체 회원 리스트
 */
DoublyLinkedList* member_repo_get_all(MemberRepository *repo);

/**
 * @brief 회원 저장소 해제
 * @param repo 해제할 저장소
 */
void member_repository_destroy(MemberRepository *repo);
```

### 4.3 Loan Repository API

```c
/**
 * @brief 대출 저장소 생성
 * @return 생성된 저장소 포인터, 실패시 NULL
 */
LoanRepository* loan_repository_create(void);

/**
 * @brief 대출 기록 추가
 * @param repo 대출 저장소
 * @param loan 추가할 대출 기록
 * @return 성공시 LMS_SUCCESS
 */
LMS_Result loan_repo_add(LoanRepository *repo, const Loan *loan);

/**
 * @brief 대출 ID로 대출 기록 검색
 * @param repo 대출 저장소
 * @param loan_id 검색할 대출 ID
 * @return 찾은 대출 기록 포인터, 못찾으면 NULL
 */
Loan* loan_repo_find_by_id(LoanRepository *repo, const char *loan_id);

/**
 * @brief 회원 ID로 대출 기록 검색
 * @param repo 대출 저장소
 * @param member_id 검색할 회원 ID
 * @return 검색 결과 리스트
 */
DoublyLinkedList* loan_repo_find_by_member(LoanRepository *repo, const char *member_id);

/**
 * @brief 도서 ISBN으로 대출 기록 검색
 * @param repo 대출 저장소
 * @param isbn 검색할 도서 ISBN
 * @return 검색 결과 리스트
 */
DoublyLinkedList* loan_repo_find_by_book(LoanRepository *repo, const char *isbn);

/**
 * @brief 활성 대출 목록 가져오기
 * @param repo 대출 저장소
 * @return 활성 대출 리스트
 */
DoublyLinkedList* loan_repo_get_active(LoanRepository *repo);

/**
 * @brief 연체 대출 목록 가져오기
 * @param repo 대출 저장소
 * @return 연체 대출 리스트
 */
DoublyLinkedList* loan_repo_get_overdue(LoanRepository *repo);

/**
 * @brief 대출 기록 수정
 * @param repo 대출 저장소
 * @param loan_id 수정할 대출 ID
 * @param updated_loan 수정된 대출 정보
 * @return 성공시 LMS_SUCCESS
 */
LMS_Result loan_repo_update(LoanRepository *repo, const char *loan_id, const Loan *updated_loan);

/**
 * @brief 대출 기록 삭제
 * @param repo 대출 저장소
 * @param loan_id 삭제할 대출 ID
 * @return 성공시 LMS_SUCCESS
 */
LMS_Result loan_repo_delete(LoanRepository *repo, const char *loan_id);

/**
 * @brief 전체 대출 기록 가져오기
 * @param repo 대출 저장소
 * @return 전체 대출 리스트
 */
DoublyLinkedList* loan_repo_get_all(LoanRepository *repo);

/**
 * @brief 대출 저장소 해제
 * @param repo 해제할 저장소
 */
void loan_repository_destroy(LoanRepository *repo);
```

## 5. Service API

### 5.1 Book Service API

```c
/**
 * @brief 도서 서비스 생성
 * @param book_repo 도서 저장소
 * @param loan_repo 대출 저장소
 * @return 생성된 서비스 포인터, 실패시 NULL
 */
BookService* book_service_create(BookRepository *book_repo, LoanRepository *loan_repo);

/**
 * @brief 도서 등록
 * @param service 도서 서비스
 * @param book 등록할 도서
 * @return 성공시 LMS_SUCCESS
 */
LMS_Result book_service_register(BookService *service, const Book *book);

/**
 * @brief 고급 도서 검색
 * @param service 도서 서비스
 * @param criteria 검색 조건
 * @return 검색 결과 리스트
 */
DoublyLinkedList* book_service_search(BookService *service, const BookSearchCriteria *criteria);

/**
 * @brief 도서 추천
 * @param service 도서 서비스
 * @param member_id 회원 ID
 * @return 추천 도서 리스트
 */
DoublyLinkedList* book_service_recommend(BookService *service, const char *member_id);

/**
 * @brief 인기 도서 목록
 * @param service 도서 서비스
 * @param limit 최대 개수
 * @return 인기 도서 리스트
 */
DoublyLinkedList* book_service_get_popular(BookService *service, int limit);

/**
 * @brief 신착 도서 목록
 * @param service 도서 서비스
 * @param days 최근 일수
 * @return 신착 도서 리스트
 */
DoublyLinkedList* book_service_get_new_arrivals(BookService *service, int days);

/**
 * @brief 도서 대출 가능 여부 확인
 * @param service 도서 서비스
 * @param isbn 도서 ISBN
 * @return 대출 가능하면 true
 */
bool book_service_is_available(BookService *service, const char *isbn);

/**
 * @brief 재고 보고서 생성
 * @param service 도서 서비스
 * @return 재고 보고서
 */
InventoryReport book_service_generate_inventory_report(BookService *service);

/**
 * @brief 도서 서비스 해제
 * @param service 해제할 서비스
 */
void book_service_destroy(BookService *service);
```

### 5.2 Member Service API

```c
/**
 * @brief 회원 서비스 생성
 * @param member_repo 회원 저장소
 * @param loan_repo 대출 저장소
 * @return 생성된 서비스 포인터, 실패시 NULL
 */
MemberService* member_service_create(MemberRepository *member_repo, LoanRepository *loan_repo);

/**
 * @brief 회원 등록
 * @param service 회원 서비스
 * @param member 등록할 회원
 * @return 성공시 LMS_SUCCESS
 */
LMS_Result member_service_register(MemberService *service, const Member *member);

/**
 * @brief 회원 대출 권한 확인
 * @param service 회원 서비스
 * @param member_id 회원 ID
 * @return 대출 가능하면 true
 */
bool member_service_can_borrow(MemberService *service, const char *member_id);

/**
 * @brief 남은 대출 한도 조회
 * @param service 회원 서비스
 * @param member_id 회원 ID
 * @return 남은 대출 가능 수량
 */
int member_service_get_remaining_limit(MemberService *service, const char *member_id);

/**
 * @brief 미납 연체료 조회
 * @param service 회원 서비스
 * @param member_id 회원 ID
 * @return 미납 연체료 총액
 */
double member_service_get_outstanding_fines(MemberService *service, const char *member_id);

/**
 * @brief 회원 정지
 * @param service 회원 서비스
 * @param member_id 회원 ID
 * @param reason 정지 사유
 * @return 성공시 LMS_SUCCESS
 */
LMS_Result member_service_suspend(MemberService *service, const char *member_id, const char *reason);

/**
 * @brief 회원 활성화
 * @param service 회원 서비스
 * @param member_id 회원 ID
 * @return 성공시 LMS_SUCCESS
 */
LMS_Result member_service_activate(MemberService *service, const char *member_id);

/**
 * @brief 회원 검색
 * @param service 회원 서비스
 * @param criteria 검색 조건
 * @return 검색 결과 리스트
 */
DoublyLinkedList* member_service_search(MemberService *service, const MemberSearchCriteria *criteria);

/**
 * @brief 회원 활동 보고서 생성
 * @param service 회원 서비스
 * @return 활동 보고서
 */
ActivityReport member_service_generate_activity_report(MemberService *service);

/**
 * @brief 회원 서비스 해제
 * @param service 해제할 서비스
 */
void member_service_destroy(MemberService *service);
```

### 5.3 Loan Service API

```c
/**
 * @brief 대출 서비스 생성
 * @param loan_repo 대출 저장소
 * @param book_repo 도서 저장소
 * @param member_repo 회원 저장소
 * @return 생성된 서비스 포인터, 실패시 NULL
 */
LoanService* loan_service_create(LoanRepository *loan_repo, BookRepository *book_repo,
                                MemberRepository *member_repo);

/**
 * @brief 도서 대출
 * @param service 대출 서비스
 * @param member_id 회원 ID
 * @param isbn 도서 ISBN
 * @return 성공시 LMS_SUCCESS
 */
LMS_Result loan_service_borrow_book(LoanService *service, const char *member_id, const char *isbn);

/**
 * @brief 도서 반납
 * @param service 대출 서비스
 * @param loan_id 대출 ID
 * @return 성공시 LMS_SUCCESS
 */
LMS_Result loan_service_return_book(LoanService *service, const char *loan_id);

/**
 * @brief 대출 연장
 * @param service 대출 서비스
 * @param loan_id 대출 ID
 * @return 성공시 LMS_SUCCESS
 */
LMS_Result loan_service_renew_loan(LoanService *service, const char *loan_id);

/**
 * @brief 대출 가능 여부 확인
 * @param service 대출 서비스
 * @param member_id 회원 ID
 * @param isbn 도서 ISBN
 * @return 대출 가능하면 true
 */
bool loan_service_can_borrow(LoanService *service, const char *member_id, const char *isbn);

/**
 * @brief 연장 가능 여부 확인
 * @param service 대출 서비스
 * @param loan_id 대출 ID
 * @return 연장 가능하면 true
 */
bool loan_service_can_renew(LoanService *service, const char *loan_id);

/**
 * @brief 연체 대출 목록 조회
 * @param service 대출 서비스
 * @return 연체 대출 리스트
 */
DoublyLinkedList* loan_service_get_overdue_loans(LoanService *service);

/**
 * @brief 연체료 계산
 * @param service 대출 서비스
 * @return 성공시 LMS_SUCCESS
 */
LMS_Result loan_service_calculate_fines(LoanService *service);

/**
 * @brief 연체료 납부 처리
 * @param service 대출 서비스
 * @param loan_id 대출 ID
 * @param amount 납부 금액
 * @return 성공시 LMS_SUCCESS
 */
LMS_Result loan_service_pay_fine(LoanService *service, const char *loan_id, double amount);

/**
 * @brief 대출 통계 조회
 * @param service 대출 서비스
 * @return 대출 통계
 */
LoanStatistics loan_service_get_statistics(LoanService *service);

/**
 * @brief 일일 대출 보고서 생성
 * @param service 대출 서비스
 * @param date 날짜 (YYYY-MM-DD)
 * @return 일일 보고서
 */
DailyLoanReport loan_service_generate_daily_report(LoanService *service, const char *date);

/**
 * @brief 대출 서비스 해제
 * @param service 해제할 서비스
 */
void loan_service_destroy(LoanService *service);
```

## 6. Utility API

### 6.1 문자열 유틸리티

```c
/**
 * @brief 문자열 앞뒤 공백 제거
 * @param str 대상 문자열
 * @return 처리된 문자열 포인터
 */
char* str_trim(char *str);

/**
 * @brief 문자열을 대문자로 변환
 * @param str 대상 문자열
 * @return 처리된 문자열 포인터
 */
char* str_to_upper(char *str);

/**
 * @brief 문자열을 소문자로 변환
 * @param str 대상 문자열
 * @return 처리된 문자열 포인터
 */
char* str_to_lower(char *str);

/**
 * @brief ISBN 유효성 검증
 * @param isbn 검증할 ISBN
 * @return 유효하면 true
 */
bool validate_isbn(const char *isbn);

/**
 * @brief 이메일 유효성 검증
 * @param email 검증할 이메일
 * @return 유효하면 true
 */
bool validate_email(const char *email);

/**
 * @brief 전화번호 유효성 검증
 * @param phone 검증할 전화번호
 * @return 유효하면 true
 */
bool validate_phone(const char *phone);

/**
 * @brief 고유 ID 생성
 * @param prefix 접두사
 * @return 생성된 ID 문자열
 */
char* generate_unique_id(const char *prefix);
```

### 6.2 날짜 유틸리티

```c
/**
 * @brief 현재 날짜 가져오기
 * @return 현재 날짜 문자열 (YYYY-MM-DD)
 */
char* get_current_date(void);

/**
 * @brief 날짜에 일수 더하기
 * @param date 기준 날짜 (YYYY-MM-DD)
 * @param days 더할 일수
 * @return 계산된 날짜 문자열
 */
char* add_days_to_date(const char *date, int days);

/**
 * @brief 두 날짜 간의 일수 차이 계산
 * @param date1 첫 번째 날짜
 * @param date2 두 번째 날짜
 * @return 일수 차이
 */
int days_between_dates(const char *date1, const char *date2);

/**
 * @brief 날짜 유효성 검증
 * @param date 검증할 날짜 (YYYY-MM-DD)
 * @return 유효하면 true
 */
bool validate_date(const char *date);

/**
 * @brief 과거 날짜인지 확인
 * @param date 확인할 날짜
 * @return 과거 날짜면 true
 */
bool is_past_date(const char *date);
```

### 6.3 에러 처리

```c
/**
 * @brief 에러 메시지 반환
 * @param error 에러 코드
 * @return 에러 메시지 문자열
 */
const char* get_error_message(LMS_Result error);

/**
 * @brief 에러 로그 기록
 * @param error 에러 코드
 * @param function 함수명
 * @param line 라인 번호
 */
void log_error(LMS_Result error, const char *function, int line);

/**
 * @brief 경고 로그 기록
 * @param message 경고 메시지
 */
void log_warning(const char *message, ...);

/**
 * @brief 정보 로그 기록
 * @param message 정보 메시지
 */
void log_info(const char *message, ...);

/**
 * @brief 디버그 로그 기록
 * @param message 디버그 메시지
 */
void log_debug(const char *message, ...);
```

## 7. 사용 예제

### 7.1 기본 리스트 사용

```c
#include "doubly_linked_list.h"

// 정수 비교 함수
int compare_int(const void *a, const void *b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

// 정수 출력 함수
void print_int(const void *data) {
    printf("%d", *(const int*)data);
}

int main() {
    // 리스트 생성
    DoublyLinkedList *list = dll_create(sizeof(int), compare_int, print_int);

    // 데이터 삽입
    int values[] = {10, 20, 30, 15, 25};
    for (int i = 0; i < 5; i++) {
        dll_insert_rear(list, &values[i]);
    }

    // 정렬
    dll_sort(list);

    // 반복자로 순회
    Iterator *iter = dll_iterator_create(list);
    while (iterator_has_next(iter)) {
        int *value = (int*)iterator_next(iter);
        printf("%d ", *value);
    }
    iterator_destroy(iter);

    // 메모리 해제
    dll_destroy(list);
    return 0;
}
```

### 7.2 도서 관리 사용

```c
#include "book_service.h"

int main() {
    // 저장소 생성
    BookRepository *book_repo = book_repository_create();
    LoanRepository *loan_repo = loan_repository_create();

    // 서비스 생성
    BookService *book_service = book_service_create(book_repo, loan_repo);

    // 도서 등록
    Book book = {
        .isbn = "9788956746425",
        .title = "Clean Code",
        .author = "Robert C. Martin",
        .publisher = "Insight",
        .publication_year = 2013,
        .category = "Programming",
        .total_copies = 5,
        .available_copies = 5,
        .price = 32000.0,
        .status = 'A'
    };

    LMS_Result result = book_service_register(book_service, &book);
    if (result == LMS_SUCCESS) {
        printf("도서가 성공적으로 등록되었습니다.\n");
    }

    // 검색 조건 설정
    BookSearchCriteria criteria = {
        .search_by_title = true,
        .title = "Clean",
        .search_by_author = false,
        .search_by_category = false
    };

    // 도서 검색
    DoublyLinkedList *results = book_service_search(book_service, &criteria);

    // 검색 결과 출력
    if (!dll_is_empty(results)) {
        printf("검색된 도서:\n");
        Iterator *iter = dll_iterator_create(results);
        while (iterator_has_next(iter)) {
            Book *found_book = (Book*)iterator_next(iter);
            printf("제목: %s, 저자: %s\n", found_book->title, found_book->author);
        }
        iterator_destroy(iter);
    }

    // 메모리 해제
    dll_destroy(results);
    book_service_destroy(book_service);
    book_repository_destroy(book_repo);
    loan_repository_destroy(loan_repo);

    return 0;
}
```

이 API 레퍼런스는 시스템의 모든 주요 함수와 사용법을 상세히 설명하여 개발자가 효율적으로 라이브러리를 활용할 수 있도록 돕습니다.