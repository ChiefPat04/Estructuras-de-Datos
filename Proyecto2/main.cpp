// main.cpp - menu principal del simulador de memoria paginada.
//
// Formas de usarlo:
//   proyecto.exe                       -> muestra el menu
//   proyecto.exe entrada_prueba.txt    -> procesa ese archivo directamente
//                                         (sirve para guardar evidencias:
//                                          proyecto.exe entrada.txt > salida.txt)

#include <iostream>
#include <string>
#include "OperationParser.h"

// Quita espacios al inicio y al final (para la opcion del menu y el nombre del archivo).
static std::string limpiar(const std::string& texto) {
    const std::string espacios = " \t\r\n";
    std::size_t inicio = texto.find_first_not_of(espacios);
    if (inicio == std::string::npos) return "";
    std::size_t fin = texto.find_last_not_of(espacios);
    return texto.substr(inicio, fin - inicio + 1);
}

// Simula un archivo de operaciones completo. Cada simulacion empieza de cero.
static void simularArchivo(const std::string& ruta) {
    OperationParser parser(std::cout);
    parser.procesarArchivo(ruta);
    parser.mostrarResumen();
}

static void simularConsola() {
    OperationParser parser(std::cout);
    parser.procesarConsola(std::cin);
    parser.mostrarResumen();
}

int main(int argc, char* argv[]) {
    // Si se paso un archivo al ejecutar, se procesa sin mostrar el menu.
    if (argc > 1) {
        OperationParser parser(std::cout);
        bool exito = parser.procesarArchivo(argv[1]);
        parser.mostrarResumen();
        return exito ? 0 : 1;
    }

    std::string opcion;
    while (true) {
        std::cout << "\n============================================================\n"
                  << " SIMULADOR DE MEMORIA PAGINADA (AVL + Splay Tree)\n"
                  << "============================================================\n"
                  << " 1. Leer operaciones desde un archivo\n"
                  << " 2. Escribir operaciones en la consola\n"
                  << " 3. Salir\n"
                  << "Opcion: ";
        if (!std::getline(std::cin, opcion)) break;
        opcion = limpiar(opcion);

        if (opcion == "1") {
            std::cout << "Nombre del archivo (ENTER = entrada_prueba.txt): ";
            std::string ruta;
            if (!std::getline(std::cin, ruta)) break;
            ruta = limpiar(ruta);
            if (ruta.empty()) ruta = "entrada_prueba.txt";
            simularArchivo(ruta);
        } else if (opcion == "2") {
            simularConsola();
            // Si se cerro la entrada (Ctrl+Z), no se puede volver al menu.
            if (!std::cin) break;
        } else if (opcion == "3") {
            break;
        } else {
            std::cout << "Opcion invalida: escriba 1, 2 o 3.\n";
        }
    }

    std::cout << "Fin del programa.\n";
    return 0;
}
