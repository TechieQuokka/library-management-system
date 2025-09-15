# Library Management System

A complete library management system implemented in C using doubly linked lists as the core data structure.

## Features

### Core Functionality
- **Book Management**: Add, update, delete, and search books
- **Member Management**: Register, manage, and track library members
- **Loan Management**: Handle book borrowing, returning, and renewals
- **Search System**: Advanced search capabilities across all data types
- **Reporting**: Generate various reports and statistics

### Technical Features
- **Doubly Linked List**: Custom implementation with full CRUD operations
- **Modular Architecture**: Clean separation of concerns with repository, service, and UI layers
- **Memory Management**: Efficient memory usage with proper cleanup
- **Input Validation**: Comprehensive data validation throughout
- **Error Handling**: Robust error handling and reporting
- **Test Framework**: Complete test suite with unit and integration tests

## Architecture

The system follows a layered architecture pattern:

```
┌─────────────────────────────────────┐
│           User Interface            │
├─────────────────────────────────────┤
│          Business Logic             │
│    (Services Layer)                 │
├─────────────────────────────────────┤
│          Data Access                │
│    (Repository Layer)               │
├─────────────────────────────────────┤
│         Core Data Structures        │
│    (Doubly Linked List)             │
└─────────────────────────────────────┘
```

## Building and Running

### Prerequisites
- GCC compiler (version 7.0+)
- Make utility
- Standard C library

### Build Commands

```bash
# Basic build
make

# Debug build with AddressSanitizer
make debug

# Release build (optimized)
make release

# Run tests
make test

# Memory leak check
make memcheck

# Clean build artifacts
make clean
```

### Running the Application

```bash
# Run the main application
./library_system

# Run tests
./test_runner
```

## Usage

### Main Menu Options

1. **Book Management**
   - Add new books to the library
   - Search for books by title, author, ISBN, or category
   - Update book information
   - View available books

2. **Member Management**
   - Register new library members
   - Search and view member information
   - Manage member status (active/suspended)
   - Track member loan history

3. **Loan Management**
   - Process book loans
   - Handle book returns
   - Manage loan renewals
   - Track overdue books and fines

4. **Reports and Statistics**
   - View library statistics
   - Generate various reports
   - Monitor system performance

### Sample Data

The system includes sample data to demonstrate functionality:
- Sample books from programming and literature categories
- Test member accounts
- Example loan transactions

## Data Models

### Book
- ISBN (13-digit identifier)
- Title, Author, Publisher
- Publication year and category
- Copy management (total/available)
- Price and status

### Member
- Unique member ID
- Personal information (name, contact)
- Membership type (Regular/Premium)
- Loan tracking and status

### Loan
- Unique loan ID
- Member and book references
- Date tracking (loan/due/return)
- Fine calculation and status

## File Structure

```
├── src/
│   ├── core/           # Core data structures
│   ├── models/         # Data models
│   ├── repositories/   # Data access layer
│   ├── services/       # Business logic
│   ├── ui/            # User interface
│   └── utils/         # Utilities
├── include/           # Header files
├── tests/             # Test suite
├── docs/              # Documentation
├── examples/          # Example code
└── Makefile          # Build configuration
```

## Testing

The project includes a comprehensive test suite:

```bash
# Run all tests
make test

# Individual test categories
- Doubly Linked List operations
- Data model validation
- Repository CRUD operations
- Service layer business logic
```

## Development Guidelines

### Code Style
- Use ASCII characters only for maximum compatibility
- Follow consistent naming conventions
- Implement proper error handling
- Include comprehensive comments

### Memory Management
- Always free allocated memory
- Use proper null checks
- Avoid memory leaks (verified with Valgrind)

### Testing
- Write tests for all new functionality
- Maintain high test coverage
- Test both success and failure cases

## Performance Characteristics

### Time Complexity
- Insert/Delete: O(1) for known positions, O(n) for search
- Search: O(n) for linear search
- Sort: O(n log n) using merge sort

### Space Complexity
- Memory usage scales linearly with data size
- Efficient memory pooling for frequent allocations

## Extensibility

The modular design allows for easy extension:
- Add new data models
- Implement additional search algorithms
- Extend reporting capabilities
- Add file persistence
- Integrate with databases

## Contributing

1. Follow the established code style
2. Write comprehensive tests
3. Update documentation
4. Ensure memory safety

## License

This project is provided as an educational example. Feel free to use and modify for learning purposes.

## ASCII-Only Design

This system is specifically designed to use only ASCII characters for maximum compatibility across different systems and environments. All text output, data validation, and user interfaces are ASCII-compliant.

---

Built with C programming language and doubly linked lists for educational and practical library management.