// pruebaABB.cpp - pruebas del ABB (tiene su propio main, se compila aparte)

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "ABB.h"

static int pasadas = 0;
static int falladas = 0;

// Registra el resultado de una comprobación.
static void verificar(bool condicion, const std::string& descripcion) {
    if (condicion) {
        pasadas++;
        std::cout << "  [OK]    " << descripcion << "\n";
    } else {
        falladas++;
        std::cout << "  [FALLO] " << descripcion << "\n";
    }
}

typedef ABB<int, std::string> Arbol;
typedef std::vector<std::pair<int, std::string> > Lista;

// Convierte un recorrido a texto: "20 30 40" para poder compararlo.
static std::string aTexto(const Lista& recorrido) {
    std::ostringstream salida;
    for (std::size_t i = 0; i < recorrido.size(); i++) {
        if (i > 0) salida << " ";
        salida << recorrido[i].first;
    }
    return salida.str();
}

// Arma el árbol de ejemplo:        50
//                                30    70
//                              20 40  60 80
static void armarEjemplo(Arbol& a) {
    int claves[] = {50, 30, 70, 20, 40, 60, 80};
    for (int i = 0; i < 7; i++) {
        // Armamos el texto "n50", "n30", ... con ostringstream porque
        // std::to_string no existe en algunos MinGW antiguos (6.x).
        std::ostringstream nombre;
        nombre << "n" << claves[i];
        a.insertar(claves[i], nombre.str());
    }
}

static void pruebaVacio() {
    std::cout << "\n== Arbol vacio ==\n";
    Arbol a;
    verificar(a.vacio(), "un arbol nuevo esta vacio");
    verificar(a.contar() == 0, "contar() = 0");
    verificar(a.altura() == 0, "altura() = 0");
    verificar(a.buscar(10) == nullptr, "buscar en vacio devuelve nullptr");
    verificar(a.minimo() == nullptr, "minimo en vacio devuelve nullptr");
    verificar(a.maximo() == nullptr, "maximo en vacio devuelve nullptr");
    verificar(!a.eliminar(10), "eliminar en vacio devuelve false");
    verificar(a.inorden().empty(), "inorden de vacio es lista vacia");
}

static void pruebaInsertarYRecorridos() {
    std::cout << "\n== Insertar y recorridos ==\n";
    Arbol a;
    armarEjemplo(a);
    verificar(a.contar() == 7, "contar() = 7");
    verificar(aTexto(a.inorden()) == "20 30 40 50 60 70 80",
              "inorden sale ORDENADO: 20 30 40 50 60 70 80");
    verificar(aTexto(a.preorden()) == "50 30 20 40 70 60 80",
              "preorden: 50 30 20 40 70 60 80");
    verificar(aTexto(a.postorden()) == "20 40 30 60 80 70 50",
              "postorden: 20 40 30 60 80 70 50");
    verificar(a.altura() == 3, "altura() = 3 niveles");
    verificar(!a.insertar(40, "repetido"), "insertar clave repetida devuelve false");
    verificar(a.contar() == 7, "la repetida no cambio el contador");
    verificar(*a.buscar(40) == "n40", "la repetida NO piso el valor original");
}

static void pruebaBuscarMinMax() {
    std::cout << "\n== Buscar, minimo y maximo ==\n";
    Arbol a;
    armarEjemplo(a);
    verificar(a.buscar(60) != nullptr && *a.buscar(60) == "n60", "buscar(60) encuentra n60");
    verificar(a.buscar(99) == nullptr, "buscar(99) no existe -> nullptr");
    verificar(a.contiene(20) && !a.contiene(25), "contiene(20) true, contiene(25) false");
    verificar(*a.minimo() == "n20", "minimo es n20");
    verificar(*a.maximo() == "n80", "maximo es n80");
    *a.buscar(60) = "modificado";
    verificar(*a.buscar(60) == "modificado", "el puntero devuelto permite modificar el valor");
}

static void pruebaEliminar() {
    std::cout << "\n== Eliminar: los 3 casos ==\n";
    Arbol a;
    armarEjemplo(a);

    // CASO 1: hoja
    verificar(a.eliminar(20), "eliminar hoja 20 devuelve true");
    verificar(aTexto(a.inorden()) == "30 40 50 60 70 80", "inorden sigue ordenado sin 20");
    verificar(a.contar() == 6, "contar() = 6");

    // CASO 2: un hijo (ahora el 30 solo tiene el hijo 40)
    verificar(a.eliminar(30), "eliminar 30 (un hijo) devuelve true");
    verificar(aTexto(a.preorden()) == "50 40 70 60 80", "preorden: el 40 subio a ocupar el lugar del 30");
    verificar(a.contar() == 5, "contar() = 5");

    // CASO 3: dos hijos y es la RAIZ
    verificar(a.eliminar(50), "eliminar raiz 50 (dos hijos) devuelve true");
    verificar(aTexto(a.preorden()) == "60 40 70 80", "la nueva raiz es 60 (sucesor inorden)");
    verificar(aTexto(a.inorden()) == "40 60 70 80", "inorden sigue ordenado");
    verificar(a.contar() == 4, "contar() = 4");

    // No existe
    verificar(!a.eliminar(999), "eliminar clave inexistente devuelve false");
    verificar(a.contar() == 4, "contar() no cambio");

    // Borrar todo uno por uno
    a.eliminar(60); a.eliminar(40); a.eliminar(70); a.eliminar(80);
    verificar(a.vacio() && a.contar() == 0 && a.altura() == 0,
              "tras borrar todo: vacio, contar 0, altura 0");
    verificar(a.insertar(5, "x") && a.contar() == 1, "se puede volver a insertar despues de vaciar");
}

static void pruebaVaciar() {
    std::cout << "\n== vaciar() ==\n";
    Arbol a;
    armarEjemplo(a);
    a.vaciar();
    verificar(a.vacio() && a.contar() == 0, "vaciar() deja el arbol vacio");
    armarEjemplo(a);
    verificar(a.contar() == 7, "se puede reutilizar tras vaciar()");
}

// Inserta las claves en orden y devuelve el preorden resultante.
static std::string preordenDe(const int* claves, int n) {
    Arbol a;
    for (int i = 0; i < n; i++) a.insertar(claves[i], "x");
    return aTexto(a.preorden());
}

static void pruebaBalanceo() {
    std::cout << "\n== Balanceo AVL: las 4 rotaciones ==\n";
    int ll[] = {30, 20, 10};
    int rr[] = {10, 20, 30};
    int lr[] = {30, 10, 20};
    int rl[] = {10, 30, 20};
    verificar(preordenDe(ll, 3) == "20 10 30", "izquierda-izquierda: rotacion simple a la derecha");
    verificar(preordenDe(rr, 3) == "20 10 30", "derecha-derecha: rotacion simple a la izquierda");
    verificar(preordenDe(lr, 3) == "20 10 30", "izquierda-derecha: rotacion doble");
    verificar(preordenDe(rl, 3) == "20 10 30", "derecha-izquierda: rotacion doble");

    std::cout << "\n== Balanceo AVL: claves en orden ==\n";
    Arbol a;
    for (int i = 1; i <= 10; i++) a.insertar(i, "x");
    verificar(a.altura() == 4, "insertar 1..10 en orden da altura 4 (antes era 10)");
    verificar(a.estaBalanceado(), "el arbol queda balanceado");
    verificar(a.contar() == 10, "contar() = 10");
    Arbol grande;
    for (int i = 1; i <= 1000; i++) grande.insertar(i, "x");
    verificar(grande.altura() == 10, "1000 claves en orden: altura 10 (log2)");
    verificar(grande.estaBalanceado(), "1000 claves en orden: balanceado");
    Arbol bajada;
    for (int i = 1000; i >= 1; i--) bajada.insertar(i, "x");
    verificar(bajada.altura() == 10 && bajada.estaBalanceado(), "1000 claves en orden inverso: altura 10 y balanceado");

    std::cout << "\n== Balanceo AVL: eliminar ==\n";
    Arbol e;
    int claves[] = {2, 1, 3, 4};
    for (int i = 0; i < 4; i++) e.insertar(claves[i], "x");
    verificar(aTexto(e.preorden()) == "2 1 3 4", "arbol de partida: 2 1 3 4");
    e.eliminar(1);
    verificar(aTexto(e.preorden()) == "3 2 4", "al borrar el 1 se rota y la raiz pasa a ser 3");
    verificar(e.estaBalanceado(), "balanceado tras eliminar");
    for (int i = 1; i <= 1000; i += 2) grande.eliminar(i);
    verificar(grande.estaBalanceado() && grande.contar() == 500, "borrar los impares de 1..1000 mantiene el balance");
    for (int i = 2; i <= 600; i += 2) grande.eliminar(i);
    verificar(grande.estaBalanceado() && grande.contar() == 200, "seguir borrando mantiene el balance");
}

static void pruebaMuchos() {
    std::cout << "\n== 500 claves pseudoaleatorias ==\n";
    Arbol a;
    unsigned int semilla = 12345;
    int insertadas = 0;
    for (int i = 0; i < 500; i++) {
        semilla = semilla * 1103515245u + 12345u;     // generador simple
        int clave = static_cast<int>((semilla / 65536u) % 1000u);
        if (a.insertar(clave, "v")) insertadas++;
    }
    Lista orden = a.inorden();
    bool ordenado = true;
    for (std::size_t i = 1; i < orden.size(); i++) {
        if (!(orden[i - 1].first < orden[i].first)) ordenado = false;
    }
    verificar(a.contar() == insertadas, "contar() coincide con las insertadas");
    verificar(static_cast<int>(orden.size()) == insertadas, "inorden tiene todas las claves");
    verificar(ordenado, "inorden esta estrictamente ordenado (sin repetidas)");
    verificar(a.estaBalanceado(), "el arbol esta balanceado tras insertar");

    // Borrar la mitad y volver a comprobar la regla de orden
    for (int c = 0; c < 1000; c += 2) a.eliminar(c);
    orden = a.inorden();
    ordenado = true;
    bool hayPares = false;
    for (std::size_t i = 0; i < orden.size(); i++) {
        if (orden[i].first % 2 == 0) hayPares = true;
        if (i > 0 && !(orden[i - 1].first < orden[i].first)) ordenado = false;
    }
    verificar(!hayPares, "tras borrar los pares no queda ninguno");
    verificar(ordenado, "el arbol sigue ordenado tras muchas eliminaciones");
    verificar(a.contar() == static_cast<int>(orden.size()), "contar() sigue coincidiendo");
    verificar(a.estaBalanceado(), "el arbol sigue balanceado tras eliminar");
}

static void pruebaMostrarYConst() {
    std::cout << "\n== mostrar y minimo/maximo en objeto const ==\n";
    Arbol a;
    a.insertar(50, "a");
    a.insertar(30, "b");
    a.insertar(70, "c");
    std::ostringstream salida;
    a.mostrar(salida);
    verificar(salida.str() == "      70\n50\n      30\n", "mostrar dibuja el árbol de lado");
    const Arbol& constante = a;
    verificar(constante.minimo() != nullptr && *constante.minimo() == "b", "minimo() const");
    verificar(constante.maximo() != nullptr && *constante.maximo() == "c", "maximo() const");
    Arbol vacio;
    const Arbol& cv = vacio;
    verificar(cv.minimo() == nullptr && cv.maximo() == nullptr, "minimo/maximo const en árbol vacío");
}

int main() {
    std::cout << "=== PRUEBAS DEL ABB ===\n";
    pruebaVacio();
    pruebaInsertarYRecorridos();
    pruebaBuscarMinMax();
    pruebaEliminar();
    pruebaVaciar();
    pruebaBalanceo();
    pruebaMuchos();
    pruebaMostrarYConst();

    std::cout << "\n=============================\n";
    std::cout << "Pasaron: " << pasadas << "   Fallaron: " << falladas << "\n";
    if (falladas == 0) {
        std::cout << "TODO CORRECTO\n";
    } else {
        std::cout << "HAY FALLOS: revise las lineas [FALLO]\n";
    }
    return (falladas == 0) ? 0 : 1;
}
