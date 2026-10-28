@echo off
REM ============================================================
REM probar_parser.bat - compila y ejecuta las pruebas del lector de
REM operaciones (modo archivo y modo consola).
REM (Es un programa aparte: tiene su propio main.)
REM ============================================================
echo Compilando pruebas del lector de operaciones...
g++ -std=c++14 -Wall -Wextra -g pruebaParser.cpp OperationParser.cpp MemoryManager.cpp Process.cpp Page.cpp Cache.cpp -o pruebaParser.exe
if %errorlevel% neq 0 (
    echo.
    echo *** ERROR de compilacion. Revise los mensajes de arriba. ***
    pause
    exit /b 1
)
echo Ejecutando pruebas...
echo.
pruebaParser.exe
echo.
pause
