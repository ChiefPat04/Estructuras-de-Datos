#include <iostream>
#include <unordered_map>
#include <list>
#include <stack>
#include <queue>      // CAMBIO: necesario para el recorrido en anchura (BFS)
#include <limits>
#include <vector>

using namespace std;

// Clase template para representar un Grafo genérico
template <typename T>
class Grafo {
private:
    // Lista de adyacencia: Mapa de nodos con sus listas de adyacencia (destino, peso)
    unordered_map<T, list<pair<T, int>>> listaAdyacencia;
    // CAMBIO: orden en que aparecen los nodos, para imprimir de forma predecible
    // (unordered_map no garantiza ningún orden al recorrerlo).
    vector<T> nodos;
    bool esDirigido;

    // CAMBIO: registra un nodo aunque no tenga aristas salientes.
    void agregarNodo(const T& nodo) {
        if (listaAdyacencia.find(nodo) == listaAdyacencia.end()) {
            listaAdyacencia[nodo];          // crea su lista vacía
            nodos.push_back(nodo);
        }
    }

public:
    // Constructor
    Grafo(bool dirigido = false) : esDirigido(dirigido) {}

    // Función para agregar una arista
    void agregarArista(T origen, T destino, int peso) {
        // CAMBIO: ambos extremos quedan registrados como nodos. Antes, en un
        // grafo dirigido, un destino sin aristas salientes (p. ej. F) no
        // existía en el mapa: no se imprimía y hacía fallar el DFS.
        agregarNodo(origen);
        agregarNodo(destino);
        listaAdyacencia[origen].emplace_back(destino, peso);
        if (!esDirigido) {
            listaAdyacencia[destino].emplace_back(origen, peso);
        }
    }

    // Imprimir el grafo
    void imprimirGrafo() const {
        cout << "Grafo " << (esDirigido ? "dirigido" : "no dirigido") << ":" << endl;
        // CAMBIO: se recorre en orden de aparición de los nodos.
        for (const auto& nodo : nodos) {
            cout << nodo << " -> ";
            for (const auto& adyacente : listaAdyacencia.at(nodo)) {
                cout << "(" << adyacente.first << ", peso: " << adyacente.second << ") ";
            }
            cout << endl;
        }
    }

    // DFS: Recorrido en Profundidad Primero
    void dfs(T inicio) const {
        // CAMBIO: validar que el nodo inicial exista.
        if (listaAdyacencia.find(inicio) == listaAdyacencia.end()) {
            cout << "Recorrido DFS: el nodo " << inicio << " no existe" << endl;
            return;
        }

        unordered_map<T, bool> visitado;
        stack<T> s;

        s.push(inicio);

        cout << "Recorrido DFS: ";
        while (!s.empty()) {
            T nodo = s.top();
            s.pop();

            if (!visitado[nodo]) {
                visitado[nodo] = true;
                cout << nodo << " ";

                // CAMBIO: se usa find() en vez de at(). at() lanzaba
                // std::out_of_range con nodos sin aristas salientes.
                auto it = listaAdyacencia.find(nodo);
                if (it == listaAdyacencia.end()) continue;

                // CAMBIO: los vecinos se apilan en orden inverso para que se
                // visiten en el mismo orden en que se insertaron las aristas
                // (la pila es LIFO). Antes A->B, A->C visitaba C antes que B.
                const auto& vecinos = it->second;
                for (auto v = vecinos.rbegin(); v != vecinos.rend(); ++v) {
                    if (!visitado[v->first]) {
                        s.push(v->first);
                    }
                }
            }
        }
        cout << endl;
    }

    // CAMBIO: BFS (Recorrido en Anchura Primero), no existía en el código original.
    void bfs(T inicio) const {
        if (listaAdyacencia.find(inicio) == listaAdyacencia.end()) {
            cout << "Recorrido BFS: el nodo " << inicio << " no existe" << endl;
            return;
        }

        unordered_map<T, bool> visitado;
        queue<T> q;

        visitado[inicio] = true;     // se marca al encolar para no repetir nodos
        q.push(inicio);

        cout << "Recorrido BFS: ";
        while (!q.empty()) {
            T nodo = q.front();
            q.pop();
            cout << nodo << " ";

            for (const auto& vecino : listaAdyacencia.at(nodo)) {
                if (!visitado[vecino.first]) {
                    visitado[vecino.first] = true;
                    q.push(vecino.first);
                }
            }
        }
        cout << endl;
    }
};

int main() {
    // ---------- Grafo dirigido ----------
    cout << "===== GRAFO DIRIGIDO =====" << endl;
    Grafo<char> dirigido(true);

    // 1. Insertar A->B, A->C, B->D, C->D, D->F con pesos 5, 10, 20, 1, 2
    dirigido.agregarArista('A', 'B', 5);
    dirigido.agregarArista('A', 'C', 10);
    dirigido.agregarArista('B', 'D', 20);
    dirigido.agregarArista('C', 'D', 1);
    dirigido.agregarArista('D', 'F', 2);

    // 2. Imprimir el contenido del grafo
    dirigido.imprimirGrafo();

    // 3. Recorrido en profundidad primero
    dirigido.dfs('A');

    // 4. Recorrido en anchura primero
    dirigido.bfs('A');

    // ---------- Grafo no dirigido ----------
    cout << endl << "===== GRAFO NO DIRIGIDO =====" << endl;
    Grafo<int> noDirigido(false);

    // 5. Insertar 1-2, 1-3, 2-4, 3-4, 4-5 con pesos 4, 3, 7, 1, 2
    noDirigido.agregarArista(1, 2, 4);
    noDirigido.agregarArista(1, 3, 3);
    noDirigido.agregarArista(2, 4, 7);
    noDirigido.agregarArista(3, 4, 1);
    noDirigido.agregarArista(4, 5, 2);

    // 6. Imprimir el contenido del grafo
    noDirigido.imprimirGrafo();

    // 7. Recorrido en profundidad primero
    noDirigido.dfs(1);

    // 8. Recorrido en anchura primero
    noDirigido.bfs(1);

    return 0;
}
