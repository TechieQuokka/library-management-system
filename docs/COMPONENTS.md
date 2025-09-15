# 시스템 컴포넌트 및 모듈 설계

## 1. 모듈 계층 구조

```
┌─────────────────────────────────────────────────────────────┐
│                    Application Layer                        │
│                     (main.c)                               │
├─────────────────────────────────────────────────────────────┤
│                 User Interface Layer                        │
│           ┌─────────────┬─────────────┬─────────────┐       │
│           │    Menu     │   Input     │   Output    │       │
│           │   System    │  Handler    │  Formatter  │       │
│           └─────────────┴─────────────┴─────────────┘       │
├─────────────────────────────────────────────────────────────┤
│                Business Logic Layer                         │
│      ┌─────────────┬─────────────┬─────────────┬─────────┐  │
│      │    Book     │   Member    │    Loan     │  Report │  │
│      │   Service   │   Service   │   Service   │ Service │  │
│      └─────────────┴─────────────┴─────────────┴─────────┘  │
├─────────────────────────────────────────────────────────────┤
│                Data Access Layer                            │
│      ┌─────────────┬─────────────┬─────────────┬─────────┐  │
│      │    Book     │   Member    │    Loan     │  Data   │  │
│      │ Repository  │ Repository  │ Repository  │ Manager │  │
│      └─────────────┴─────────────┴─────────────┴─────────┘  │
├─────────────────────────────────────────────────────────────┤
│                 Core Infrastructure                         │
│      ┌─────────────┬─────────────┬─────────────┬─────────┐  │
│      │   Doubly    │   Memory    │   Error     │  Utils  │  │
│      │ Linked List │   Manager   │  Handler    │         │  │
│      └─────────────┴─────────────┴─────────────┴─────────┘  │
└─────────────────────────────────────────────────────────────┘
```

## 2. 각 모듈별 상세 설계

### 2.1 Core Infrastructure 모듈

#### 2.1.1 Doubly Linked List Core (`src/core/`)

**파일**: `doubly_linked_list.h`, `doubly_linked_list.c`

```c
// 핵심 자료구조 및 기본 연산 제공
typedef struct {
    // 기본 CRUD 연산
    DoublyLinkedList* (*create)(size_t data_size, CompareFunc cmp, PrintFunc print);
    LMS_Result (*insert_front)(DoublyLinkedList *list, const void *data);
    LMS_Result (*insert_rear)(DoublyLinkedList *list, const void *data);
    LMS_Result (*insert_at)(DoublyLinkedList *list, int index, const void *data);
    LMS_Result (*delete_front)(DoublyLinkedList *list);
    LMS_Result (*delete_rear)(DoublyLinkedList *list);
    LMS_Result (*delete_at)(DoublyLinkedList *list, int index);

    // 검색 및 순회
    Node* (*search)(DoublyLinkedList *list, const void *data);
    Node* (*find_if)(DoublyLinkedList *list, ConditionFunc condition, void *context);
    Iterator* (*get_iterator)(DoublyLinkedList *list);

    // 정렬 및 조작
    LMS_Result (*sort)(DoublyLinkedList *list);
    LMS_Result (*reverse)(DoublyLinkedList *list);
    DoublyLinkedList* (*clone)(DoublyLinkedList *list);

    // 메모리 관리
    void (*clear)(DoublyLinkedList *list);
    void (*destroy)(DoublyLinkedList *list);
} DLL_Operations;
```

#### 2.1.2 Memory Manager (`src/core/memory_manager.h`, `src/core/memory_manager.c`)

```c
// 메모리 풀 관리 및 동적 할당 최적화
typedef struct MemoryManager {
    MemoryPool *node_pool;          // 노드용 메모리 풀
    MemoryPool *data_pool;          // 데이터용 메모리 풀
    size_t total_allocated;         // 총 할당된 메모리
    size_t total_freed;             // 총 해제된 메모리
    size_t peak_usage;              // 최대 사용량

    // 메모리 관리 함수들
    void* (*alloc)(size_t size);
    void* (*realloc)(void *ptr, size_t new_size);
    void (*free)(void *ptr);
    void (*pool_init)(size_t pool_size);
    void (*pool_cleanup)(void);
    void (*print_stats)(void);
} MemoryManager;
```

#### 2.1.3 Error Handler (`src/core/error_handler.h`, `src/core/error_handler.c`)

```c
// 통합 에러 처리 및 로깅 시스템
typedef struct ErrorHandler {
    LogLevel current_level;
    FILE *log_file;
    bool console_output;

    // 에러 처리 함수들
    void (*log_error)(LMS_Result error, const char *function, int line);
    void (*log_warning)(const char *message, ...);
    void (*log_info)(const char *message, ...);
    void (*log_debug)(const char *message, ...);
    const char* (*get_error_string)(LMS_Result error);
    void (*set_log_level)(LogLevel level);
    void (*set_log_file)(const char *filename);
} ErrorHandler;
```

#### 2.1.4 Utilities (`src/utils/`)

**파일들**: `string_utils.h`, `date_utils.h`, `validation.h`, `common.h`

```c
// 문자열 유틸리티
typedef struct StringUtils {
    char* (*trim)(char *str);
    char* (*to_upper)(char *str);
    char* (*to_lower)(char *str);
    bool (*is_valid_isbn)(const char *isbn);
    bool (*is_valid_email)(const char *email);
    bool (*is_valid_phone)(const char *phone);
    char* (*generate_id)(const char *prefix);
    int (*safe_strcmp)(const char *s1, const char *s2);
} StringUtils;

// 날짜 유틸리티
typedef struct DateUtils {
    char* (*get_current_date)(void);
    char* (*add_days)(const char *date, int days);
    int (*days_between)(const char *date1, const char *date2);
    bool (*is_valid_date)(const char *date);
    bool (*is_date_past)(const char *date);
} DateUtils;

// 입력 검증
typedef struct Validation {
    bool (*validate_book_data)(const Book *book);
    bool (*validate_member_data)(const Member *member);
    bool (*validate_loan_data)(const Loan *loan);
    ValidationResult (*validate_isbn)(const char *isbn);
    ValidationResult (*validate_member_id)(const char *member_id);
} Validation;
```

### 2.2 Data Access Layer 모듈

#### 2.2.1 Book Repository (`src/repositories/book_repository.h`, `src/repositories/book_repository.c`)

```c
// 도서 데이터 관리 전담
typedef struct BookRepository {
    DoublyLinkedList *book_list;    // 메인 도서 리스트
    DoublyLinkedList *isbn_index;   // ISBN 인덱스
    DoublyLinkedList *title_index;  // 제목 인덱스
    DoublyLinkedList *author_index; // 저자 인덱스

    // CRUD 연산
    LMS_Result (*add_book)(BookRepository *repo, const Book *book);
    LMS_Result (*update_book)(BookRepository *repo, const char *isbn, const Book *updated_book);
    LMS_Result (*delete_book)(BookRepository *repo, const char *isbn);
    Book* (*get_book_by_isbn)(BookRepository *repo, const char *isbn);

    // 검색 연산
    DoublyLinkedList* (*search_by_title)(BookRepository *repo, const char *title);
    DoublyLinkedList* (*search_by_author)(BookRepository *repo, const char *author);
    DoublyLinkedList* (*search_by_category)(BookRepository *repo, const char *category);
    DoublyLinkedList* (*search_by_keyword)(BookRepository *repo, const char *keyword);

    // 고급 검색
    DoublyLinkedList* (*search_advanced)(BookRepository *repo, const BookSearchCriteria *criteria);
    DoublyLinkedList* (*get_available_books)(BookRepository *repo);
    DoublyLinkedList* (*get_popular_books)(BookRepository *repo, int limit);

    // 통계 및 관리
    int (*get_total_books)(BookRepository *repo);
    int (*get_available_count)(BookRepository *repo);
    void (*update_availability)(BookRepository *repo, const char *isbn, int change);

    // 메모리 관리
    void (*clear)(BookRepository *repo);
    void (*destroy)(BookRepository *repo);
} BookRepository;
```

#### 2.2.2 Member Repository (`src/repositories/member_repository.h`, `src/repositories/member_repository.c`)

```c
// 회원 데이터 관리 전담
typedef struct MemberRepository {
    DoublyLinkedList *member_list;      // 메인 회원 리스트
    DoublyLinkedList *id_index;         // 회원 ID 인덱스
    DoublyLinkedList *email_index;      // 이메일 인덱스
    DoublyLinkedList *phone_index;      // 전화번호 인덱스

    // CRUD 연산
    LMS_Result (*add_member)(MemberRepository *repo, const Member *member);
    LMS_Result (*update_member)(MemberRepository *repo, const char *member_id, const Member *updated_member);
    LMS_Result (*delete_member)(MemberRepository *repo, const char *member_id);
    Member* (*get_member_by_id)(MemberRepository *repo, const char *member_id);

    // 검색 연산
    Member* (*get_member_by_email)(MemberRepository *repo, const char *email);
    Member* (*get_member_by_phone)(MemberRepository *repo, const char *phone);
    DoublyLinkedList* (*search_by_name)(MemberRepository *repo, const char *name);

    // 회원 관리
    LMS_Result (*suspend_member)(MemberRepository *repo, const char *member_id);
    LMS_Result (*activate_member)(MemberRepository *repo, const char *member_id);
    DoublyLinkedList* (*get_active_members)(MemberRepository *repo);
    DoublyLinkedList* (*get_suspended_members)(MemberRepository *repo);

    // 대출 관련
    LMS_Result (*update_loan_count)(MemberRepository *repo, const char *member_id, int change);
    bool (*can_borrow)(MemberRepository *repo, const char *member_id);

    // 통계
    int (*get_total_members)(MemberRepository *repo);
    int (*get_active_count)(MemberRepository *repo);

    // 메모리 관리
    void (*clear)(MemberRepository *repo);
    void (*destroy)(MemberRepository *repo);
} MemberRepository;
```

#### 2.2.3 Loan Repository (`src/repositories/loan_repository.h`, `src/repositories/loan_repository.c`)

```c
// 대출 데이터 관리 전담
typedef struct LoanRepository {
    DoublyLinkedList *loan_list;        // 메인 대출 리스트
    DoublyLinkedList *member_index;     // 회원별 인덱스
    DoublyLinkedList *book_index;       // 도서별 인덱스
    DoublyLinkedList *date_index;       // 날짜별 인덱스

    // CRUD 연산
    LMS_Result (*add_loan)(LoanRepository *repo, const Loan *loan);
    LMS_Result (*update_loan)(LoanRepository *repo, const char *loan_id, const Loan *updated_loan);
    LMS_Result (*delete_loan)(LoanRepository *repo, const char *loan_id);
    Loan* (*get_loan_by_id)(LoanRepository *repo, const char *loan_id);

    // 검색 연산
    DoublyLinkedList* (*get_loans_by_member)(LoanRepository *repo, const char *member_id);
    DoublyLinkedList* (*get_loans_by_book)(LoanRepository *repo, const char *isbn);
    DoublyLinkedList* (*get_active_loans)(LoanRepository *repo);
    DoublyLinkedList* (*get_overdue_loans)(LoanRepository *repo);

    // 대출 처리
    LMS_Result (*process_loan)(LoanRepository *repo, const char *member_id, const char *isbn);
    LMS_Result (*process_return)(LoanRepository *repo, const char *loan_id);
    LMS_Result (*calculate_fine)(LoanRepository *repo, const char *loan_id, double *fine);

    // 통계 및 보고서
    int (*get_total_loans)(LoanRepository *repo);
    int (*get_active_count)(LoanRepository *repo);
    int (*get_overdue_count)(LoanRepository *repo);
    DoublyLinkedList* (*get_loan_history)(LoanRepository *repo, const char *member_id);

    // 메모리 관리
    void (*clear)(LoanRepository *repo);
    void (*destroy)(LoanRepository *repo);
} LoanRepository;
```

### 2.3 Business Logic Layer 모듈

#### 2.3.1 Book Service (`src/services/book_service.h`, `src/services/book_service.c`)

```c
// 도서 관리 비즈니스 로직
typedef struct BookService {
    BookRepository *book_repo;
    LoanRepository *loan_repo;

    // 도서 관리 비즈니스 로직
    LMS_Result (*register_book)(BookService *service, const Book *book);
    LMS_Result (*update_book_info)(BookService *service, const char *isbn, const Book *book);
    LMS_Result (*remove_book)(BookService *service, const char *isbn);

    // 고급 검색 및 추천
    DoublyLinkedList* (*search_books)(BookService *service, const BookSearchCriteria *criteria);
    DoublyLinkedList* (*recommend_books)(BookService *service, const char *member_id);
    DoublyLinkedList* (*get_new_arrivals)(BookService *service, int days);
    DoublyLinkedList* (*get_most_borrowed)(BookService *service, int limit);

    // 재고 관리
    LMS_Result (*adjust_inventory)(BookService *service, const char *isbn, int quantity);
    bool (*is_available_for_loan)(BookService *service, const char *isbn);
    int (*get_available_copies)(BookService *service, const char *isbn);

    // 데이터 검증
    ValidationResult (*validate_book_data)(BookService *service, const Book *book);
    bool (*is_isbn_duplicate)(BookService *service, const char *isbn);

    // 보고서
    BookReport (*generate_inventory_report)(BookService *service);
    BookReport (*generate_category_report)(BookService *service);

    // 메모리 관리
    void (*destroy)(BookService *service);
} BookService;
```

#### 2.3.2 Member Service (`src/services/member_service.h`, `src/services/member_service.c`)

```c
// 회원 관리 비즈니스 로직
typedef struct MemberService {
    MemberRepository *member_repo;
    LoanRepository *loan_repo;

    // 회원 관리
    LMS_Result (*register_member)(MemberService *service, const Member *member);
    LMS_Result (*update_member_info)(MemberService *service, const char *member_id, const Member *member);
    LMS_Result (*deactivate_member)(MemberService *service, const char *member_id);

    // 회원 상태 관리
    LMS_Result (*suspend_member)(MemberService *service, const char *member_id, const char *reason);
    LMS_Result (*reactivate_member)(MemberService *service, const char *member_id);
    MemberStatus (*get_member_status)(MemberService *service, const char *member_id);

    // 대출 권한 검증
    bool (*can_borrow_book)(MemberService *service, const char *member_id);
    int (*get_remaining_loan_limit)(MemberService *service, const char *member_id);
    double (*get_outstanding_fines)(MemberService *service, const char *member_id);

    // 회원 검색 및 관리
    DoublyLinkedList* (*search_members)(MemberService *service, const MemberSearchCriteria *criteria);
    DoublyLinkedList* (*get_active_members)(MemberService *service);
    DoublyLinkedList* (*get_members_with_overdues)(MemberService *service);

    // 데이터 검증
    ValidationResult (*validate_member_data)(MemberService *service, const Member *member);
    bool (*is_email_duplicate)(MemberService *service, const char *email);
    bool (*is_phone_duplicate)(MemberService *service, const char *phone);

    // 보고서
    MemberReport (*generate_membership_report)(MemberService *service);
    MemberReport (*generate_activity_report)(MemberService *service);

    // 메모리 관리
    void (*destroy)(MemberService *service);
} MemberService;
```

#### 2.3.3 Loan Service (`src/services/loan_service.h`, `src/services/loan_service.c`)

```c
// 대출/반납 비즈니스 로직
typedef struct LoanService {
    LoanRepository *loan_repo;
    BookRepository *book_repo;
    MemberRepository *member_repo;

    // 대출 처리
    LMS_Result (*borrow_book)(LoanService *service, const char *member_id, const char *isbn);
    LMS_Result (*return_book)(LoanService *service, const char *loan_id);
    LMS_Result (*renew_loan)(LoanService *service, const char *loan_id);

    // 대출 상태 관리
    LMS_Result (*mark_as_lost)(LoanService *service, const char *loan_id);
    LMS_Result (*process_fine_payment)(LoanService *service, const char *loan_id, double amount);

    // 연체 관리
    DoublyLinkedList* (*get_overdue_loans)(LoanService *service);
    LMS_Result (*calculate_overdue_fines)(LoanService *service);
    LMS_Result (*send_overdue_notices)(LoanService *service);

    // 대출 기록 및 통계
    DoublyLinkedList* (*get_loan_history)(LoanService *service, const char *member_id);
    DoublyLinkedList* (*get_popular_books)(LoanService *service, int limit);
    LoanStatistics (*get_loan_statistics)(LoanService *service);

    // 대출 가능성 검증
    bool (*can_borrow)(LoanService *service, const char *member_id, const char *isbn);
    bool (*can_renew)(LoanService *service, const char *loan_id);
    int (*get_loan_period)(LoanService *service, const char *member_id);

    // 보고서
    LoanReport (*generate_daily_report)(LoanService *service, const char *date);
    LoanReport (*generate_monthly_report)(LoanService *service, int year, int month);
    OverdueReport (*generate_overdue_report)(LoanService *service);

    // 메모리 관리
    void (*destroy)(LoanService *service);
} LoanService;
```

#### 2.3.4 Report Service (`src/services/report_service.h`, `src/services/report_service.c`)

```c
// 통계 및 보고서 생성 서비스
typedef struct ReportService {
    BookRepository *book_repo;
    MemberRepository *member_repo;
    LoanRepository *loan_repo;

    // 종합 보고서
    SystemReport (*generate_system_report)(ReportService *service);
    DailyReport (*generate_daily_report)(ReportService *service, const char *date);
    MonthlyReport (*generate_monthly_report)(ReportService *service, int year, int month);

    // 도서 관련 보고서
    InventoryReport (*generate_inventory_report)(ReportService *service);
    PopularityReport (*generate_popularity_report)(ReportService *service);
    CategoryReport (*generate_category_report)(ReportService *service);

    // 회원 관련 보고서
    MembershipReport (*generate_membership_report)(ReportService *service);
    ActivityReport (*generate_activity_report)(ReportService *service);

    // 대출 관련 보고서
    CirculationReport (*generate_circulation_report)(ReportService *service);
    OverdueReport (*generate_overdue_report)(ReportService *service);
    FineReport (*generate_fine_report)(ReportService *service);

    // 보고서 출력
    void (*print_report)(ReportService *service, const void *report, ReportType type);
    LMS_Result (*export_report)(ReportService *service, const void *report,
                               ReportType type, const char *filename);

    // 메모리 관리
    void (*destroy)(ReportService *service);
} ReportService;
```

### 2.4 User Interface Layer 모듈

#### 2.4.1 Menu System (`src/ui/menu_system.h`, `src/ui/menu_system.c`)

```c
// 메뉴 시스템 관리
typedef struct MenuSystem {
    Menu *current_menu;
    Menu *main_menu;
    MenuStack *menu_stack;

    // 메뉴 관리
    LMS_Result (*init)(MenuSystem *system);
    void (*display_current_menu)(MenuSystem *system);
    LMS_Result (*handle_selection)(MenuSystem *system, int choice);
    void (*push_menu)(MenuSystem *system, Menu *menu);
    Menu* (*pop_menu)(MenuSystem *system);

    // 메뉴 구성
    Menu* (*create_main_menu)(MenuSystem *system);
    Menu* (*create_book_menu)(MenuSystem *system);
    Menu* (*create_member_menu)(MenuSystem *system);
    Menu* (*create_loan_menu)(MenuSystem *system);
    Menu* (*create_report_menu)(MenuSystem *system);

    // 유틸리티
    void (*show_help)(MenuSystem *system);
    void (*show_about)(MenuSystem *system);

    // 메모리 관리
    void (*destroy)(MenuSystem *system);
} MenuSystem;
```

#### 2.4.2 Input Handler (`src/ui/input_handler.h`, `src/ui/input_handler.c`)

```c
// 사용자 입력 처리
typedef struct InputHandler {
    // 기본 입력
    int (*get_integer)(const char *prompt, int min, int max);
    double (*get_double)(const char *prompt, double min, double max);
    char* (*get_string)(const char *prompt, int max_length);
    bool (*get_confirmation)(const char *prompt);

    // 특화된 입력
    char* (*get_isbn)(const char *prompt);
    char* (*get_email)(const char *prompt);
    char* (*get_phone)(const char *prompt);
    char* (*get_date)(const char *prompt);

    // 복합 입력
    Book* (*input_book_data)(InputHandler *handler);
    Member* (*input_member_data)(InputHandler *handler);
    SearchCriteria* (*input_search_criteria)(InputHandler *handler);

    // 입력 검증
    bool (*validate_input)(InputHandler *handler, const char *input, InputType type);
    void (*show_input_error)(InputHandler *handler, const char *message);

    // 유틸리티
    void (*clear_input_buffer)(InputHandler *handler);
    void (*wait_for_enter)(InputHandler *handler);

    // 메모리 관리
    void (*destroy)(InputHandler *handler);
} InputHandler;
```

#### 2.4.3 Output Formatter (`src/ui/output_formatter.h`, `src/ui/output_formatter.c`)

```c
// 출력 포맷팅
typedef struct OutputFormatter {
    // 기본 출력
    void (*print_header)(const char *title);
    void (*print_separator)(void);
    void (*print_message)(const char *message, MessageType type);
    void (*print_error)(const char *error);
    void (*print_success)(const char *message);

    // 데이터 출력
    void (*print_book)(const Book *book);
    void (*print_member)(const Member *member);
    void (*print_loan)(const Loan *loan);
    void (*print_book_list)(const DoublyLinkedList *books);
    void (*print_member_list)(const DoublyLinkedList *members);
    void (*print_loan_list)(const DoublyLinkedList *loans);

    // 테이블 출력
    void (*print_book_table)(const DoublyLinkedList *books);
    void (*print_member_table)(const DoublyLinkedList *members);
    void (*print_loan_table)(const DoublyLinkedList *loans);

    // 보고서 출력
    void (*print_system_report)(const SystemReport *report);
    void (*print_statistics)(const Statistics *stats);

    // 포맷 설정
    void (*set_page_size)(int size);
    void (*set_column_width)(int width);

    // 페이징
    void (*print_with_paging)(const DoublyLinkedList *list,
                             void (*print_item)(const void *item));

    // 메모리 관리
    void (*destroy)(OutputFormatter *formatter);
} OutputFormatter;
```

### 2.5 Application Layer

#### 2.5.1 Main Application (`src/main.c`)

```c
// 메인 애플리케이션 구조
typedef struct LibraryApp {
    // 서비스 계층
    BookService *book_service;
    MemberService *member_service;
    LoanService *loan_service;
    ReportService *report_service;

    // UI 계층
    MenuSystem *menu_system;
    InputHandler *input_handler;
    OutputFormatter *output_formatter;

    // 시스템 상태
    bool is_running;
    UserSession *current_session;
    SystemSettings *settings;

    // 초기화 및 정리
    LMS_Result (*init)(LibraryApp *app);
    void (*run)(LibraryApp *app);
    void (*shutdown)(LibraryApp *app);

    // 세션 관리
    LMS_Result (*login)(LibraryApp *app, const char *username, const char *password);
    void (*logout)(LibraryApp *app);

    // 설정 관리
    LMS_Result (*load_settings)(LibraryApp *app, const char *filename);
    LMS_Result (*save_settings)(LibraryApp *app, const char *filename);
} LibraryApp;
```

## 3. 모듈 간 의존성 관리

### 3.1 의존성 주입 패턴

```c
// 서비스 팩토리
typedef struct ServiceFactory {
    BookService* (*create_book_service)(BookRepository *book_repo, LoanRepository *loan_repo);
    MemberService* (*create_member_service)(MemberRepository *member_repo, LoanRepository *loan_repo);
    LoanService* (*create_loan_service)(LoanRepository *loan_repo, BookRepository *book_repo,
                                       MemberRepository *member_repo);
    ReportService* (*create_report_service)(BookRepository *book_repo, MemberRepository *member_repo,
                                           LoanRepository *loan_repo);
} ServiceFactory;

// 저장소 팩토리
typedef struct RepositoryFactory {
    BookRepository* (*create_book_repository)(void);
    MemberRepository* (*create_member_repository)(void);
    LoanRepository* (*create_loan_repository)(void);
} RepositoryFactory;
```

### 3.2 모듈 간 통신 인터페이스

```c
// 이벤트 시스템
typedef enum {
    EVENT_BOOK_ADDED,
    EVENT_BOOK_UPDATED,
    EVENT_BOOK_DELETED,
    EVENT_MEMBER_REGISTERED,
    EVENT_MEMBER_SUSPENDED,
    EVENT_LOAN_CREATED,
    EVENT_LOAN_RETURNED,
    EVENT_FINE_CALCULATED
} EventType;

typedef struct Event {
    EventType type;
    void *data;
    char timestamp[20];
} Event;

typedef struct EventManager {
    DoublyLinkedList *listeners;

    void (*subscribe)(EventManager *manager, EventType type,
                     void (*handler)(const Event *event));
    void (*publish)(EventManager *manager, const Event *event);
    void (*unsubscribe)(EventManager *manager, EventType type,
                       void (*handler)(const Event *event));
} EventManager;
```

## 4. 확장성 및 유지보수성

### 4.1 플러그인 아키텍처

```c
// 플러그인 인터페이스
typedef struct Plugin {
    char name[50];
    char version[10];
    char description[200];

    LMS_Result (*init)(void);
    void (*cleanup)(void);
    void (*configure)(const char *config);
} Plugin;

typedef struct PluginManager {
    DoublyLinkedList *plugins;

    LMS_Result (*load_plugin)(PluginManager *manager, const char *path);
    LMS_Result (*unload_plugin)(PluginManager *manager, const char *name);
    Plugin* (*get_plugin)(PluginManager *manager, const char *name);
    void (*list_plugins)(PluginManager *manager);
} PluginManager;
```

### 4.2 설정 관리

```c
// 시스템 설정
typedef struct SystemSettings {
    // 대출 설정
    int max_loans_per_member;
    int loan_period_days;
    double fine_per_day;

    // 시스템 설정
    char log_level[10];
    char log_file[256];
    int memory_pool_size;

    // UI 설정
    int page_size;
    bool use_colors;
    char date_format[20];

    // 함수들
    LMS_Result (*load_from_file)(SystemSettings *settings, const char *filename);
    LMS_Result (*save_to_file)(SystemSettings *settings, const char *filename);
    void (*set_default)(SystemSettings *settings);
    void (*print)(SystemSettings *settings);
} SystemSettings;
```

## 5. 테스트 모듈 구조

### 5.1 단위 테스트 프레임워크

```c
// 테스트 케이스 구조
typedef struct TestCase {
    char name[100];
    void (*setup)(void);
    void (*test_function)(void);
    void (*teardown)(void);
    bool passed;
    char error_message[500];
} TestCase;

// 테스트 스위트
typedef struct TestSuite {
    char name[100];
    DoublyLinkedList *test_cases;
    int passed_count;
    int failed_count;

    void (*add_test)(TestSuite *suite, TestCase *test);
    void (*run_all)(TestSuite *suite);
    void (*print_results)(TestSuite *suite);
} TestSuite;
```

### 5.2 모듈별 테스트

```c
// 각 모듈별 테스트 함수들
void test_doubly_linked_list(void);
void test_book_repository(void);
void test_member_repository(void);
void test_loan_repository(void);
void test_book_service(void);
void test_member_service(void);
void test_loan_service(void);
void test_memory_manager(void);
void test_input_validation(void);
void test_report_generation(void);
```

이 컴포넌트 설계는 모듈화, 확장성, 유지보수성을 고려하여 각 계층과 모듈의 책임을 명확히 분리했습니다. 각 모듈은 독립적으로 테스트하고 수정할 수 있으며, 새로운 기능 추가나 기존 기능 변경 시에도 다른 모듈에 최소한의 영향을 미치도록 설계되었습니다.