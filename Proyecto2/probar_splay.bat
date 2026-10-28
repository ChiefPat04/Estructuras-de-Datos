@echo off
REM ============================================================
REM probar_splay.bat - compila y ejecuta SOLO las pruebas del Splay Tree.
REM (Es un programa aparte: tiene su propio main.)
REM ============================================================
echo Compilando pruebas del Splay Tree...
g++ -std=c++14 -Wall -Wextra -g pruebaSplay.cpp -o pruebaSplay.exe
if %errorlevel% neq 0 (
    echo.
    echo *** ERROR de compilacion. Revise los mensajes de arriba. ***
    pause
    exit /b 1
)
echo Ejecutando pruebas...
echo.
pruebaSplay.exe
echo.
pause
