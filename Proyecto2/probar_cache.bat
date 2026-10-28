@echo off
REM ============================================================
REM probar_cache.bat - compila y ejecuta SOLO las pruebas del cache LRU.
REM (Es un programa aparte: tiene su propio main.)
REM ============================================================
echo Compilando pruebas del cache LRU...
g++ -std=c++14 -Wall -Wextra -g pruebaCache.cpp Cache.cpp -o pruebaCache.exe
if %errorlevel% neq 0 (
    echo.
    echo *** ERROR de compilacion. Revise los mensajes de arriba. ***
    pause
    exit /b 1
)
echo Ejecutando pruebas...
echo.
pruebaCache.exe
echo.
pause
