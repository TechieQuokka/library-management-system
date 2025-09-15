@echo off
echo Building Library Management System for Windows...

REM Create obj directories
if not exist obj mkdir obj
if not exist obj\core mkdir obj\core
if not exist obj\models mkdir obj\models
if not exist obj\repositories mkdir obj\repositories
if not exist obj\services mkdir obj\services
if not exist obj\ui mkdir obj\ui
if not exist obj\test mkdir obj\test

echo Compiling source files...

REM Compile core files
gcc -Wall -Wextra -std=c11 -g -O0 -Iinclude -c src\core\doubly_linked_list.c -o obj\core\doubly_linked_list.o

REM Compile model files
gcc -Wall -Wextra -std=c11 -g -O0 -Iinclude -c src\models\book.c -o obj\models\book.o
gcc -Wall -Wextra -std=c11 -g -O0 -Iinclude -c src\models\member.c -o obj\models\member.o
gcc -Wall -Wextra -std=c11 -g -O0 -Iinclude -c src\models\loan.c -o obj\models\loan.o

REM Compile repository files
gcc -Wall -Wextra -std=c11 -g -O0 -Iinclude -c src\repositories\book_repository.c -o obj\repositories\book_repository.o
gcc -Wall -Wextra -std=c11 -g -O0 -Iinclude -c src\repositories\member_repository.c -o obj\repositories\member_repository.o
gcc -Wall -Wextra -std=c11 -g -O0 -Iinclude -c src\repositories\loan_repository.c -o obj\repositories\loan_repository.o

REM Compile service files
gcc -Wall -Wextra -std=c11 -g -O0 -Iinclude -c src\services\book_service.c -o obj\services\book_service.o
gcc -Wall -Wextra -std=c11 -g -O0 -Iinclude -c src\services\member_service.c -o obj\services\member_service.o
gcc -Wall -Wextra -std=c11 -g -O0 -Iinclude -c src\services\loan_service.c -o obj\services\loan_service.o

REM Compile UI files
gcc -Wall -Wextra -std=c11 -g -O0 -Iinclude -c src\ui\menu_system.c -o obj\ui\menu_system.o
gcc -Wall -Wextra -std=c11 -g -O0 -Iinclude -c src\ui\input_handler.c -o obj\ui\input_handler.o
gcc -Wall -Wextra -std=c11 -g -O0 -Iinclude -c src\ui\output_formatter.c -o obj\ui\output_formatter.o

REM Compile main file
gcc -Wall -Wextra -std=c11 -g -O0 -Iinclude -c main.c -o obj\main.o

echo Linking executable...

REM Link all object files to create executable
gcc obj\core\doubly_linked_list.o obj\models\book.o obj\models\member.o obj\models\loan.o obj\repositories\book_repository.o obj\repositories\member_repository.o obj\repositories\loan_repository.o obj\services\book_service.o obj\services\member_service.o obj\services\loan_service.o obj\ui\menu_system.o obj\ui\input_handler.o obj\ui\output_formatter.o obj\main.o -o library_system.exe

if exist library_system.exe (
    echo Build successful! Run library_system.exe to start the program.
) else (
    echo Build failed! Check the error messages above.
)

pause