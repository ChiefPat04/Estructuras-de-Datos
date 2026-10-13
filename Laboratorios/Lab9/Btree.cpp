
// 
BTree.cpp  -  Implementación de la clase BTree (Árbol B de orden 5)
#include "BTree.h"
#include <iostream>
#include <fstream>
#include <queue>
#include <utility>
#include <climits>

using namespace std;

/* ----------------------------------------------------------------------------
 *  Nodo
 * --------------------------------------------------------------------------*/
Nodo::Nodo(bool esHoja) : n(0), hoja(esHoja) {
    for (int i = 0; i <= ORDEN; i++) hijos[i] = nullptr;
}

/* ----------------------------------------------------------------------------
 *  Constructor y destructor
 * --------------------------------------------------------------------------*/
BTree::BTree() : raiz(nullptr) {}

BTree::~BTree() { liberar(raiz); }

// Libera recursivamente la memoria de todos los nodos.
void BTree::liberar(Nodo* nodo) {
    if (nodo == nullptr) return;
    if (!nodo->hoja)
        for (int i = 0; i <= nodo->n; i++) liberar(nodo->hijos[i]);
    delete nodo;
}

// Devuelve el índice de la primera llave >= 'llave' dentro del nodo.
// Sirve tanto para saber si la llave está en el nodo como para saber
// por cuál hijo hay que bajar.
int BTree::posicion(const Nodo* nodo, int llave) {
    int i = 0;
    while (i < nodo->n && nodo->llaves[i] < llave) i++;
    return i;
}

/* ============================================================================
 *  BÚSQUEDA
 *  Se baja desde la raíz: en cada nodo se busca la llave; si no está,
 *  se continúa por el hijo correspondiente hasta llegar a una hoja.
 * ==========================================================================*/
bool BTree::buscar(int llave) const {
    cout << "Buscar " << llave << ": recorrido ";
    Nodo* actual = raiz;
    while (actual != nullptr) {
        cout << textoNodo(actual);
        int i = posicion(actual, llave);
        if (i < actual->n && actual->llaves[i] == llave) {
            cout << "  -> ENCONTRADO (posicion " << i << " del nodo)\n";
            return true;
        }
        if (actual->hoja) break;          // no hay más niveles
        cout << " -> ";
        actual = actual->hijos[i];        // bajar al subárbol correcto
    }
    cout << "  -> NO ENCONTRADO\n";
    return false;
}

/* ============================================================================
 *  INSERCIÓN
 *  Siempre se inserta en una hoja. Si un nodo queda con más de MAX_LLAVES,
 *  se divide en dos y la llave del medio "sube" al padre. Si la raíz se
 *  divide, se crea una raíz nueva y el árbol crece un nivel.
 * ==========================================================================*/
void BTree::insertar(int llave) {
    // Árbol vacío: la raíz es una hoja con una sola llave
    if (raiz == nullptr) {
        raiz = new Nodo(true);
        raiz->llaves[0] = llave;
        raiz->n = 1;
        return;
    }

    int promovida;
    Nodo* nuevoDerecho;
    // Si la raíz se dividió, se crea una nueva raíz con la llave promovida
    if (insertarRec(raiz, llave, promovida, nuevoDerecho)) {
        Nodo* nuevaRaiz = new Nodo(false);
        nuevaRaiz->llaves[0] = promovida;
        nuevaRaiz->hijos[0] = raiz;
        nuevaRaiz->hijos[1] = nuevoDerecho;
        nuevaRaiz->n = 1;
        raiz = nuevaRaiz;
    }
}

// Inserta recursivamente. Devuelve true si 'nodo' se dividió; en ese caso
// 'promovida' es la llave que sube y 'nuevoDerecho' el nodo nuevo creado.
bool BTree::insertarRec(Nodo* nodo, int llave, int& promovida, Nodo*& nuevoDerecho) {
    int i = posicion(nodo, llave);

    // No se permiten llaves repetidas
    if (i < nodo->n && nodo->llaves[i] == llave) return false;

    if (nodo->hoja) {
        // Correr las llaves mayores una posición a la derecha e insertar
        for (int j = nodo->n; j > i; j--) nodo->llaves[j] = nodo->llaves[j - 1];
        nodo->llaves[i] = llave;
        nodo->n++;
    } else {
        // Bajar al hijo i; si ese hijo se divide, recibimos una llave
        int subio;
        Nodo* derecho;
        if (!insertarRec(nodo->hijos[i], llave, subio, derecho)) return false;

        // Insertar la llave que subió (y su nuevo hijo derecho) en este nodo
        for (int j = nodo->n; j > i; j--) {
            nodo->llaves[j]     = nodo->llaves[j - 1];
            nodo->hijos[j + 1]  = nodo->hijos[j];
        }
        nodo->llaves[i]    = subio;
        nodo->hijos[i + 1] = derecho;
        nodo->n++;
    }

    // ¿Se desbordó el nodo? Entonces se divide
    if (nodo->n > MAX_LLAVES) {
        dividir(nodo, promovida, nuevoDerecho);
        return true;
    }
    return false;
}

// Divide un nodo desbordado (MAX_LLAVES + 1 llaves) en dos nodos.
// La llave del medio se devuelve en 'promovida' para subirla al padre.
void BTree::dividir(Nodo* nodo, int& promovida, Nodo*& nuevoDerecho) {
    int medio = nodo->n / 2;                 // con 5 llaves: índice 2
    promovida = nodo->llaves[medio];

    nuevoDerecho = new Nodo(nodo->hoja);
    // Copiar al nodo derecho las llaves (e hijos) que están después del medio
    int k = 0;
    for (int j = medio + 1; j < nodo->n; j++, k++) {
        nuevoDerecho->llaves[k] = nodo->llaves[j];
        nuevoDerecho->hijos[k]  = nodo->hijos[j];
    }
    nuevoDerecho->hijos[k] = nodo->hijos[nodo->n];
    nuevoDerecho->n = k;

    // El nodo original se queda solo con las llaves antes del medio
    for (int j = medio + 1; j <= nodo->n; j++) nodo->hijos[j] = nullptr;
    nodo->n = medio;
}

/* ============================================================================
 *  ELIMINACIÓN
 *  Casos:
 *   a) La llave está en una hoja: se borra directamente.
 *   b) La llave está en un nodo interno: se reemplaza por su PREDECESOR
 *      (la mayor llave del subárbol izquierdo) y se borra el predecesor
 *      de la hoja donde estaba.
 *  Si después de borrar un hijo queda con menos de MIN_LLAVES, se repara:
 *   1. Préstamo del hermano izquierdo (si le sobran llaves).
 *   2. Préstamo del hermano derecho (si le sobran llaves).
 *   3. Si ninguno puede prestar, se fusiona con un hermano y la llave
 *      separadora del padre baja al nodo fusionado.
 *  Si la raíz se queda sin llaves, su único hijo pasa a ser la raíz
 *  (el árbol pierde un nivel).
 * ==========================================================================*/
bool BTree::eliminar(int llave) {
    if (raiz == nullptr) return false;

    bool borrado = eliminarRec(raiz, llave);

    // Si la raíz quedó vacía, se ajusta
    if (raiz->n == 0) {
        Nodo* vieja = raiz;
        raiz = raiz->hoja ? nullptr : raiz->hijos[0];
        delete vieja;
    }
    return borrado;
}

bool BTree::eliminarRec(Nodo* nodo, int llave) {
    int i = posicion(nodo, llave);
    bool estaAqui = (i < nodo->n && nodo->llaves[i] == llave);

    // Caso a) hoja
    if (nodo->hoja) {
        if (!estaAqui) return false;                 // la llave no existe
        for (int j = i; j < nodo->n - 1; j++) nodo->llaves[j] = nodo->llaves[j + 1];
        nodo->n--;
        return true;
    }

    bool borrado;
    if (estaAqui) {
        // Caso b) nodo interno: buscar el predecesor (lo más a la derecha
        // del subárbol izquierdo), copiarlo aquí y borrarlo abajo
        Nodo* p = nodo->hijos[i];
        while (!p->hoja) p = p->hijos[p->n];
        int predecesor = p->llaves[p->n - 1];
        nodo->llaves[i] = predecesor;
        borrado = eliminarRec(nodo->hijos[i], predecesor);
    } else {
        // La llave no está en este nodo: bajar por el hijo i
        borrado = eliminarRec(nodo->hijos[i], llave);
    }

    // Si el hijo quedó con muy pocas llaves, se repara
    if (nodo->hijos[i]->n < MIN_LLAVES) reparar(nodo, i);
    return borrado;
}

// Repara el hijo 'idx' de 'padre' que quedó con menos de MIN_LLAVES llaves
void BTree::reparar(Nodo* padre, int idx) {
    if (idx > 0 && padre->hijos[idx - 1]->n > MIN_LLAVES)
        prestarDeIzquierdo(padre, idx);
    else if (idx < padre->n && padre->hijos[idx + 1]->n > MIN_LLAVES)
        prestarDeDerecho(padre, idx);
    else if (idx > 0)
        fusionar(padre, idx - 1);      // fusionar con el hermano izquierdo
    else
        fusionar(padre, idx);          // fusionar con el hermano derecho
}

// Rotación a la derecha: la llave separadora del padre baja al hijo,
// y la última llave del hermano izquierdo sube al padre.
void BTree::prestarDeIzquierdo(Nodo* padre, int idx) {
    Nodo* hijo = padre->hijos[idx];
    Nodo* izq  = padre->hijos[idx - 1];

    // Abrir espacio al inicio del hijo
    for (int j = hijo->n; j > 0; j--) hijo->llaves[j] = hijo->llaves[j - 1];
    if (!hijo->hoja)
        for (int j = hijo->n + 1; j > 0; j--) hijo->hijos[j] = hijo->hijos[j - 1];

    hijo->llaves[0] = padre->llaves[idx - 1];
    if (!hijo->hoja) {
        hijo->hijos[0] = izq->hijos[izq->n];
        izq->hijos[izq->n] = nullptr;
    }
    hijo->n++;

    padre->llaves[idx - 1] = izq->llaves[izq->n - 1];
    izq->n--;
}

// Rotación a la izquierda: la llave separadora del padre baja al hijo,
// y la primera llave del hermano derecho sube al padre.
void BTree::prestarDeDerecho(Nodo* padre, int idx) {
    Nodo* hijo = padre->hijos[idx];
    Nodo* der  = padre->hijos[idx + 1];

    hijo->llaves[hijo->n] = padre->llaves[idx];
    if (!hijo->hoja) hijo->hijos[hijo->n + 1] = der->hijos[0];
    hijo->n++;

    padre->llaves[idx] = der->llaves[0];

    // Correr hacia la izquierda las llaves e hijos del hermano derecho
    for (int j = 0; j < der->n - 1; j++) der->llaves[j] = der->llaves[j + 1];
    if (!der->hoja)
        for (int j = 0; j < der->n; j++) der->hijos[j] = der->hijos[j + 1];
    der->hijos[der->n] = nullptr;
    der->n--;
}

// Fusiona hijos[idx] y hijos[idx + 1] en un solo nodo. La llave
// separadora del padre baja al medio del nodo fusionado.
void BTree::fusionar(Nodo* padre, int idx) {
    Nodo* izq = padre->hijos[idx];
    Nodo* der = padre->hijos[idx + 1];

    izq->llaves[izq->n] = padre->llaves[idx];      // baja la separadora
    for (int j = 0; j < der->n; j++) {
        izq->llaves[izq->n + 1 + j] = der->llaves[j];
        izq->hijos[izq->n + 1 + j]  = der->hijos[j];
    }
    izq->hijos[izq->n + 1 + der->n] = der->hijos[der->n];
    izq->n += der->n + 1;

    // Quitar la separadora y el puntero al nodo derecho del padre
    for (int j = idx; j < padre->n - 1; j++) {
        padre->llaves[j]    = padre->llaves[j + 1];
        padre->hijos[j + 1] = padre->hijos[j + 2];
    }
    padre->hijos[padre->n] = nullptr;
    padre->n--;

    delete der;
}

/* ============================================================================
 *  IMPRESIÓN Y VERIFICACIÓN
 * ==========================================================================*/

// Devuelve el nodo como texto, por ejemplo: [5 7 8]
string BTree::textoNodo(const Nodo* nodo) {
    string s = "[";
    for (int i = 0; i < nodo->n; i++) {
        if (i > 0) s += " ";
        s += to_string(nodo->llaves[i]);
    }
    return s + "]";
}

// Imprime el árbol nivel por nivel (recorrido por anchura con una cola)
void BTree::imprimir() const {
    if (raiz == nullptr) { cout << "  (arbol vacio)\n"; return; }

    queue<pair<Nodo*, int>> cola;   // (nodo, nivel)
    cola.push({raiz, 0});
    int nivelActual = -1;
    while (!cola.empty()) {
        Nodo* nodo = cola.front().first;
        int nivel  = cola.front().second;
        cola.pop();
        if (nivel != nivelActual) {
            if (nivelActual != -1) cout << "\n";
            cout << "  Nivel " << nivel << ": ";
            nivelActual = nivel;
        }
        cout << textoNodo(nodo) << " ";
        if (!nodo->hoja)
            for (int i = 0; i <= nodo->n; i++) cola.push({nodo->hijos[i], nivel + 1});
    }
    cout << "\n";
}

// Comprueba todas las propiedades del árbol B
bool BTree::verificar() const {
    if (raiz == nullptr) return true;
    int nivelHojas = -1;
    return verificarRec(raiz, true, LONG_MIN, LONG_MAX, 0, nivelHojas);
}

bool BTree::verificarRec(const Nodo* nodo, bool esRaiz, long minimo, long maximo,
                         int nivel, int& nivelHojas) const {
    // 1) Cantidad de llaves dentro de los límites
    if (nodo->n > MAX_LLAVES) return false;
    if (!esRaiz && nodo->n < MIN_LLAVES) return false;
    if (esRaiz && nodo->n < 1) return false;

    // 2) Llaves ordenadas y dentro del rango permitido por el padre
    for (int i = 0; i < nodo->n; i++) {
        if (nodo->llaves[i] <= minimo || nodo->llaves[i] >= maximo) return false;
        if (i > 0 && nodo->llaves[i - 1] >= nodo->llaves[i]) return false;
    }

    // 3) Todas las hojas en el mismo nivel
    if (nodo->hoja) {
        if (nivelHojas == -1) nivelHojas = nivel;
        return nivelHojas == nivel;
    }

    // 4) Revisar cada subárbol con su rango (izq < llave < der)
    for (int i = 0; i <= nodo->n; i++) {
        long lo = (i == 0)       ? minimo : nodo->llaves[i - 1];
        long hi = (i == nodo->n) ? maximo : nodo->llaves[i];
        if (nodo->hijos[i] == nullptr) return false;
        if (!verificarRec(nodo->hijos[i], false, lo, hi, nivel + 1, nivelHojas))
            return false;
    }
    return true;
}

// Genera un archivo .dot (Graphviz) para dibujar el árbol como imagen
void BTree::exportarDot(const string& archivo, const string& titulo) const {
    ofstream out(archivo);
    out << "digraph BTree {\n"
        << "  labelloc=\"t\"; label=\"" << titulo << "\"; fontsize=18;\n"
        << "  node [shape=record, fontname=\"Helvetica\"];\n";
    if (raiz != nullptr) {
        queue<Nodo*> cola;
        cola.push(raiz);
        while (!cola.empty()) {
            Nodo* nodo = cola.front(); cola.pop();
            // Cada nodo: <h0>|llave|<h1>|llave|<h2> ...
            out << "  n" << nodo << " [label=\"";
            for (int i = 0; i < nodo->n; i++)
                out << "<h" << i << ">|" << nodo->llaves[i] << "|";
            out << "<h" << nodo->n << ">\"];\n";
            if (!nodo->hoja)
                for (int i = 0; i <= nodo->n; i++) {
                    out << "  n" << nodo << ":h" << i << " -> n" << nodo->hijos[i] << ";\n";
                    cola.push(nodo->hijos[i]);
                }
        }
    }
    out << "}\n";
}