# 도서관 관리 시스템 아키텍처 설계

## 1. 시스템 개요

### 1.1 설계 목표
- **효율성**: Doubly Linked List를 활용한 빠른 데이터 접근 및 조작
- **확장성**: 모듈화된 구조를 통한 기능 확장 용이성
- **유지보수성**: 명확한 계층 구조와 책임 분리
- **메모리 효율성**: 동적 메모리 관리를 통한 최적화

### 1.2 핵심 자료구조 선택 이유
Doubly Linked List를 선택한 이유:
- **양방향 순회**: 앞뒤로 자유로운 데이터 탐색
- **효율적인 삽입/삭제**: O(1) 시간 복잡도
- **메모리 효율성**: 필요한 만큼만 동적 할당
- **구현 유연성**: 다양한 정렬 및 검색 알고리즘 적용 가능

## 2. 시스템 아키텍처

### 2.1 계층형 아키텍처

```
┌─────────────────────────────────────┐
│           사용자 인터페이스 계층        │
│        (User Interface Layer)      │
├─────────────────────────────────────┤
│          비즈니스 로직 계층            │
│       (Business Logic Layer)       │
├─────────────────────────────────────┤
│          데이터 접근 계층              │
│       (Data Access Layer)          │
├─────────────────────────────────────┤
│          자료구조 계층                │
│      (Data Structure Layer)        │
└─────────────────────────────────────┘
```

### 2.2 모듈 구성도

```
Library Management System
├── UI Module (인터페이스 모듈)
│   ├── Menu System (메뉴 시스템)
│   ├── Input Handler (입력 처리)
│   └── Output Formatter (출력 포맷)
│
├── Service Module (서비스 모듈)
│   ├── Book Service (도서 서비스)
│   ├── Member Service (회원 서비스)
│   └── Loan Service (대출 서비스)
│
├── Repository Module (저장소 모듈)
│   ├── Book Repository (도서 저장소)
│   ├── Member Repository (회원 저장소)
│   └── Loan Repository (대출 저장소)
│
└── Core Module (핵심 모듈)
    ├── Doubly Linked List (이중 연결 리스트)
    ├── Memory Manager (메모리 관리자)
    └── Utilities (유틸리티)
```

## 3. 데이터 모델 설계

### 3.1 도서 (Book) 모델
```c
typedef struct Book {
    char isbn[14];          // ISBN (13자리 + null)
    char title[101];        // 제목 (100자 + null)
    char author[51];        // 저자 (50자 + null)
    char publisher[51];     // 출판사 (50자 + null)
    int publication_year;   // 출판년도
    char category[31];      // 카테고리 (30자 + null)
    int total_copies;       // 총 보유 수량
    int available_copies;   // 대출 가능 수량
    double price;           // 가격
    char status;            // 상태 ('A': 사용가능, 'D': 삭제)
} Book;
```

### 3.2 회원 (Member) 모델
```c
typedef struct Member {
    char member_id[11];     // 회원 ID (10자 + null)
    char name[51];          // 이름 (50자 + null)
    char phone[16];         // 전화번호 (15자 + null)
    char email[101];        // 이메일 (100자 + null)
    char address[201];      // 주소 (200자 + null)
    char join_date[11];     // 가입일 (YYYY-MM-DD)
    char membership_type;   // 회원 유형 ('R': 일반, 'P': 프리미엄)
    int loan_count;         // 현재 대출 수량
    char status;            // 상태 ('A': 활성, 'S': 정지, 'D': 삭제)
} Member;
```

### 3.3 대출 (Loan) 모델
```c
typedef struct Loan {
    char loan_id[11];       // 대출 ID (10자 + null)
    char member_id[11];     // 회원 ID
    char isbn[14];          // 도서 ISBN
    char loan_date[11];     // 대출일 (YYYY-MM-DD)
    char due_date[11];      // 반납 예정일 (YYYY-MM-DD)
    char return_date[11];   // 실제 반납일 (YYYY-MM-DD, 미반납시 empty)
    int overdue_days;       // 연체 일수
    double fine_amount;     // 연체료
    char status;            // 상태 ('L': 대출중, 'R': 반납, 'O': 연체)
} Loan;
```

## 4. Doubly Linked List 아키텍처

### 4.1 노드 구조
```c
typedef struct Node {
    void *data;             // 데이터 포인터 (Generic)
    struct Node *prev;      // 이전 노드 포인터
    struct Node *next;      // 다음 노드 포인터
} Node;
```

### 4.2 리스트 구조
```c
typedef struct DoublyLinkedList {
    Node *head;             // 헤드 노드 포인터
    Node *tail;             // 테일 노드 포인터
    int size;               // 리스트 크기
    size_t data_size;       // 데이터 크기
    int (*compare)(const void *a, const void *b);  // 비교 함수 포인터
    void (*print)(const void *data);               // 출력 함수 포인터
} DoublyLinkedList;
```

### 4.3 리스트 연산
- **생성/소멸**: create_list(), destroy_list()
- **삽입**: insert_front(), insert_rear(), insert_at()
- **삭제**: delete_front(), delete_rear(), delete_at(), delete_data()
- **검색**: search_data(), find_by_condition()
- **순회**: traverse_forward(), traverse_backward()
- **정렬**: sort_list() (병합 정렬 알고리즘 적용)

## 5. 시스템 플로우

### 5.1 도서 관리 플로우
```
도서 추가 요청 → 입력 검증 → 중복 확인 → 리스트 삽입 → 결과 출력
도서 검색 요청 → 검색 조건 입력 → 리스트 순회 → 결과 필터링 → 결과 출력
도서 수정 요청 → 도서 선택 → 수정 정보 입력 → 노드 업데이트 → 결과 출력
도서 삭제 요청 → 도서 선택 → 삭제 확인 → 노드 삭제 → 결과 출력
```

### 5.2 대출/반납 플로우
```
대출 요청 → 회원/도서 확인 → 대출 가능 여부 검증 → 대출 기록 생성 → 도서 수량 업데이트
반납 요청 → 대출 기록 검색 → 연체 확인 → 반납 처리 → 도서 수량 업데이트
```

## 6. 메모리 관리 전략

### 6.1 동적 메모리 할당
- **노드 생성**: malloc()을 통한 동적 할당
- **데이터 복사**: memcpy()를 통한 안전한 데이터 복사
- **메모리 해제**: free()를 통한 메모리 누수 방지

### 6.2 메모리 풀 관리
```c
typedef struct MemoryPool {
    void *pool;             // 메모리 풀 시작 주소
    size_t block_size;      // 블록 크기
    size_t total_blocks;    // 총 블록 수
    size_t free_blocks;     // 사용 가능 블록 수
    void *free_list;        // 사용 가능 블록 리스트
} MemoryPool;
```

## 7. 에러 처리 및 로깅

### 7.1 에러 코드 정의
```c
typedef enum {
    LMS_SUCCESS = 0,        // 성공
    LMS_ERROR_NULL_POINTER, // 널 포인터 에러
    LMS_ERROR_MEMORY,       // 메모리 할당 에러
    LMS_ERROR_NOT_FOUND,    // 데이터 찾을 수 없음
    LMS_ERROR_DUPLICATE,    // 중복 데이터
    LMS_ERROR_INVALID_INPUT,// 잘못된 입력
    LMS_ERROR_LOAN_LIMIT,   // 대출 한도 초과
    LMS_ERROR_BOOK_UNAVAILABLE // 도서 대출 불가
} LMS_Result;
```

### 7.2 로깅 시스템
```c
typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
} LogLevel;

void log_message(LogLevel level, const char *format, ...);
```

## 8. 성능 최적화 전략

### 8.1 검색 성능 향상
- **인덱싱**: 자주 검색되는 필드에 대한 별도 인덱스 리스트 구성
- **캐싱**: 최근 검색된 결과를 캐시하여 반복 검색 성능 향상
- **정렬된 삽입**: 특정 조건에 따라 정렬된 상태로 삽입하여 검색 성능 향상

### 8.2 메모리 최적화
- **메모리 풀**: 빈번한 할당/해제를 위한 메모리 풀 사용
- **지연 삭제**: 즉시 삭제 대신 마킹 후 배치 삭제
- **압축**: 문자열 데이터 압축을 통한 메모리 사용량 감소

## 9. 확장성 고려사항

### 9.1 기능 확장
- **파일 I/O**: 데이터 영속성을 위한 파일 저장/로드 기능
- **데이터베이스 연동**: SQLite 등 경량 DB 연동
- **네트워킹**: 클라이언트-서버 모델로 확장
- **GUI**: 그래픽 사용자 인터페이스 추가

### 9.2 플랫폼 확장
- **크로스 플랫폼**: Windows, Linux, macOS 지원
- **모바일**: 모바일 플랫폼 포팅
- **웹**: WebAssembly를 통한 웹 브라우저 지원

## 10. 테스트 전략

### 10.1 단위 테스트
- 각 함수별 단위 테스트 작성
- 경계값 테스트 및 예외 상황 테스트
- 메모리 누수 테스트

### 10.2 통합 테스트
- 모듈 간 상호작용 테스트
- 전체 시스템 시나리오 테스트
- 성능 테스트 및 부하 테스트