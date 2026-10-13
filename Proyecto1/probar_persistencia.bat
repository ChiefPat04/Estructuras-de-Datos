@echo off
REM probar_persistencia.bat - pruebas de guardar y cargar
echo Compilando...
g++ -std=c++14 -Wall -Wextra -g pruebaPersistencia.cpp Persistencia.cpp Reportes.cpp GestorEstudiantes.cpp Estudiante.cpp HistorialAcademico.cpp ArbolGenerico.cpp GestorCursos.cpp GestorNavegacion.cpp GestorSolicitudes.cpp Utils.cpp -o pruebaPersistencia.exe
if %errorlevel% neq 0 (
    echo.
    echo *** ERROR de compilacion. Revise los mensajes de arriba. ***
    pause
    exit /b 1
)
echo Ejecutando pruebas...
echo.
pruebaPersistencia.exe
echo.
pause
