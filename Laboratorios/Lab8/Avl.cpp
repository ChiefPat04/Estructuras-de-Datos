//Implementación de la clase AVL y Main

#include "Avl.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

Nodo::Nodo(int v) : val(v), altura(1), izq(nullptr), der(nullptr) {}

AVL::AVL(bool sucesor) : usarSucesor(sucesor), raiz(nullptr) {}
AVL::~AVL() { liberar(raiz); }

int AVL::alt(Nodo* n){return n ? n->altura : 0; }
int AVL::balance(Nodo* n){return n ? alt(n->izq) - alt(n->der) : 0; }
void AVL::actualizar(Nodo* n){n->altura = 1 + max(alt(n->izq), alt(n->der)); }

Nodo* AVL::rotarDer(Nodo* y){
    Nodo* x = y->izq;
    y->izq = x->der;
    x->der = y;
    actualizar(y);
    actualizar(x);
    return x;
}

Nodo* AVL::rotarIzq(Nodo* x){
    Nodo* y = x->der;
    x->der = y->izq;
    y->izq = x;
    actualizar(x);
    actualizar(y);
    return y;
}

Nodo* AVL::rebalancear(Nodo* n){
    actualizar(n);
    int b = balance(n);
    if (b > 1) {
        if (balance(n->izq) < 0) n->izq = rotarIzq(n->izq);
        return rotarDer(n);
    }
    if (b < -1) {
        if (balance(n->der) > 0) n->der = rotarDer(n->der);
        return rotarIzq(n);
    }
    return n;
}

Nodo* AVL::minimo(Nodo* n){
    while (n && n->izq) n = n->izq;
    return n;
}

Nodo* AVL::maximo(Nodo* n){
    while (n && n->der) n = n->der;
    return n;
}

void AVL::insertar(int v) { raiz = insertar(raiz, v); }
void AVL::eliminar(int v) { raiz = eliminar(raiz, v); }

Nodo* AVL::insertar(Nodo* n, int v){
    if (!n) return new Nodo(v);
    if (v < n->val) n->izq = insertar(n->izq, v);
    else if (v > n->val) n->der = insertar(n->der, v);
    else return n; // No se permiten duplicados
    return rebalancear(n);
}

Nodo* AVL::eliminar(Nodo* n, int v){
    if (!n) return nullptr;
    if (v < n->val) n->izq = eliminar(n->izq, v);
    else if (v > n->val) n->der = eliminar(n->der, v);
    else {
        if (!n->izq || !n->der) {
            Nodo* hijo = n->izq ? n->izq : n->der;
            delete n;
            return hijo;
        } 
        if (usarSucesor) {
            Nodo* sucesor = minimo(n->der);
            n->val = sucesor->val;
            n->der = eliminar(n->der, sucesor->val);
        } else {
            Nodo* predecesor = maximo(n->izq);
            n->val = predecesor->val;
            n->izq = eliminar(n->izq, predecesor->val);
        }
    }
    return rebalancear(n);
}

void AVL::imprimir() const { imprimir(raiz, 0); }
void AVL::preorden() const { preorden(raiz); cout << endl; }
void AVL::inorden() const { inorden(raiz); cout << endl; }

void AVL::imprimir(Nodo* n, int nivel) const {
    if (!n) return;
    imprimir(n->der, nivel + 1);
    cout << string(nivel * 4, ' ') << n->val << endl;
    imprimir(n->izq, nivel + 1);
}

void AVL::preorden(Nodo* n) const {
    if (!n) return;
    cout << n->val << " ";
    preorden(n->izq);
    preorden(n->der);
}

void AVL::inorden(Nodo* n) const {
    if (!n) return;
    inorden(n->izq);
    cout << n->val << " ";
    inorden(n->der);
}

void AVL::porNiveles() const {
    vector<Nodo*> actual{raiz};
    int nivel = 0;
    while (!actual.empty() && actual[0]) {
        cout << "Nivel " << nivel++ << ": ";
        vector<Nodo*> sig;
        for (Nodo* n : actual) {
            cout << n->val << "(fe=" << balance(n) << ") ";
            if (n->izq) sig.push_back(n->izq);
            if (n->der) sig.push_back(n->der);
        }
        cout << "\n";
        actual = sig;
    }
}

void AVL::liberar(Nodo* n){
    if (!n) return;
    liberar(n->izq);
    liberar(n->der);
    delete n;
}

int main() {
    vector<int> inserciones = {12, 20, 16, 18, 8, 25, 19, 6, 1, 4, 30, 29, 2, 3, 23};
    vector<int> eliminaciones = {3, 19, 20, 25};
 
    for (bool sucesor : {true, false}) {
        cout << "==========================================================\n";
        cout << "  Eliminacion con nodo de dos hijos usando: "
             << (sucesor ? "SUCESOR inorden" : "PREDECESOR inorden") << "\n";
        cout << "==========================================================\n";
        AVL t(sucesor);
 
        for (int v : inserciones) t.insertar(v);
        cout << "\n--- Arbol tras todas las inserciones ---\n";
        t.imprimir();
        cout << "Preorden: "; t.preorden();
        cout << "Inorden : "; t.inorden();
        t.porNiveles();
 
        for (int v : eliminaciones) {
            t.eliminar(v);
            cout << "\n--- Tras eliminar " << v << " ---\n";
            t.imprimir();
        }
        cout << "\n--- Arbol FINAL tras todas las eliminaciones ---\n";
        t.imprimir();
        cout << "Preorden: "; t.preorden();
        cout << "Inorden : "; t.inorden();
        t.porNiveles();
        cout << "\n";
    }
    return 0;
}
