@echo off
REM probar_reportes.bat - pruebas de los reportes
echo Compilando...
g++ -std=c++14 -Wall -Wextra -g pruebaReportes.cpp Reportes.cpp GestorEstudiantes.cpp Estudiante.cpp HistorialAcademico.cpp ArbolGenerico.cpp GestorCursos.cpp GestorNavegacion.cpp GestorSolicitudes.cpp Utils.cpp -o pruebaReportes.exe
if %errorlevel% neq 0 (
    echo.
    echo *** ERROR de compilacion. Revise los mensajes de arriba. ***
    pause
    exit /b 1
)
echo Ejecutando pruebas...
echo.
pruebaReportes.exe
echo.
pause
