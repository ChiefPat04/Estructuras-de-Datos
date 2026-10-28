@echo off
REM ============================================================
REM ejecutar_pruebas.bat - compila el simulador y lo ejecuta con los
REM tres archivos de entrada. La salida de cada uno se guarda en un
REM archivo salida_XXX.txt: esas son las evidencias de ejecucion.
REM ============================================================
echo Compilando...
g++ -std=c++14 -Wall -Wextra -g main.cpp OperationParser.cpp MemoryManager.cpp Process.cpp Page.cpp Cache.cpp -o proyecto.exe
if %errorlevel% neq 0 (
    echo.
    echo *** ERROR de compilacion. Revise los mensajes de arriba. ***
    pause
    exit /b 1
)

echo Ejecutando entrada_prueba.txt       (ejemplo del enunciado)...
proyecto.exe entrada_prueba.txt > salida_prueba.txt

echo Ejecutando entrada_casos.txt        (12 casos minimos y errores)...
proyecto.exe entrada_casos.txt > salida_casos.txt

echo Ejecutando entrada_error_config.txt (configuracion invalida)...
proyecto.exe entrada_error_config.txt > salida_error_config.txt

echo.
echo Listo. Se crearon:
echo   salida_prueba.txt
echo   salida_casos.txt
echo   salida_error_config.txt
echo Abralos con el Bloc de notas o VS Code para revisar la salida.
pause
