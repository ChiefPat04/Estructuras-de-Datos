@echo off
REM ============================================================
REM probar_estudiantes.bat - compila y ejecuta SOLO las pruebas de
REM GestorEstudiantes. (Programa aparte: tiene su propio main.)
REM ============================================================
echo Compilando pruebas de estudiantes...
g++ -std=c++14 -Wall -Wextra -g pruebaEstudiantes.cpp GestorEstudiantes.cpp Estudiante.cpp HistorialAcademico.cpp Utils.cpp -o pruebaEstudiantes.exe
if %errorlevel% neq 0 (
    echo.
    echo *** ERROR de compilacion. Revise los mensajes de arriba. ***
    pause
    exit /b 1
)
echo Ejecutando pruebas...
echo.
pruebaEstudiantes.exe
echo.
pause
