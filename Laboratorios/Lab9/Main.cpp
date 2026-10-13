/*
 * ============================================================================
 *  main.cpp  -  Programa de prueba del Laboratorio de Árboles B
 * ============================================================================
 *  1. Inserta en secuencia:
 *     10 20 5 12 30 7 17 8 14 32 9 18 1 15 11 35 47 36 39 41 42 16 48 49 13
 *  2. Busca en secuencia: 12, 100
 *  3. Elimina en secuencia: 12, 30, 10
 *
 *  Después de cada operación se muestra el árbol por niveles y se verifica
 *  que siga cumpliendo las propiedades de un árbol B.
 *
 *  Compilar:  g++ -std=c++11 main.cpp BTree.cpp -o labbtree
 *  Ejecutar:  ./labbtree        (en Windows: labbtree.exe)
 * ============================================================================
 */
#include "BTree.h"
#include <iostream>

using namespace std;

// Muestra el estado del árbol y el resultado de la verificación
void mostrar(const BTree& arbol) {
    arbol.imprimir();
    cout << "  Propiedades de arbol B: "
         << (arbol.verificar() ? "CORRECTAS" : "ERROR") << "\n\n";
}

int main() {
    BTree arbol;

    const int insertar[]  = {10, 20, 5, 12, 30, 7, 17, 8, 14, 32, 9, 18, 1,
                             15, 11, 35, 47, 36, 39, 41, 42, 16, 48, 49, 13};
    const int buscar[]    = {12, 100};
    const int eliminar[]  = {12, 30, 10};

    cout << "==================================================\n";
    cout << " LABORATORIO ARBOLES B  (orden " << ORDEN
         << ": max " << MAX_LLAVES << " llaves, min " << MIN_LLAVES << ")\n";
    cout << "==================================================\n\n";

    // ---------------- 1. INSERCIONES ----------------
    cout << "---------------- 1. INSERCIONES ----------------\n";
    for (int llave : insertar) {
        arbol.insertar(llave);
        cout << "Insertar " << llave << ":\n";
        mostrar(arbol);
    }
    arbol.exportarDot("1_insercion.dot", "Arbol B despues de las 25 inserciones");

    // ---------------- 2. BÚSQUEDAS ----------------
    cout << "---------------- 2. BUSQUEDAS ------------------\n";
    for (int llave : buscar) arbol.buscar(llave);
    cout << "\n";

    // ---------------- 3. ELIMINACIONES ----------------
    cout << "---------------- 3. ELIMINACIONES --------------\n";
    int paso = 2;
    for (int llave : eliminar) {
        bool ok = arbol.eliminar(llave);
        cout << "Eliminar " << llave << (ok ? " (eliminado)" : " (no existe)") << ":\n";
        mostrar(arbol);
        arbol.exportarDot(to_string(paso++) + "_eliminar_" + to_string(llave) + ".dot",
                          "Arbol B despues de eliminar " + to_string(llave));
    }

    return 0;
}