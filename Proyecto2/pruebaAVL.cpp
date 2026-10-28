// pruebaAVL.cpp - pruebas del arbol AVL (tiene su propio main, se compila aparte)

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "AVLTree.h"

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

typedef AVLTree<int, std::string> Arbol;
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
    verificar(a.altura() == 0, "altura() = 0");
    verificar(a.buscar(10) == nullptr, "buscar en vacio devuelve nullptr");
    verificar(a.minimo() == nullptr, "minimo en vacio devuelve nullptr");
    verificar(!a.eliminar(10), "eliminar en vacio devuelve false");
    verificar(a.inOrden().empty(), "inOrden de vacio es lista vacia");
}

static void pruebaInsertarYBuscar() {
    std::cout << "\n== Insertar, buscar e inOrden ==\n";
    Arbol a;
    verificar(a.insertar(5, "cinco"), "insertar 5 devuelve true");
    verificar(a.insertar(3, "tres"), "insertar 3 devuelve true");
    verificar(a.insertar(8, "ocho"), "insertar 8 devuelve true");
    verificar(!a.insertar(3, "otro"), "insertar 3 otra vez devuelve false (ya existe)");
    verificar(a.contar() == 3, "contar() = 3");
    verificar(aTexto(a.inOrden()) == "3 5 8", "inOrden sale ordenado: 3 5 8");

    std::string* encontrado = a.buscar(3);
    verificar(encontrado != nullptr && *encontrado == "tres", "buscar 3 encuentra 'tres'");
    verificar(a.buscar(99) == nullptr, "buscar 99 devuelve nullptr (no existe)");
    verificar(a.contiene(8) && !a.contiene(4), "contiene(8) true y contiene(4) false");
}

static void pruebaBalanceSecuencial() {
    std::cout << "\n== Insertar 1..100 en orden (debe rebalancearse) ==\n";
    AVLTree<int, int> a;
    for (int i = 1; i <= 100; i++) {
        a.insertar(i, i * 10);
    }
    verificar(a.contar() == 100, "contar() = 100");
    verificar(a.estaBalanceado(), "el arbol sigue siendo AVL valido");
    // Un AVL con 100 nodos tiene altura <= 1.44*log2(101), o sea como mucho 9.
    verificar(a.altura() <= 9, "altura razonable (<= 9) para 100 nodos");
    verificar(a.buscar(73) != nullptr && *a.buscar(73) == 730, "buscar 73 devuelve 730");
}

static void pruebaEliminar() {
    std::cout << "\n== Eliminar ==\n";
    AVLTree<int, int> a;
    for (int i = 1; i <= 20; i++) {
        a.insertar(i, i);
    }
    verificar(a.eliminar(10), "eliminar 10 devuelve true");
    verificar(!a.eliminar(10), "eliminar 10 otra vez devuelve false");
    verificar(!a.contiene(10), "10 ya no esta en el arbol");
    verificar(a.contar() == 19, "contar() baja a 19");
    verificar(a.estaBalanceado(), "sigue siendo AVL valido tras eliminar");

    for (int i = 1; i <= 20; i++) {
        a.eliminar(i);
    }
    verificar(a.vacio() && a.contar() == 0, "al eliminar todo, el arbol queda vacio");
}

static void pruebaExtraerMinimo() {
    std::cout << "\n== Extraer minimo (para tomar paginas libres) ==\n";
    AVLTree<int, std::string> a;
    a.insertar(7, "p7");
    a.insertar(2, "p2");
    a.insertar(9, "p9");

    int clave = 0;
    std::string valor;
    verificar(a.extraerMinimo(clave, valor) && clave == 2 && valor == "p2",
              "extraerMinimo saca la clave 2");
    verificar(a.contar() == 2, "quedan 2 elementos");
    verificar(a.estaBalanceado(), "sigue siendo AVL valido");
}

int main() {
    std::cout << "=== PRUEBAS DEL ARBOL AVL ===\n";

    pruebaVacio();
    pruebaInsertarYBuscar();
    pruebaBalanceSecuencial();
    pruebaEliminar();
    pruebaExtraerMinimo();

    std::cout << "\nResultado: " << pasadas << " aprobadas, "
              << falladas << " falladas.\n";
    return (falladas == 0) ? 0 : 1;
}
