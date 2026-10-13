@echo off
REM ============================================================
REM compilar.bat - compila el proyecto en Windows con g++ (MinGW)
REM Doble clic, o ejecutar "compilar.bat" desde la terminal.
REM Cada archivo .cpp NUEVO del sistema se agrega a la linea de g++.
REM (Los archivos pruebaXXX.cpp NO van aqui: tienen su propio main.)
REM ============================================================
echo Compilando...
g++ -std=c++14 -Wall -Wextra -g main.cpp Utils.cpp Estudiante.cpp HistorialAcademico.cpp GestorEstudiantes.cpp ArbolGenerico.cpp GestorCursos.cpp GestorNavegacion.cpp GestorSolicitudes.cpp Reportes.cpp Persistencia.cpp Menus.cpp MenuNavegacion.cpp MenuSolicitudes.cpp MenuIndiceReportes.cpp -o proyecto.exe
if %errorlevel% neq 0 (
    echo.
    echo *** ERROR de compilacion. Revise los mensajes de arriba. ***
) else (
    echo.
    echo Compilacion exitosa. Ejecute: proyecto.exe
)
pause
