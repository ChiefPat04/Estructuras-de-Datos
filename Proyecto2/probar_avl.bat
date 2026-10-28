@echo off
REM ============================================================
REM probar_avl.bat - compila y ejecuta SOLO las pruebas del AVL.
REM (Es un programa aparte: tiene su propio main.)
REM ============================================================
echo Compilando pruebas del AVL...
g++ -std=c++14 -Wall -Wextra -g pruebaAVL.cpp -o pruebaAVL.exe
if %errorlevel% neq 0 (
    echo.
    echo *** ERROR de compilacion. Revise los mensajes de arriba. ***
    pause
    exit /b 1
)
echo Ejecutando pruebas...
echo.
pruebaAVL.exe
echo.
pause
