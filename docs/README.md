# 도서관 관리 시스템 (Library Management System)

C언어 기반 Doubly Linked List를 활용한 도서관 관리 프로그램

## 프로젝트 개요

본 프로젝트는 C언어와 Doubly Linked List 자료구조를 사용하여 효율적인 도서관 관리 시스템을 구현합니다.

## 주요 기능

### 도서 관리
- 도서 추가/삭제/수정
- 도서 검색 (제목, 저자, ISBN)
- 도서 목록 조회

### 회원 관리
- 회원 등록/삭제/수정
- 회원 정보 조회

### 대출/반납 관리
- 도서 대출 처리
- 도서 반납 처리
- 대출 기록 관리
- 연체 도서 관리

## 기술 스택

- **언어**: C
- **자료구조**: Doubly Linked List
- **컴파일러**: GCC
- **개발환경**: 크로스 플랫폼 지원

## 디렉토리 구조

```
Doubly-Linked-List/
├── docs/                   # 문서
│   ├── README.md
│   ├── ARCHITECTURE.md
│   └── API_REFERENCE.md
├── src/                    # 소스 코드
│   ├── core/              # 핵심 자료구조
│   ├── models/            # 데이터 모델
│   ├── services/          # 비즈니스 로직
│   └── utils/             # 유틸리티
├── include/               # 헤더 파일
├── tests/                 # 테스트 코드
├── examples/              # 예제 코드
└── Makefile              # 빌드 설정
```

## 빌드 및 실행

```bash
# 컴파일
make

# 실행
./library_system

# 테스트
make test

# 정리
make clean
```

## 라이선스

MIT License