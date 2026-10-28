@echo off
REM ============================================================
REM probar_memoria.bat - compila y ejecuta las pruebas del simulador
REM (Page, Process, MemoryManager): los 12 casos minimos del enunciado.
REM (Es un programa aparte: tiene su propio main.)
REM ============================================================
echo Compilando pruebas del simulador...
g++ -std=c++14 -Wall -Wextra -g pruebaMemoria.cpp MemoryManager.cpp Process.cpp Page.cpp Cache.cpp -o pruebaMemoria.exe
if %errorlevel% neq 0 (
    echo.
    echo *** ERROR de compilacion. Revise los mensajes de arriba. ***
    pause
    exit /b 1
)
echo Ejecutando pruebas...
echo.
pruebaMemoria.exe
echo.
pause
