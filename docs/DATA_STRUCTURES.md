# Doubly Linked List 자료구조 상세 설계

## 1. Doubly Linked List 개념 및 특징

### 1.1 정의
이중 연결 리스트(Doubly Linked List)는 각 노드가 데이터와 함께 이전 노드와 다음 노드에 대한 포인터를 가지는 연결 리스트입니다.

### 1.2 특징
- **양방향 순회**: 앞에서 뒤로, 뒤에서 앞으로 자유로운 탐색
- **효율적인 삽입/삭제**: 임의의 위치에서 O(1) 시간 복잡도
- **메모리 오버헤드**: 추가 포인터로 인한 메모리 사용량 증가
- **복잡한 구현**: 단순 연결 리스트보다 복잡한 포인터 관리

### 1.3 Single vs Doubly Linked List 비교

| 특징 | Single Linked List | Doubly Linked List |
|------|-------------------|-------------------|
| 메모리 사용량 | 적음 | 많음 (포인터 추가) |
| 순회 방향 | 한 방향만 | 양방향 |
| 삭제 연산 | O(n) | O(1) |
| 구현 복잡도 | 낮음 | 높음 |
| 캐시 지역성 | 좋음 | 보통 |

## 2. 핵심 자료구조 정의

### 2.1 노드 구조체
```c
/**
 * @brief 이중 연결 리스트의 노드 구조체
 */
typedef struct Node {
    void *data;             // 제네릭 데이터 포인터
    struct Node *prev;      // 이전 노드 포인터
    struct Node *next;      // 다음 노드 포인터
} Node;
```

### 2.2 리스트 구조체
```c
/**
 * @brief 이중 연결 리스트 구조체
 */
typedef struct DoublyLinkedList {
    Node *head;                                     // 첫 번째 노드 포인터
    Node *tail;                                     // 마지막 노드 포인터
    int size;                                       // 현재 리스트 크기
    size_t data_size;                              // 데이터 크기 (바이트)

    // 함수 포인터들
    int (*compare)(const void *a, const void *b);   // 비교 함수
    void (*print)(const void *data);               // 출력 함수
    void (*free_data)(void *data);                 // 데이터 해제 함수
    void* (*copy_data)(const void *data);          // 데이터 복사 함수
} DoublyLinkedList;
```

### 2.3 반복자 구조체
```c
/**
 * @brief 리스트 순회를 위한 반복자
 */
typedef struct Iterator {
    Node *current;          // 현재 노드 포인터
    DoublyLinkedList *list; // 연결된 리스트 포인터
    int direction;          // 순회 방향 (1: 정방향, -1: 역방향)
} Iterator;
```

## 3. 기본 연산 설계

### 3.1 생성 및 초기화
```c
/**
 * @brief 새로운 이중 연결 리스트 생성
 * @param data_size 저장할 데이터의 크기
 * @param compare 비교 함수 포인터
 * @param print 출력 함수 포인터
 * @return 생성된 리스트 포인터 (실패시 NULL)
 */
DoublyLinkedList* dll_create(
    size_t data_size,
    int (*compare)(const void *a, const void *b),
    void (*print)(const void *data)
);

/**
 * @brief 새로운 노드 생성
 * @param data 저장할 데이터
 * @param data_size 데이터 크기
 * @return 생성된 노드 포인터 (실패시 NULL)
 */
Node* node_create(const void *data, size_t data_size);
```

### 3.2 삽입 연산
```c
/**
 * @brief 리스트 앞쪽에 데이터 삽입
 * @param list 대상 리스트
 * @param data 삽입할 데이터
 * @return 성공시 LMS_SUCCESS, 실패시 에러 코드
 */
LMS_Result dll_insert_front(DoublyLinkedList *list, const void *data);

/**
 * @brief 리스트 뒤쪽에 데이터 삽입
 * @param list 대상 리스트
 * @param data 삽입할 데이터
 * @return 성공시 LMS_SUCCESS, 실패시 에러 코드
 */
LMS_Result dll_insert_rear(DoublyLinkedList *list, const void *data);

/**
 * @brief 지정된 위치에 데이터 삽입
 * @param list 대상 리스트
 * @param index 삽입할 위치 (0부터 시작)
 * @param data 삽입할 데이터
 * @return 성공시 LMS_SUCCESS, 실패시 에러 코드
 */
LMS_Result dll_insert_at(DoublyLinkedList *list, int index, const void *data);

/**
 * @brief 정렬된 위치에 데이터 삽입
 * @param list 대상 리스트 (정렬된 상태여야 함)
 * @param data 삽입할 데이터
 * @return 성공시 LMS_SUCCESS, 실패시 에러 코드
 */
LMS_Result dll_insert_sorted(DoublyLinkedList *list, const void *data);
```

### 3.3 삭제 연산
```c
/**
 * @brief 리스트 앞쪽 데이터 삭제
 * @param list 대상 리스트
 * @return 성공시 LMS_SUCCESS, 실패시 에러 코드
 */
LMS_Result dll_delete_front(DoublyLinkedList *list);

/**
 * @brief 리스트 뒤쪽 데이터 삭제
 * @param list 대상 리스트
 * @return 성공시 LMS_SUCCESS, 실패시 에러 코드
 */
LMS_Result dll_delete_rear(DoublyLinkedList *list);

/**
 * @brief 지정된 위치의 데이터 삭제
 * @param list 대상 리스트
 * @param index 삭제할 위치 (0부터 시작)
 * @return 성공시 LMS_SUCCESS, 실패시 에러 코드
 */
LMS_Result dll_delete_at(DoublyLinkedList *list, int index);

/**
 * @brief 특정 데이터를 찾아서 삭제
 * @param list 대상 리스트
 * @param data 삭제할 데이터
 * @return 성공시 LMS_SUCCESS, 실패시 에러 코드
 */
LMS_Result dll_delete_data(DoublyLinkedList *list, const void *data);

/**
 * @brief 조건에 맞는 모든 데이터 삭제
 * @param list 대상 리스트
 * @param condition 조건 함수
 * @param context 조건 함수에 전달할 컨텍스트
 * @return 삭제된 항목 수
 */
int dll_delete_all_if(DoublyLinkedList *list,
                      bool (*condition)(const void *data, void *context),
                      void *context);
```

### 3.4 검색 연산
```c
/**
 * @brief 데이터 검색
 * @param list 대상 리스트
 * @param data 찾을 데이터
 * @return 찾은 노드 포인터 (못찾으면 NULL)
 */
Node* dll_search(DoublyLinkedList *list, const void *data);

/**
 * @brief 조건에 맞는 첫 번째 데이터 검색
 * @param list 대상 리스트
 * @param condition 조건 함수
 * @param context 조건 함수에 전달할 컨텍스트
 * @return 찾은 노드 포인터 (못찾으면 NULL)
 */
Node* dll_find_if(DoublyLinkedList *list,
                  bool (*condition)(const void *data, void *context),
                  void *context);

/**
 * @brief 지정된 위치의 데이터 가져오기
 * @param list 대상 리스트
 * @param index 위치 (0부터 시작)
 * @return 데이터 포인터 (실패시 NULL)
 */
void* dll_get_at(DoublyLinkedList *list, int index);

/**
 * @brief 데이터의 인덱스 찾기
 * @param list 대상 리스트
 * @param data 찾을 데이터
 * @return 인덱스 (못찾으면 -1)
 */
int dll_index_of(DoublyLinkedList *list, const void *data);
```

### 3.5 정렬 연산
```c
/**
 * @brief 병합 정렬을 사용한 리스트 정렬
 * @param list 대상 리스트
 * @return 성공시 LMS_SUCCESS, 실패시 에러 코드
 */
LMS_Result dll_sort(DoublyLinkedList *list);

/**
 * @brief 사용자 정의 비교 함수로 정렬
 * @param list 대상 리스트
 * @param compare 비교 함수
 * @return 성공시 LMS_SUCCESS, 실패시 에러 코드
 */
LMS_Result dll_sort_with(DoublyLinkedList *list,
                         int (*compare)(const void *a, const void *b));

/**
 * @brief 리스트가 정렬되어 있는지 확인
 * @param list 대상 리스트
 * @return 정렬되어 있으면 true, 아니면 false
 */
bool dll_is_sorted(DoublyLinkedList *list);
```

## 4. 고급 연산 설계

### 4.1 반복자 패턴
```c
/**
 * @brief 정방향 반복자 생성
 * @param list 대상 리스트
 * @return 반복자 포인터
 */
Iterator* dll_iterator_create(DoublyLinkedList *list);

/**
 * @brief 역방향 반복자 생성
 * @param list 대상 리스트
 * @return 반복자 포인터
 */
Iterator* dll_reverse_iterator_create(DoublyLinkedList *list);

/**
 * @brief 다음 요소로 이동
 * @param iter 반복자
 * @return 다음 데이터 포인터 (끝에 도달하면 NULL)
 */
void* iterator_next(Iterator *iter);

/**
 * @brief 반복자가 유효한지 확인
 * @param iter 반복자
 * @return 유효하면 true, 아니면 false
 */
bool iterator_has_next(Iterator *iter);

/**
 * @brief 반복자 해제
 * @param iter 반복자
 */
void iterator_destroy(Iterator *iter);
```

### 4.2 함수형 연산
```c
/**
 * @brief 모든 요소에 함수 적용
 * @param list 대상 리스트
 * @param func 적용할 함수
 * @param context 함수에 전달할 컨텍스트
 */
void dll_for_each(DoublyLinkedList *list,
                  void (*func)(void *data, void *context),
                  void *context);

/**
 * @brief 조건에 맞는 요소들로 새 리스트 생성
 * @param list 원본 리스트
 * @param condition 필터 조건
 * @param context 조건 함수에 전달할 컨텍스트
 * @return 필터링된 새 리스트
 */
DoublyLinkedList* dll_filter(DoublyLinkedList *list,
                            bool (*condition)(const void *data, void *context),
                            void *context);

/**
 * @brief 모든 요소를 변환하여 새 리스트 생성
 * @param list 원본 리스트
 * @param transform 변환 함수
 * @param context 변환 함수에 전달할 컨텍스트
 * @param new_data_size 변환된 데이터 크기
 * @return 변환된 새 리스트
 */
DoublyLinkedList* dll_map(DoublyLinkedList *list,
                         void* (*transform)(const void *data, void *context),
                         void *context,
                         size_t new_data_size);

/**
 * @brief 리스트 요소들을 하나의 값으로 축약
 * @param list 대상 리스트
 * @param accumulator 축약 함수
 * @param initial_value 초기값
 * @param context 축약 함수에 전달할 컨텍스트
 * @return 축약된 결과
 */
void* dll_reduce(DoublyLinkedList *list,
                void* (*accumulator)(void *acc, const void *data, void *context),
                void *initial_value,
                void *context);
```

### 4.3 리스트 조작
```c
/**
 * @brief 두 리스트 병합
 * @param list1 첫 번째 리스트
 * @param list2 두 번째 리스트
 * @return 병합된 새 리스트
 */
DoublyLinkedList* dll_merge(DoublyLinkedList *list1, DoublyLinkedList *list2);

/**
 * @brief 리스트 복사
 * @param list 복사할 리스트
 * @return 복사된 새 리스트
 */
DoublyLinkedList* dll_clone(DoublyLinkedList *list);

/**
 * @brief 리스트 뒤집기
 * @param list 대상 리스트
 * @return 성공시 LMS_SUCCESS, 실패시 에러 코드
 */
LMS_Result dll_reverse(DoublyLinkedList *list);

/**
 * @brief 리스트 분할
 * @param list 분할할 리스트
 * @param index 분할 지점
 * @return 분할된 두 번째 리스트 (첫 번째는 원본)
 */
DoublyLinkedList* dll_split(DoublyLinkedList *list, int index);

/**
 * @brief 부분 리스트 추출
 * @param list 원본 리스트
 * @param start 시작 인덱스
 * @param length 길이
 * @return 추출된 새 리스트
 */
DoublyLinkedList* dll_sublist(DoublyLinkedList *list, int start, int length);
```

## 5. 메모리 관리

### 5.1 메모리 해제
```c
/**
 * @brief 노드 메모리 해제
 * @param node 해제할 노드
 * @param free_data 데이터 해제 함수 (NULL이면 기본 free 사용)
 */
void node_destroy(Node *node, void (*free_data)(void *data));

/**
 * @brief 리스트 전체 메모리 해제
 * @param list 해제할 리스트
 */
void dll_destroy(DoublyLinkedList *list);

/**
 * @brief 리스트 내용만 비우기 (구조체는 유지)
 * @param list 대상 리스트
 */
void dll_clear(DoublyLinkedList *list);
```

### 5.2 메모리 풀 관리
```c
/**
 * @brief 노드용 메모리 풀 초기화
 * @param pool_size 풀 크기
 * @return 성공시 LMS_SUCCESS, 실패시 에러 코드
 */
LMS_Result node_pool_init(size_t pool_size);

/**
 * @brief 메모리 풀에서 노드 할당
 * @return 할당된 노드 포인터 (실패시 NULL)
 */
Node* node_pool_alloc(void);

/**
 * @brief 메모리 풀에 노드 반환
 * @param node 반환할 노드
 */
void node_pool_free(Node *node);

/**
 * @brief 메모리 풀 정리
 */
void node_pool_cleanup(void);
```

## 6. 성능 분석

### 6.1 시간 복잡도

| 연산 | 평균 | 최악 | 설명 |
|------|------|------|------|
| 삽입 (앞/뒤) | O(1) | O(1) | 포인터 조작만 필요 |
| 삽입 (중간) | O(n) | O(n) | 위치 찾기 필요 |
| 삭제 (앞/뒤) | O(1) | O(1) | 포인터 조작만 필요 |
| 삭제 (중간) | O(1) | O(1) | 노드 포인터를 알고 있을 때 |
| 검색 | O(n) | O(n) | 순차 탐색 |
| 정렬 | O(n log n) | O(n log n) | 병합 정렬 사용 |

### 6.2 공간 복잡도
- **노드당 메모리**: `sizeof(Node) + data_size`
- **추가 포인터**: 노드당 16바이트 (64비트 시스템)
- **리스트 구조체**: 약 64바이트
- **총 메모리**: `n * (sizeof(Node) + data_size) + sizeof(DoublyLinkedList)`

## 7. 사용 예제

### 7.1 기본 사용법
```c
// 정수 리스트 생성
DoublyLinkedList *int_list = dll_create(sizeof(int), compare_int, print_int);

// 데이터 삽입
int data1 = 10, data2 = 20, data3 = 15;
dll_insert_rear(int_list, &data1);
dll_insert_rear(int_list, &data2);
dll_insert_sorted(int_list, &data3);  // 정렬된 위치에 삽입

// 데이터 검색
Node *found = dll_search(int_list, &data2);
if (found) {
    printf("찾음: %d\n", *(int*)found->data);
}

// 리스트 정렬
dll_sort(int_list);

// 순회
Iterator *iter = dll_iterator_create(int_list);
while (iterator_has_next(iter)) {
    int *value = (int*)iterator_next(iter);
    printf("%d ", *value);
}
iterator_destroy(iter);

// 메모리 해제
dll_destroy(int_list);
```

### 7.2 도서 관리 예제
```c
// 도서 리스트 생성
DoublyLinkedList *book_list = dll_create(sizeof(Book), compare_book_isbn, print_book);

// 도서 추가
Book book1 = {"9788956746425", "Clean Code", "Robert C. Martin", ...};
dll_insert_sorted(book_list, &book1);

// ISBN으로 검색
Book search_book = {"9788956746425", "", "", ...};
Node *found = dll_search(book_list, &search_book);

// 조건부 검색 (저자명으로 검색)
typedef struct {
    char author[51];
} AuthorContext;

bool is_author_match(const void *data, void *context) {
    Book *book = (Book*)data;
    AuthorContext *ctx = (AuthorContext*)context;
    return strcmp(book->author, ctx->author) == 0;
}

AuthorContext ctx = {"Robert C. Martin"};
Node *author_book = dll_find_if(book_list, is_author_match, &ctx);
```

## 8. 디버깅 및 검증

### 8.1 무결성 검증
```c
/**
 * @brief 리스트 무결성 검증
 * @param list 검증할 리스트
 * @return 무결성이 유지되면 true, 아니면 false
 */
bool dll_validate_integrity(DoublyLinkedList *list);

/**
 * @brief 리스트 통계 정보 출력
 * @param list 대상 리스트
 */
void dll_print_stats(DoublyLinkedList *list);

/**
 * @brief 메모리 사용량 정보 출력
 * @param list 대상 리스트
 */
void dll_print_memory_usage(DoublyLinkedList *list);
```

### 8.2 테스트 유틸리티
```c
/**
 * @brief 랜덤 데이터로 리스트 채우기
 * @param list 대상 리스트
 * @param count 생성할 데이터 개수
 * @param generator 데이터 생성 함수
 */
void dll_fill_random(DoublyLinkedList *list, int count,
                     void* (*generator)(void));

/**
 * @brief 리스트 성능 테스트
 * @param list 대상 리스트
 * @param operation 테스트할 연산
 * @param iterations 반복 횟수
 * @return 평균 실행 시간 (마이크로초)
 */
double dll_benchmark(DoublyLinkedList *list,
                     void (*operation)(DoublyLinkedList*),
                     int iterations);
```