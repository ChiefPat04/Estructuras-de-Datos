#include <iostream>
#include <unordered_map>
#include <list>
#include <stack>
#include <limits>
#include <vector>

using namespace std;

// Clase template para representar un Grafo genérico
template <typename T>
class Grafo {
private:
    // Lista de adyacencia: Mapa de nodos con sus listas de adyacencia (destino, peso)
    unordered_map<T, list<pair<T, int>>> listaAdyacencia;
    bool esDirigido;

public:
    // Constructor
    Grafo(bool dirigido = false) : esDirigido(dirigido) {}

    // Función para agregar una arista
    void agregarArista(T origen, T destino, int peso) {
        listaAdyacencia[origen].emplace_back(destino, peso);
        if (!esDirigido) {
            listaAdyacencia[destino].emplace_back(origen, peso);
        }
    }

    // Imprimir el grafo
    void imprimirGrafo() const {
        for (const auto& nodo : listaAdyacencia) {
            cout << nodo.first << " -> ";
            for (const auto& adyacente : nodo.second) {
                cout << "(" << adyacente.first << ", peso: " << adyacente.second << ") ";
            }
            cout << endl;
        }
    }

    
    // DFS: Recorrido en Profundidad Primero
    void dfs(T inicio) const {
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

                
                for (const auto& vecino : listaAdyacencia.at(nodo)) {
                    if (!visitado[vecino.first]) {
                            s.push(vecino.first);
                    }
                }
                
            }
        }
        cout << endl;
    }
};

int main() {
    
}
