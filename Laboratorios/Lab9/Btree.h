// BTree.h  -  Declaración de la clase BTree (Árbol B)

// Un árbol B de orden m cumple que:
//1. Cada nodo tiene como máximo m hijos y (m - 1) llaves.
//2. Cada nodo (excepto la raíz) tiene como mínimo ceil(m/2) - 1 llaves.
//3. La raíz tiene al menos 1 llave (si el árbol no está vacío).
//4. Las llaves de cada nodo están ordenadas de menor a mayor.
//5. Todas las hojas están en el mismo nivel (es decir equilibrado).
//6. Toda llave es mayor que las del subárbol izquierdo y menor que
//   las del subárbol derecho.
 
//En esta implementación se usa orden = 5:
// - máximo 4 llaves y 5 hijos por nodo
// - mínimo 2 llaves por nodo (excepto la raíz)

#ifndef BTREE_H
#define BTREE_H

#include <string>

const int ORDEN = 5; // Orden del árbol (cantidad máxima de hijos por nodo)

const int MAX_LLAVES = ORDEN - 1; // Cantidad máxima de llaves por nodo: m - 1

const int MIN_LLAVES = (ORDEN + 1) / 2 - 1; // Cantidad mínima de llaves por nodo (excepto la raíz): ceil(m/2) - 1

//Nodo (o bloque) del árbol B.
//Se reserva espacio para UNA llave y UN hijo extra, para poder
//insertar temporalmente y luego dividir el nodo cuando se desborda.

struct Nodo {
    int   llaves[MAX_LLAVES + 1];   // llaves ordenadas
    Nodo* hijos[ORDEN + 1];         // punteros a los hijos
    int   n;                        // cantidad actual de llaves
    bool  hoja;                     // true si el nodo no tiene hijos

    explicit Nodo(bool esHoja);
};

class BTree {
public:
    BTree();
    ~BTree();

    void insertar(int llave);         
    bool buscar(int llave) const;     
    bool eliminar(int llave);          

    void imprimir() const;             // muestra el árbol por niveles
    bool verificar() const;            // comprueba las propiedades del árbol B
    void exportarDot(const std::string& archivo,
                     const std::string& titulo) const; // genera archivo Graphviz

private:
    Nodo* raiz;

    // Inserción 
    bool insertarRec(Nodo* nodo, int llave, int& promovida, Nodo*& nuevoDerecho);
    void dividir(Nodo* nodo, int& promovida, Nodo*& nuevoDerecho);

    // Eliminación 
    bool eliminarRec(Nodo* nodo, int llave);
    void reparar(Nodo* padre, int idx);
    void prestarDeIzquierdo(Nodo* padre, int idx);
    void prestarDeDerecho(Nodo* padre, int idx);
    void fusionar(Nodo* padre, int idx);

    // Utilidades 
    static int  posicion(const Nodo* nodo, int llave);
    static void liberar(Nodo* nodo);
    bool verificarRec(const Nodo* nodo, bool esRaiz, long minimo, long maximo,
                      int nivel, int& nivelHojas) const;
    static std::string textoNodo(const Nodo* nodo);
};

#endif 