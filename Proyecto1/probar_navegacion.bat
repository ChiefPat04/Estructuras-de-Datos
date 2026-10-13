@echo off
REM probar_navegacion.bat - pruebas del historial y la navegacion
echo Compilando...
g++ -std=c++14 -Wall -Wextra -g pruebaNavegacion.cpp HistorialAcademico.cpp GestorNavegacion.cpp GestorEstudiantes.cpp Estudiante.cpp ArbolGenerico.cpp GestorCursos.cpp Utils.cpp -o pruebaNavegacion.exe
if %errorlevel% neq 0 (
    echo.
    echo *** ERROR de compilacion. Revise los mensajes de arriba. ***
    pause
    exit /b 1
)
echo Ejecutando pruebas...
echo.
pruebaNavegacion.exe
echo.
pause
