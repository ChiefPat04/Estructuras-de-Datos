//Declaración de la clase AVL
#ifndef AVL_H
#define AVL_H

struct Nodo {
    int val;
    int altura;
    Nodo *izq, *der;
    Nodo(int v);
};

class AVL {
public:
    // true: al borrar un nodo con dos hijos se usa el sucesor inorden
    // (menor del subárbol derecho); false: el predecesor (mayor del izquierdo).
    bool usarSucesor;
    
    AVL(bool sucesor = true);
    ~AVL();

    void insertar(int v);
    void eliminar(int v);

    void imprimir() const;
    void preorden() const;
    void inorden() const;
    void porNiveles() const;

private:
    Nodo *raiz;

    static int alt(Nodo *n);
    static int balance(Nodo *n);
    static void actualizar(Nodo *n);
    static Nodo *rotarIzq(Nodo *x);
    static Nodo *rotarDer(Nodo *y);
    static Nodo *rebalancear(Nodo *n);
    static Nodo *minimo(Nodo *n);
    static Nodo *maximo(Nodo *n);

    Nodo *insertar(Nodo *n, int v);
    Nodo *eliminar(Nodo *n, int v);

    void imprimir(Nodo* n, int nivel) const;
    void preorden(Nodo* n) const;
    void inorden(Nodo* n) const;
    void liberar(Nodo* n);
};

#endif