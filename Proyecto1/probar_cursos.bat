@echo off
REM ============================================================
REM probar_cursos.bat - compila y ejecuta SOLO las pruebas del arbol
REM generico y de GestorCursos. (Programa aparte: tiene su propio main.)
REM ============================================================
echo Compilando pruebas de cursos...
g++ -std=c++14 -Wall -Wextra -g pruebaCursos.cpp ArbolGenerico.cpp GestorCursos.cpp Utils.cpp -o pruebaCursos.exe
if %errorlevel% neq 0 (
    echo.
    echo *** ERROR de compilacion. Revise los mensajes de arriba. ***
    pause
    exit /b 1
)
echo Ejecutando pruebas...
echo.
pruebaCursos.exe
echo.
pause
