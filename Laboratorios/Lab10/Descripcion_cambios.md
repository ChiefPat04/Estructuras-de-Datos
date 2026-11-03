# Laboratorio Grafos 1 — Revisión de `Grafos1.cpp`

## ¿Es correcto el código compartido?

**No.** Al ejecutar el código original con las aristas del grafo dirigido (A->B, A->C, B->D, C->D, D->F) ocurre lo siguiente:

| Problema | Qué se observa | Causa |
|---|---|---|
| 1. El DFS termina el programa con error | `terminate called after throwing an instance of 'std::out_of_range'` | `listaAdyacencia.at(nodo)` lanza una excepción cuando el nodo no tiene entrada en el mapa. En el grafo dirigido, **F** solo aparece como destino, así que nunca se crea su entrada. |
| 2. El nodo F no aparece al imprimir | La impresión solo muestra A, B, C y D | Mismo motivo: en un grafo dirigido solo se registraba el nodo **origen** de cada arista. |
| 3. La impresión sale en un orden cualquiera | Salió `D, C, B, A` | `unordered_map` no garantiza ningún orden al recorrerlo. |
| 4. El DFS visita los vecinos al revés | Desde A visitaría C antes que B | La pila es LIFO: si se apilan B y C en ese orden, sale primero C. (El recorrido seguía siendo un DFS válido, pero no seguía el orden de inserción.) |
| 5. No existe recorrido en anchura | No se puede cumplir los puntos 4 y 8 | La clase no tenía método `bfs`, ni se incluía `<queue>`. |
| 6. `main` vacío | No se ejecuta nada | Hubo que escribir la prueba solicitada. |

En el grafo no dirigido el error 1 no aparece (cada nodo queda registrado por la arista inversa), pero los puntos 3, 4 y 5 sí aplican.

## Cambios efectuados (`Grafos1_corregido.cpp`)

Todos los cambios están marcados en el código con un comentario `// CAMBIO:`.

1. **Nuevo método privado `agregarNodo`** y llamada a él para el origen y el destino en `agregarArista`. Así todo nodo existe en la lista de adyacencia aunque no tenga aristas salientes (F queda como `F -> `).
2. **Vector `nodos`** con el orden en que aparecen los vértices; `imprimirGrafo` lo usa para imprimir siempre en el mismo orden. También indica si el grafo es dirigido o no.
3. **`dfs`**:
   - usa `find()` en lugar de `at()` para no lanzar excepciones;
   - valida que el nodo inicial exista;
   - apila los vecinos en orden inverso (`rbegin`/`rend`) para que se visiten en el orden en que se insertaron las aristas.
4. **Nuevo método `bfs`** con `std::queue` (se agregó `#include <queue>`). Marca cada nodo como visitado al encolarlo para no repetirlo.
5. **`main`** implementa los 8 puntos del enunciado usando solo los métodos de la clase `Grafo`.

## Resultados obtenidos

**Grafo dirigido** (A->B 5, A->C 10, B->D 20, C->D 1, D->F 2)

```
A -> (B, peso: 5) (C, peso: 10)
B -> (D, peso: 20)
C -> (D, peso: 1)
D -> (F, peso: 2)
F ->
Recorrido DFS: A B D F C
Recorrido BFS: A B C D F
```

**Grafo no dirigido** (1-2 4, 1-3 3, 2-4 7, 3-4 1, 4-5 2)

```
1 -> (2, peso: 4) (3, peso: 3)
2 -> (1, peso: 4) (4, peso: 7)
3 -> (1, peso: 3) (4, peso: 1)
4 -> (2, peso: 7) (3, peso: 1) (5, peso: 2)
5 -> (4, peso: 2)
Recorrido DFS: 1 2 4 3 5
Recorrido BFS: 1 2 3 4 5
```

### Verificación manual

- **DFS dirigido desde A:** A → B → D → F (sin salidas), se regresa y queda C (su vecino D ya fue visitado). Resultado: `A B D F C`.
- **BFS dirigido desde A:** nivel 0: A; nivel 1: B, C; nivel 2: D; nivel 3: F. Resultado: `A B C D F`.
- **DFS no dirigido desde 1:** 1 → 2 → 4 → 3 (1 y 4 ya visitados), se regresa a 4 y se visita 5. Resultado: `1 2 4 3 5`.
- **BFS no dirigido desde 1:** nivel 0: 1; nivel 1: 2, 3; nivel 2: 4; nivel 3: 5. Resultado: `1 2 3 4 5`.

## Compilación

```
g++ -std=c++17 Grafos1_corregido.cpp -o grafos
./grafos
```
