@echo off
REM probar_solicitudes.bat - pruebas de las solicitudes
echo Compilando...
g++ -std=c++14 -Wall -Wextra -g pruebaSolicitudes.cpp GestorSolicitudes.cpp GestorEstudiantes.cpp Estudiante.cpp HistorialAcademico.cpp ArbolGenerico.cpp GestorCursos.cpp Utils.cpp -o pruebaSolicitudes.exe
if %errorlevel% neq 0 (
    echo.
    echo *** ERROR de compilacion. Revise los mensajes de arriba. ***
    pause
    exit /b 1
)
echo Ejecutando pruebas...
echo.
pruebaSolicitudes.exe
echo.
pause
