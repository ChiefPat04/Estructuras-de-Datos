// pruebaSplay.cpp - pruebas del Splay Tree (tiene su propio main, se compila aparte)

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "SplayTree.h"

static int pasadas = 0;
static int falladas = 0;

// Registra el resultado de una comprobacion.
static void verificar(bool condicion, const std::string& descripcion) {
    if (condicion) {
        pasadas++;
        std::cout << "  [OK]    " << descripcion << "\n";
    } else {
        falladas++;
        std::cout << "  [FALLO] " << descripcion << "\n";
    }
}

typedef SplayTree<int, std::string> Arbol;
typedef std::vector<std::pair<int, std::string> > Lista;

// Convierte las claves de un recorrido a texto: "3 5 8".
// Se usa ostringstream porque std::to_string no existe en algunos MinGW antiguos.
static std::string aTexto(const Lista& recorrido) {
    std::ostringstream salida;
    for (std::size_t i = 0; i < recorrido.size(); i++) {
        if (i > 0) salida << " ";
        salida << recorrido[i].first;
    }
    return salida.str();
}

static void pruebaVacio() {
    std::cout << "\n== Arbol vacio ==\n";
    Arbol a;
    verificar(a.vacio(), "un arbol nuevo esta vacio");
    verificar(a.contar() == 0, "contar() = 0");
    verificar(a.buscar(10) == nullptr, "buscar en vacio devuelve nullptr");
    verificar(!a.eliminar(10), "eliminar en vacio devuelve false");
    verificar(a.recorrido().empty(), "recorrido de vacio es lista vacia");
}

static void pruebaInsertarYRecorrido() {
    std::cout << "\n== Insertar y recorrido ==\n";
    Arbol a;
    verificar(a.insertar(5, "cinco"), "insertar 5 devuelve true");
    verificar(a.insertar(3, "tres"), "insertar 3 devuelve true");
    verificar(a.insertar(8, "ocho"), "insertar 8 devuelve true");
    verificar(!a.insertar(3, "otro"), "insertar 3 otra vez devuelve false (ya existe)");
    verificar(a.contar() == 3, "contar() = 3");
    verificar(aTexto(a.recorrido()) == "3 5 8", "recorrido sale ordenado: 3 5 8");
}

static void pruebaBuscarSubeARaiz() {
    std::cout << "\n== Buscar sube la clave a la raiz (splay) ==\n";
    Arbol a;
    for (int i = 1; i <= 5; i++) {
        a.insertar(i, "p" + std::string(1, static_cast<char>('0' + i)));
    }
    // Con 1, 2, 3, 4, 5 insertados, la raiz es 5.
    verificar(a.estaEnRaiz(5), "antes de buscar, la raiz es 5");

    std::string* encontrado = a.buscar(2);
    verificar(encontrado != nullptr && *encontrado == "p2", "buscar 2 encuentra 'p2'");
    verificar(a.estaEnRaiz(2), "despues de buscar 2, la raiz es 2");
    verificar(aTexto(a.recorrido()) == "1 2 3 4 5", "el orden se mantiene tras el splay");
}

static void pruebaZigZig() {
    std::cout << "\n== Caso zig-zig (izquierda-izquierda) ==\n";
    Arbol a;
    // Insertar 1, 2, 3 deja una cadena: 3 -> 2 -> 1 hacia la izquierda.
    a.insertar(1, "a");
    a.insertar(2, "b");
    a.insertar(3, "c");
    verificar(a.estaEnRaiz(3), "la raiz es 3 despues de insertar 1, 2, 3");
    a.buscar(1);
    verificar(a.estaEnRaiz(1), "buscar 1 (zig-zig) la sube a la raiz");
    verificar(aTexto(a.recorrido()) == "1 2 3", "el orden se mantiene");
}

static void pruebaBuscarInexistente() {
    std::cout << "\n== Buscar una clave que no existe ==\n";
    Arbol a;
    a.insertar(10, "diez");
    a.insertar(20, "veinte");
    verificar(a.buscar(15) == nullptr, "buscar 15 devuelve nullptr");
    verificar(aTexto(a.recorrido()) == "10 20", "los datos siguen igual");
}

static void pruebaEliminar() {
    std::cout << "\n== Eliminar ==\n";
    Arbol a;
    for (int i = 1; i <= 7; i++) {
        a.insertar(i, "x");
    }
    verificar(a.eliminar(4), "eliminar 4 devuelve true");
    verificar(!a.eliminar(4), "eliminar 4 otra vez devuelve false");
    verificar(!a.contiene(4), "4 ya no esta");
    verificar(a.contar() == 6, "contar() baja a 6");
    verificar(aTexto(a.recorrido()) == "1 2 3 5 6 7", "el orden se mantiene sin el 4");

    verificar(a.eliminar(7) && aTexto(a.recorrido()) == "1 2 3 5 6", "eliminar 7 (el mayor) funciona");
    for (int i = 1; i <= 6; i++) {
        a.eliminar(i);
    }
    verificar(a.vacio() && a.contar() == 0, "al eliminar todo, el arbol queda vacio");
}

// Prueba de estres: 101 claves en orden "revuelto" (37 y 101 no tienen divisores
// en comun, asi que (i * 37) % 101 da todos los numeros del 0 al 100 una sola vez).
static void pruebaEstres() {
    std::cout << "\n== Prueba con 101 claves revueltas ==\n";
    Arbol a;
    for (int i = 0; i < 101; i++) {
        a.insertar((i * 37) % 101, "v");
    }
    verificar(a.contar() == 101, "contar() = 101");

    bool ordenado = true;
    Lista recorrido = a.recorrido();
    for (std::size_t i = 0; i < recorrido.size(); i++) {
        if (recorrido[i].first != static_cast<int>(i)) ordenado = false;
    }
    verificar(ordenado, "el recorrido sale ordenado del 0 al 100");

    bool todasEncontradas = true;
    for (int i = 0; i < 101; i++) {
        if (!a.contiene(i)) todasEncontradas = false;
    }
    verificar(todasEncontradas, "todas las claves 0..100 se encuentran");

    bool sinImpares = true;
    for (int i = 1; i < 101; i += 2) {
        a.eliminar(i);
    }
    for (int i = 1; i < 101; i += 2) {
        if (a.contiene(i)) sinImpares = false;
    }
    verificar(sinImpares, "al eliminar los impares, ya no quedan");
    verificar(a.contar() == 51, "quedan 51 pares");
}

int main() {
    std::cout << "=== PRUEBAS DEL SPLAY TREE ===\n";

    pruebaVacio();
    pruebaInsertarYRecorrido();
    pruebaBuscarSubeARaiz();
    pruebaZigZig();
    pruebaBuscarInexistente();
    pruebaEliminar();
    pruebaEstres();

    std::cout << "\nResultado: " << pasadas << " aprobadas, "
              << falladas << " falladas.\n";
    return (falladas == 0) ? 0 : 1;
}
