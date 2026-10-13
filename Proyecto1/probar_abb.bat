@echo off
REM ============================================================
REM probar_abb.bat - compila y ejecuta SOLO las pruebas del ABB.
REM (Es un programa aparte: tiene su propio main, por eso no se
REM  compila junto con main.cpp.)
REM ============================================================
echo Compilando pruebas del ABB...
g++ -std=c++14 -Wall -Wextra -g pruebaABB.cpp -o pruebaABB.exe
if %errorlevel% neq 0 (
    echo.
    echo *** ERROR de compilacion. Revise los mensajes de arriba. ***
    pause
    exit /b 1
)
echo Ejecutando pruebas...
echo.
pruebaABB.exe
echo.
pause
