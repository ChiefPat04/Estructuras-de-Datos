// AVLTree.h - arbol AVL generico (plantilla).
// Base tomada de ABB.h del Proyecto1 (ya probado) y adaptada al Proyecto II.
// Para cada nodo: el subarbol izquierdo y el derecho difieren en altura
// como maximo en 1. Si no, se corrige con rotaciones.
// En el proyecto se usa para administrar las paginas fisicas libres
// (clave = numero de pagina fisica, valor = informacion de la pagina).
// Esta en el .h porque las plantillas se compilan al usarlas.

#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <cstddef>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

template <typename K, typename V>
class AVLTree {
private:
    struct Nodo {
        K clave;
        V valor;
        Nodo* izq;
        Nodo* der;
        int alt;   // altura en niveles del subarbol (una hoja vale 1)
        Nodo(const K& c, const V& v)
            : clave(c), valor(v), izq(nullptr), der(nullptr), alt(1) {}
    };

    Nodo* raiz_;
    int cantidad_;

    // ---- Balanceo AVL ----

    int alturaNodo(const Nodo* n) const { return (n == nullptr) ? 0 : n->alt; }

    // Recalcula la altura de un nodo a partir de sus hijos.
    void actualizarAltura(Nodo* n) {
        int hi = alturaNodo(n->izq);
        int hd = alturaNodo(n->der);
        n->alt = 1 + (hi > hd ? hi : hd);
    }

    // Positivo: pesa mas la izquierda. Negativo: pesa mas la derecha.
    int factorBalance(const Nodo* n) const {
        return alturaNodo(n->izq) - alturaNodo(n->der);
    }

    // El hijo izquierdo sube y el nodo pasa a ser su hijo derecho.
    Nodo* rotarDerecha(Nodo* n) {
        Nodo* nueva = n->izq;
        n->izq = nueva->der;
        nueva->der = n;
        actualizarAltura(n);
        actualizarAltura(nueva);
        return nueva;
    }

    // El hijo derecho sube y el nodo pasa a ser su hijo izquierdo.
    Nodo* rotarIzquierda(Nodo* n) {
        Nodo* nueva = n->der;
        n->der = nueva->izq;
        nueva->izq = n;
        actualizarAltura(n);
        actualizarAltura(nueva);
        return nueva;
    }

    // Deja el subarbol balanceado y devuelve su nueva raiz.
    Nodo* balancear(Nodo* n) {
        actualizarAltura(n);
        int f = factorBalance(n);
        if (f > 1) {
            // Caso izquierda-derecha: primero se rota el hijo izquierdo
            if (factorBalance(n->izq) < 0) n->izq = rotarIzquierda(n->izq);
            return rotarDerecha(n);
        }
        if (f < -1) {
            // Caso derecha-izquierda: primero se rota el hijo derecho
            if (factorBalance(n->der) > 0) n->der = rotarDerecha(n->der);
            return rotarIzquierda(n);
        }
        return n;
    }

    // Devuelve la raiz del subarbol actualizada; 'insertado' indica si agrego.
    Nodo* insertarRec(Nodo* nodo, const K& clave, const V& valor,
                      bool& insertado) {
        if (nodo == nullptr) {
            insertado = true;
            return new Nodo(clave, valor);
        }
        if (clave < nodo->clave) {
            nodo->izq = insertarRec(nodo->izq, clave, valor, insertado);
        } else if (nodo->clave < clave) {
            nodo->der = insertarRec(nodo->der, clave, valor, insertado);
        }
        return balancear(nodo);   // si la clave era repetida no cambia nada
    }

    Nodo* buscarNodo(const K& clave) const {
        Nodo* actual = raiz_;
        while (actual != nullptr) {
            if (clave < actual->clave) {
                actual = actual->izq;
            } else if (actual->clave < clave) {
                actual = actual->der;
            } else {
                return actual;
            }
        }
        return nullptr;
    }

    Nodo* minimoNodo(Nodo* nodo) const {
        while (nodo->izq != nullptr) nodo = nodo->izq;
        return nodo;
    }

    Nodo* maximoNodo(Nodo* nodo) const {
        while (nodo->der != nullptr) nodo = nodo->der;
        return nodo;
    }

    Nodo* eliminarRec(Nodo* nodo, const K& clave, bool& eliminado) {
        if (nodo == nullptr) return nullptr;

        if (clave < nodo->clave) {
            nodo->izq = eliminarRec(nodo->izq, clave, eliminado);
        } else if (nodo->clave < clave) {
            nodo->der = eliminarRec(nodo->der, clave, eliminado);
        } else {
            eliminado = true;

            // Caso 1: hoja
            if (nodo->izq == nullptr && nodo->der == nullptr) {
                delete nodo;
                return nullptr;
            }
            // Caso 2: un solo hijo
            if (nodo->izq == nullptr) {
                Nodo* hijo = nodo->der;
                delete nodo;
                return hijo;
            }
            if (nodo->der == nullptr) {
                Nodo* hijo = nodo->izq;
                delete nodo;
                return hijo;
            }
            // Caso 3: dos hijos. Se reemplaza por el sucesor inorden
            // (el menor del subarbol derecho) y se borra ese nodo.
            Nodo* sucesor = minimoNodo(nodo->der);
            K claveSucesor = sucesor->clave;
            nodo->clave = sucesor->clave;
            nodo->valor = sucesor->valor;
            bool ignorar = false;
            nodo->der = eliminarRec(nodo->der, claveSucesor, ignorar);
        }
        return balancear(nodo);
    }

    // Postorden: los hijos se borran antes que el padre.
    void destruirRec(Nodo* nodo) {
        if (nodo == nullptr) return;
        destruirRec(nodo->izq);
        destruirRec(nodo->der);
        delete nodo;
    }

    // Revisa que cada altura guardada sea correcta y que ningun nodo
    // tenga diferencia mayor a 1 entre sus lados (se usa en las pruebas).
    bool balanceadoRec(const Nodo* n) const {
        if (n == nullptr) return true;
        int hi = alturaNodo(n->izq);
        int hd = alturaNodo(n->der);
        int dif = hi - hd;
        if (dif > 1 || dif < -1) return false;
        if (n->alt != 1 + (hi > hd ? hi : hd)) return false;
        return balanceadoRec(n->izq) && balanceadoRec(n->der);
    }

    void inOrdenRec(const Nodo* n, std::vector<std::pair<K, V> >& salida) const {
        if (n == nullptr) return;
        inOrdenRec(n->izq, salida);
        salida.push_back(std::make_pair(n->clave, n->valor));
        inOrdenRec(n->der, salida);
    }

    // Dibuja el arbol de lado: la raiz a la izquierda, el hijo derecho arriba.
    void mostrarRec(std::ostream& salida, const Nodo* n, int nivel) const {
        if (n == nullptr) return;
        mostrarRec(salida, n->der, nivel + 1);
        salida << std::string(static_cast<std::size_t>(nivel * 6), ' ')
               << n->clave << "\n";
        mostrarRec(salida, n->izq, nivel + 1);
    }

public:
    AVLTree() : raiz_(nullptr), cantidad_(0) {}
    ~AVLTree() { destruirRec(raiz_); }

    // No se puede copiar: dos arboles compartirian los mismos nodos.
    AVLTree(const AVLTree&) = delete;
    AVLTree& operator=(const AVLTree&) = delete;

    // Devuelve false si la clave ya existia.
    bool insertar(const K& clave, const V& valor) {
        bool insertado = false;
        raiz_ = insertarRec(raiz_, clave, valor, insertado);
        if (insertado) cantidad_++;
        return insertado;
    }

    // Devuelve un puntero al valor, o nullptr si la clave no existe.
    V* buscar(const K& clave) {
        Nodo* n = buscarNodo(clave);
        return (n == nullptr) ? nullptr : &(n->valor);
    }
    const V* buscar(const K& clave) const {
        const Nodo* n = buscarNodo(clave);
        return (n == nullptr) ? nullptr : &(n->valor);
    }

    bool contiene(const K& clave) const { return buscarNodo(clave) != nullptr; }

    // Devuelve false si la clave no existia.
    bool eliminar(const K& clave) {
        bool eliminado = false;
        raiz_ = eliminarRec(raiz_, clave, eliminado);
        if (eliminado) cantidad_--;
        return eliminado;
    }

    // Saca la clave mas pequena (la pagina fisica libre mas baja).
    // Devuelve false si el arbol esta vacio.
    bool extraerMinimo(K& claveSalida, V& valorSalida) {
        if (raiz_ == nullptr) return false;
        Nodo* m = minimoNodo(raiz_);
        claveSalida = m->clave;
        valorSalida = m->valor;
        return eliminar(claveSalida);
    }

    // nullptr si el arbol esta vacio
    V* minimo() {
        return (raiz_ == nullptr) ? nullptr : &(minimoNodo(raiz_)->valor);
    }
    V* maximo() {
        return (raiz_ == nullptr) ? nullptr : &(maximoNodo(raiz_)->valor);
    }

    // Altura en niveles: vacio = 0, solo la raiz = 1.
    int altura() const { return alturaNodo(raiz_); }
    bool estaBalanceado() const { return balanceadoRec(raiz_); }
    int contar() const { return cantidad_; }
    bool vacio() const { return raiz_ == nullptr; }

    void vaciar() {
        destruirRec(raiz_);
        raiz_ = nullptr;
        cantidad_ = 0;
    }

    void mostrar(std::ostream& salida) const { mostrarRec(salida, raiz_, 0); }

    // Devuelve los pares (clave, valor) ordenados por clave.
    std::vector<std::pair<K, V> > inOrden() const {
        std::vector<std::pair<K, V> > salida;
        inOrdenRec(raiz_, salida);
        return salida;
    }
};

#endif
