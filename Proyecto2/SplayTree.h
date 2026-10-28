// SplayTree.h - arbol Splay generico (plantilla).
// Es un arbol binario de busqueda que, cada vez que consultas una clave,
// la sube a la raiz con rotaciones (splay). Asi, las claves que se usan
// seguido quedan cerca de la raiz y consultarlas es rapido.
// En el proyecto se usa como tabla de paginas de cada proceso
// (clave = numero de pagina virtual, valor = pagina asignada).
// Esta en el .h porque las plantillas se compilan al usarlas.
//
// Solo se usan los operadores '<' de la clave. Dos claves son "iguales"
// si ninguna es menor que la otra.

#ifndef SPLAY_TREE_H
#define SPLAY_TREE_H

#include <cstddef>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

template <typename K, typename V>
class SplayTree {
private:
    struct Nodo {
        K clave;
        V valor;
        Nodo* izq;
        Nodo* der;
        Nodo(const K& c, const V& v)
            : clave(c), valor(v), izq(nullptr), der(nullptr) {}
    };

    Nodo* raiz_;
    int cantidad_;

    // Dos claves son iguales si ninguna es menor que la otra.
    static bool iguales(const K& a, const K& b) {
        return !(a < b) && !(b < a);
    }

    // ---- Rotaciones ----

    // Zig: rotacion simple. Si 'haciaDerecha' es true, el hijo izquierdo sube;
    // si no, sube el hijo derecho. Devuelve la nueva raiz de ese subarbol.
    static Nodo* rotacionZig(Nodo* n, bool haciaDerecha) {
        if (haciaDerecha) {
            Nodo* nueva = n->izq;
            n->izq = nueva->der;
            nueva->der = n;
            return nueva;
        }
        Nodo* nueva = n->der;
        n->der = nueva->izq;
        nueva->izq = n;
        return nueva;
    }

    // Zig-zig: dos rotaciones en la misma direccion.
    // Primero rota el abuelo y despues el padre, asi la clave llega a la raiz.
    static Nodo* rotacionZigZig(Nodo* abuelo, bool haciaDerecha) {
        Nodo* padre = rotacionZig(abuelo, haciaDerecha);
        return rotacionZig(padre, haciaDerecha);
    }

    // Zig-zag: el hijo y el nieto estan en direcciones contrarias.
    // Primero rota el hijo hacia el lado contrario y despues el abuelo.
    // 'hijoEsIzquierdo' dice si el hijo del abuelo es el izquierdo.
    static Nodo* rotacionZigZag(Nodo* abuelo, bool hijoEsIzquierdo) {
        if (hijoEsIzquierdo) {
            abuelo->izq = rotacionZig(abuelo->izq, false);
            return rotacionZig(abuelo, true);
        }
        abuelo->der = rotacionZig(abuelo->der, true);
        return rotacionZig(abuelo, false);
    }

    // Lleva la clave (si existe) a la raiz del subarbol 'n'.
    // Si no existe, deja en la raiz el nodo mas cercano a la clave.
    Nodo* splayRec(Nodo* n, const K& clave) {
        if (n == nullptr) {
            return nullptr;
        }

        if (clave < n->clave) {
            if (n->izq == nullptr) {
                return n;                       // no esta: queda n
            }
            if (clave < n->izq->clave) {        // izquierda-izquierda
                if (n->izq->izq != nullptr) {
                    n->izq->izq = splayRec(n->izq->izq, clave);
                    return rotacionZigZig(n, true);
                }
                return rotacionZig(n, true);
            }
            if (n->izq->clave < clave) {        // izquierda-derecha
                if (n->izq->der != nullptr) {
                    n->izq->der = splayRec(n->izq->der, clave);
                    return rotacionZigZag(n, true);
                }
                return rotacionZig(n, true);
            }
            return rotacionZig(n, true);        // el hijo es la clave
        }

        if (n->clave < clave) {
            if (n->der == nullptr) {
                return n;
            }
            if (n->der->clave < clave) {        // derecha-derecha
                if (n->der->der != nullptr) {
                    n->der->der = splayRec(n->der->der, clave);
                    return rotacionZigZig(n, false);
                }
                return rotacionZig(n, false);
            }
            if (clave < n->der->clave) {        // derecha-izquierda
                if (n->der->izq != nullptr) {
                    n->der->izq = splayRec(n->der->izq, clave);
                    return rotacionZigZag(n, false);
                }
                return rotacionZig(n, false);
            }
            return rotacionZig(n, false);       // el hijo es la clave
        }

        return n;                               // es la clave
    }

    void inOrdenRec(const Nodo* n, std::vector<std::pair<K, V> >& salida) const {
        if (n == nullptr) return;
        inOrdenRec(n->izq, salida);
        salida.push_back(std::make_pair(n->clave, n->valor));
        inOrdenRec(n->der, salida);
    }

    // Postorden: los hijos se borran antes que el padre.
    void destruirRec(Nodo* n) {
        if (n == nullptr) return;
        destruirRec(n->izq);
        destruirRec(n->der);
        delete n;
    }

    int alturaRec(const Nodo* n) const {
        if (n == nullptr) return 0;
        int hi = alturaRec(n->izq);
        int hd = alturaRec(n->der);
        return 1 + (hi > hd ? hi : hd);
    }

public:
    SplayTree() : raiz_(nullptr), cantidad_(0) {}
    ~SplayTree() { destruirRec(raiz_); }

    // No se puede copiar: dos arboles compartirian los mismos nodos.
    SplayTree(const SplayTree&) = delete;
    SplayTree& operator=(const SplayTree&) = delete;

    // Hace splay de la clave (la sube a la raiz). Si no existe, sube la mas cercana.
    void splay(const K& clave) {
        raiz_ = splayRec(raiz_, clave);
    }

    // Inserta la clave. Devuelve false si ya existia.
    bool insertar(const K& clave, const V& valor) {
        if (raiz_ == nullptr) {
            raiz_ = new Nodo(clave, valor);
            cantidad_++;
            return true;
        }

        raiz_ = splayRec(raiz_, clave);
        if (iguales(raiz_->clave, clave)) {
            return false;
        }

        // El nuevo nodo queda como raiz, y la raiz anterior baja a un lado.
        Nodo* nuevo = new Nodo(clave, valor);
        if (clave < raiz_->clave) {
            nuevo->izq = raiz_->izq;
            nuevo->der = raiz_;
            raiz_->izq = nullptr;
        } else {
            nuevo->der = raiz_->der;
            nuevo->izq = raiz_;
            raiz_->der = nullptr;
        }
        raiz_ = nuevo;
        cantidad_++;
        return true;
    }

    // Busca la clave y la sube a la raiz. Devuelve un puntero al valor,
    // o nullptr si no existe.
    V* buscar(const K& clave) {
        raiz_ = splayRec(raiz_, clave);
        if (raiz_ != nullptr && iguales(raiz_->clave, clave)) {
            return &(raiz_->valor);
        }
        return nullptr;
    }

    // Dice si la clave existe (tambien la sube a la raiz).
    bool contiene(const K& clave) {
        return buscar(clave) != nullptr;
    }

    // Elimina la clave. Devuelve false si no existia.
    bool eliminar(const K& clave) {
        if (raiz_ == nullptr) {
            return false;
        }
        raiz_ = splayRec(raiz_, clave);
        if (!iguales(raiz_->clave, clave)) {
            return false;
        }

        Nodo* viejo = raiz_;
        if (viejo->izq == nullptr) {
            raiz_ = viejo->der;
        } else {
            // Subimos el mayor de la izquierda (todas sus claves son menores)
            // y le pegamos el subarbol derecho del nodo borrado.
            Nodo* nuevaRaiz = splayRec(viejo->izq, clave);
            nuevaRaiz->der = viejo->der;
            raiz_ = nuevaRaiz;
        }
        delete viejo;
        cantidad_--;
        return true;
    }

    // Dice si la clave esta justo en la raiz (sirve para comprobar el splay).
    bool estaEnRaiz(const K& clave) const {
        return raiz_ != nullptr && iguales(raiz_->clave, clave);
    }

    // Devuelve los pares (clave, valor) ordenados por clave.
    // No hace splay, asi que no cambia el arbol.
    std::vector<std::pair<K, V> > recorrido() const {
        std::vector<std::pair<K, V> > salida;
        inOrdenRec(raiz_, salida);
        return salida;
    }

    int altura() const { return alturaRec(raiz_); }
    int contar() const { return cantidad_; }
    bool vacio() const { return raiz_ == nullptr; }

    void vaciar() {
        destruirRec(raiz_);
        raiz_ = nullptr;
        cantidad_ = 0;
    }
};

#endif
