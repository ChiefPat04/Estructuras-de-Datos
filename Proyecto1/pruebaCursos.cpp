// pruebaCursos.cpp - pruebas de ArbolGenerico y GestorCursos (tiene su propio main)
//
// Árbol de ejemplo que usan casi todas las pruebas (id entre corchetes):
//
//   Estructuras de Datos [1]
//     - Unidad 1: Fundamentos [2]
//         - Tema 1.1: Tipos abstractos [3]
//             - PDF de teoria [4]
//             - Video introductorio [5]
//         - Tema 1.2: Complejidad [6]
//             - Quiz [7]
//     - Unidad 2: Estructuras lineales [8]
//         - Tema 2.1: Listas [9]
//         - Tema 2.2: Pilas [10]
//         - Tema 2.3: Colas [11]

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "ArbolGenerico.h"
#include "GestorCursos.h"

static int pasadas = 0;
static int falladas = 0;

static void verificar(bool condicion, const std::string& descripcion) {
    if (condicion) {
        pasadas++;
        std::cout << "  [OK]    " << descripcion << "\n";
    } else {
        falladas++;
        std::cout << "  [FALLO] " << descripcion << "\n";
    }
}

// Convierte una lista de nodos a texto con sus ids: "1 2 3"
static std::string ids(const std::vector<NodoCurso*>& nodos) {
    std::ostringstream salida;
    for (std::size_t i = 0; i < nodos.size(); i++) {
        if (i > 0) salida << " ";
        salida << nodos[i]->getId();
    }
    return salida.str();
}

// Arma el árbol de ejemplo directamente sobre un ArbolGenerico.
static void armarEjemplo(ArbolGenerico& a) {
    NodoCurso* raiz = a.getRaiz();
    NodoCurso* u1 = a.agregarHijo(raiz, 2, TipoNodo::Unidad, "Unidad 1: Fundamentos");
    NodoCurso* t11 = a.agregarHijo(u1, 3, TipoNodo::Tema, "Tema 1.1: Tipos abstractos");
    a.agregarHijo(t11, 4, TipoNodo::Recurso, "PDF de teoria");
    a.agregarHijo(t11, 5, TipoNodo::Recurso, "Video introductorio");
    NodoCurso* t12 = a.agregarHijo(u1, 6, TipoNodo::Tema, "Tema 1.2: Complejidad");
    a.agregarHijo(t12, 7, TipoNodo::Actividad, "Quiz");
    NodoCurso* u2 = a.agregarHijo(raiz, 8, TipoNodo::Unidad, "Unidad 2: Estructuras lineales");
    a.agregarHijo(u2, 9, TipoNodo::Tema, "Tema 2.1: Listas");
    a.agregarHijo(u2, 10, TipoNodo::Tema, "Tema 2.2: Pilas");
    a.agregarHijo(u2, 11, TipoNodo::Tema, "Tema 2.3: Colas");
}

static void pruebaTipos() {
    std::cout << "\n== Tipos de nodo ==\n";
    verificar(nombreTipo(TipoNodo::Actividad) == "Actividad", "nombreTipo(Actividad)");
    verificar(nombreTipo(TipoNodo::Curso) == "Curso", "nombreTipo(Curso)");
    TipoNodo t = TipoNodo::Curso;
    verificar(tipoDesdeTexto("  TEMA ", t) && t == TipoNodo::Tema, "tipoDesdeTexto('  TEMA ') -> Tema");
    verificar(tipoDesdeTexto("recurso", t) && t == TipoNodo::Recurso, "tipoDesdeTexto('recurso') -> Recurso");
    verificar(!tipoDesdeTexto("xyz", t), "tipoDesdeTexto('xyz') -> false");
}

static void pruebaSoloRaiz() {
    std::cout << "\n== Arbol con solo la raiz ==\n";
    ArbolGenerico a(1, "Estructuras de Datos");
    NodoCurso* raiz = a.getRaiz();
    verificar(a.cantidadNodos() == 1, "cantidadNodos = 1");
    verificar(a.altura() == 1, "altura = 1 (un solo nivel)");
    verificar(raiz->getTipo() == TipoNodo::Curso && raiz->getPadre() == nullptr, "la raiz es de tipo Curso y no tiene padre");
    verificar(a.nivel(raiz) == 1, "nivel de la raiz = 1");
    verificar(a.contarDescendientes(raiz) == 0, "la raiz no tiene descendientes");
    verificar(raiz->esHoja(), "la raiz sin hijos es hoja");
    verificar(a.hojas().size() == 1 && a.hojas()[0] == raiz, "hojas() = solo la raiz");
    verificar(a.rutaComoTexto(raiz) == "Estructuras de Datos", "ruta de la raiz = su nombre");
    verificar(a.buscarPorId(1) == raiz && a.buscarPorId(99) == nullptr, "buscarPorId(1) si, buscarPorId(99) no");
    verificar(!a.eliminarHoja(raiz), "la raiz no se elimina con eliminarHoja");
    verificar(a.nivel(nullptr) == 0 && a.contarDescendientes(nullptr) == 0, "nullptr -> nivel 0 y 0 descendientes");
}

static void pruebaEjemplo() {
    std::cout << "\n== Arbol de ejemplo: consultas ==\n";
    ArbolGenerico a(1, "Estructuras de Datos");
    armarEjemplo(a);
    NodoCurso* raiz = a.getRaiz();

    verificar(a.cantidadNodos() == 11, "cantidadNodos = 11");
    verificar(a.altura() == 4, "altura = 4 niveles (curso, unidad, tema, recurso)");
    verificar(ids(a.preorden()) == "1 2 3 4 5 6 7 8 9 10 11", "preorden: 1 2 3 4 5 6 7 8 9 10 11");
    verificar(ids(a.hojas()) == "4 5 7 9 10 11", "hojas: 4 5 7 9 10 11");

    verificar(a.nivel(raiz) == 1, "nivel(curso) = 1");
    verificar(a.nivel(a.buscarPorId(2)) == 2, "nivel(Unidad 1) = 2");
    verificar(a.nivel(a.buscarPorId(3)) == 3, "nivel(Tema 1.1) = 3");
    verificar(a.nivel(a.buscarPorId(4)) == 4, "nivel(PDF) = 4");

    verificar(a.contarDescendientes(raiz) == 10, "descendientes del curso = 10");
    verificar(a.contarDescendientes(a.buscarPorId(2)) == 5, "descendientes de Unidad 1 = 5");
    verificar(a.contarDescendientes(a.buscarPorId(8)) == 3, "descendientes de Unidad 2 = 3");
    verificar(a.contarDescendientes(a.buscarPorId(3)) == 2, "descendientes de Tema 1.1 = 2");
    verificar(a.contarDescendientes(a.buscarPorId(4)) == 0, "descendientes de una hoja = 0");

    verificar(a.rutaComoTexto(a.buscarPorId(4)) ==
              "Estructuras de Datos > Unidad 1: Fundamentos > Tema 1.1: Tipos abstractos > PDF de teoria",
              "ruta hasta el PDF (4 nodos, de la raiz hacia abajo)");
    verificar(a.ruta(a.buscarPorId(4)).size() == 4, "ruta(PDF) tiene 4 nodos");
    verificar(a.ruta(a.buscarPorId(4)).front() == raiz, "la ruta empieza en la raiz");
    verificar(a.rutaComoTexto(a.buscarPorId(8), " / ") == "Estructuras de Datos / Unidad 2: Estructuras lineales",
              "ruta con otro separador");

    verificar(a.buscarPorId(7)->getNombre() == "Quiz" && a.buscarPorId(7)->getTipo() == TipoNodo::Actividad,
              "el nodo 7 es la Actividad Quiz");
}

// Convierte un camino a texto de ids: "4 3 5"
static std::string idsRuta(const std::vector<const NodoCurso*>& camino) {
    std::ostringstream salida;
    for (std::size_t i = 0; i < camino.size(); i++) {
        if (i > 0) salida << " ";
        salida << camino[i]->getId();
    }
    return salida.str();
}

static void pruebaRutaEntre() {
    std::cout << "\n== Ruta entre dos nodos ==\n";
    ArbolGenerico a(1, "Estructuras de Datos");
    armarEjemplo(a);
    NodoCurso* pdf = a.buscarPorId(4);
    NodoCurso* video = a.buscarPorId(5);

    verificar(idsRuta(a.rutaEntre(pdf, video)) == "4 3 5", "hermanos: PDF -> Video pasa por su tema");
    verificar(idsRuta(a.rutaEntre(pdf, a.buscarPorId(7))) == "4 3 2 6 7", "primos: PDF -> Quiz sube a la unidad y baja");
    verificar(idsRuta(a.rutaEntre(pdf, a.buscarPorId(9))) == "4 3 2 1 8 9", "otra rama: PDF -> Listas pasa por el curso");
    verificar(idsRuta(a.rutaEntre(pdf, pdf)) == "4", "mismo nodo: solo él");
    verificar(idsRuta(a.rutaEntre(a.buscarPorId(2), pdf)) == "2 3 4", "ancestro -> descendiente solo baja");
    verificar(idsRuta(a.rutaEntre(pdf, a.buscarPorId(2))) == "4 3 2", "descendiente -> ancestro solo sube");
    verificar(idsRuta(a.rutaEntre(a.getRaiz(), a.buscarPorId(11))) == "1 8 11", "del curso a un tema");
    verificar(a.rutaEntre(nullptr, pdf).empty() && a.rutaEntre(pdf, nullptr).empty(), "nodo nulo: ruta vacía");

    ArbolGenerico otro(100, "Otro curso");
    verificar(a.rutaEntre(pdf, otro.getRaiz()).empty(), "nodo de otro árbol: ruta vacía");

    // Para todos los pares: la inversa es la misma ruta al revés y cada paso es padre-hijo.
    bool coherente = true;
    for (int i = 1; i <= 11; i++) {
        for (int j = 1; j <= 11; j++) {
            std::vector<const NodoCurso*> ida = a.rutaEntre(a.buscarPorId(i), a.buscarPorId(j));
            std::vector<const NodoCurso*> vuelta = a.rutaEntre(a.buscarPorId(j), a.buscarPorId(i));
            std::reverse(vuelta.begin(), vuelta.end());
            if (ida != vuelta || ida.empty()) coherente = false;
            for (std::size_t k = 1; k < ida.size(); k++) {
                bool esPadre = ida[k - 1]->getPadre() == ida[k];
                bool esHijo = ida[k]->getPadre() == ida[k - 1];
                if (!esPadre && !esHijo) coherente = false;
            }
        }
    }
    verificar(coherente, "los 121 pares: la inversa coincide y cada paso une un padre con su hijo");
}

static void pruebaBuscarNombre() {
    std::cout << "\n== Buscar por nombre ==\n";
    ArbolGenerico a(1, "Estructuras de Datos");
    armarEjemplo(a);
    verificar(ids(a.buscarPorNombre("tema 2")) == "9 10 11", "'tema 2' encuentra los 3 temas de la Unidad 2");
    verificar(a.buscarPorNombre("TEMA").size() == 5, "'TEMA' (mayusculas) encuentra 5 temas");
    verificar(ids(a.buscarPorNombre("quiz")) == "7", "'quiz' encuentra el nodo 7");
    verificar(a.buscarPorNombre("zzz").empty(), "'zzz' no encuentra nada");
    verificar(a.buscarPorNombre("").empty() && a.buscarPorNombre("   ").empty(), "texto vacio no devuelve nodos");
    verificar(ids(a.buscarPorNombre("estructuras")) == "1 8", "'estructuras' encuentra el curso y la Unidad 2 (en preorden)");
}

static void pruebaAgregarInvalido() {
    std::cout << "\n== Agregar: casos invalidos ==\n";
    ArbolGenerico a(1, "Curso A");
    ArbolGenerico b(100, "Curso B");
    NodoCurso* u = a.agregarHijo(a.getRaiz(), 2, TipoNodo::Unidad, "  U1  ");
    verificar(u != nullptr && u->getNombre() == "U1", "el nombre se guarda sin espacios sobrantes");
    verificar(u->getPadre() == a.getRaiz(), "el padre del nuevo nodo es la raiz");

    verificar(a.agregarHijo(nullptr, 3, TipoNodo::Tema, "X") == nullptr, "padre nullptr -> rechazado");
    verificar(a.agregarHijo(u, 3, TipoNodo::Tema, "   ") == nullptr, "nombre vacio -> rechazado");
    verificar(a.agregarHijo(u, 3, TipoNodo::Curso, "X") == nullptr, "tipo Curso como hijo -> rechazado");
    verificar(a.agregarHijo(b.getRaiz(), 3, TipoNodo::Tema, "X") == nullptr, "padre de OTRO arbol -> rechazado");
    verificar(a.cantidadNodos() == 2 && b.cantidadNodos() == 1, "los rechazos no cambiaron nada");

    NodoCurso* hijoDeHoja = a.agregarHijo(u, 3, TipoNodo::Tema, "T1");
    NodoCurso* nieto = a.agregarHijo(hijoDeHoja, 4, TipoNodo::Recurso, "R1");
    verificar(nieto != nullptr && a.nivel(nieto) == 4 && a.altura() == 4, "se puede agregar a cualquier nodo (cadena de 4 niveles)");
    verificar(a.agregarHijo(nieto, 5, TipoNodo::Actividad, "Dentro de un recurso") != nullptr,
              "tambien a un Recurso (el enunciado dice 'cualquier nodo')");
    verificar(a.altura() == 5, "la altura sube a 5");
}

static void pruebaEliminar() {
    std::cout << "\n== Eliminar nodos hoja ==\n";
    ArbolGenerico a(1, "Estructuras de Datos");
    armarEjemplo(a);

    verificar(!a.eliminarHoja(a.buscarPorId(2)), "Unidad 1 tiene hijos -> no se elimina");
    verificar(!a.eliminarHoja(a.buscarPorId(3)), "Tema 1.1 tiene hijos -> no se elimina");
    verificar(!a.eliminarHoja(a.getRaiz()), "la raiz no se elimina");
    verificar(!a.eliminarHoja(nullptr), "nullptr -> false");
    verificar(a.cantidadNodos() == 11, "los rechazos no cambiaron nada");

    verificar(a.eliminarHoja(a.buscarPorId(11)), "eliminar la hoja 11 (Tema 2.3)");
    verificar(a.buscarPorId(11) == nullptr && a.cantidadNodos() == 10, "el 11 ya no existe; quedan 10");
    verificar(a.contarDescendientes(a.buscarPorId(8)) == 2, "Unidad 2 ahora tiene 2 descendientes");
    verificar(ids(a.preorden()) == "1 2 3 4 5 6 7 8 9 10", "preorden sin el 11");

    verificar(a.eliminarHoja(a.buscarPorId(9)) && a.eliminarHoja(a.buscarPorId(10)), "eliminar 9 y 10");
    verificar(a.buscarPorId(8)->esHoja(), "Unidad 2 quedo sin hijos: ahora es hoja");
    verificar(a.eliminarHoja(a.buscarPorId(8)), "y ahora si se puede eliminar la Unidad 2");
    verificar(a.cantidadNodos() == 7 && a.altura() == 4, "quedan 7 nodos y la altura sigue en 4");

    // Vaciar el resto de abajo hacia arriba
    verificar(a.eliminarHoja(a.buscarPorId(4)) && a.eliminarHoja(a.buscarPorId(5)) &&
              a.eliminarHoja(a.buscarPorId(3)), "eliminar 4, 5 y luego el Tema 1.1");
    verificar(a.eliminarHoja(a.buscarPorId(7)) && a.eliminarHoja(a.buscarPorId(6)) &&
              a.eliminarHoja(a.buscarPorId(2)), "eliminar 7, 6 y luego la Unidad 1");
    verificar(a.cantidadNodos() == 1 && a.altura() == 1 && a.getRaiz()->esHoja(), "solo queda la raiz");
}

static void pruebaMostrar() {
    std::cout << "\n== Mostrar con indentacion ==\n";
    ArbolGenerico a(1, "Curso A");
    NodoCurso* u1 = a.agregarHijo(a.getRaiz(), 2, TipoNodo::Unidad, "U1");
    NodoCurso* t1 = a.agregarHijo(u1, 3, TipoNodo::Tema, "T1");
    a.agregarHijo(t1, 4, TipoNodo::Recurso, "R1");
    a.agregarHijo(a.getRaiz(), 5, TipoNodo::Unidad, "U2");

    std::ostringstream salida;
    a.mostrar(salida);
    std::string esperado =
        "Curso A  [Curso]\n"
        "    1 U1  [Unidad #1]\n"
        "        1.1 T1  [Tema #1]\n"
        "            1.1.1 R1  [Recurso #1]\n"
        "    2 U2  [Unidad #2]\n";
    verificar(salida.str() == esperado, "el arbol se muestra con indentacion y numeracion por posicion");
}

static void pruebaNumeroDeCurso() {
    std::cout << "\n== Numero de curso en la lista ==\n";
    GestorCursos g;
    std::string error;
    ArbolGenerico* a = g.crearCurso("A", error);
    ArbolGenerico* b = g.crearCurso("B", error);
    verificar(g.posicionCurso(a) == 1 && g.posicionCurso(b) == 2, "posicionCurso numera los cursos desde 1");
    ArbolGenerico suelto(99, "Suelto");
    verificar(g.posicionCurso(&suelto) == 0, "un curso que no esta en el gestor da 0");
    std::ostringstream salida;
    b->mostrar(salida, g.posicionCurso(b));
    verificar(salida.str() == "B  [Curso #2]\n", "mostrar escribe el numero del curso");
}

static void pruebaNumeracion() {
    std::cout << "\n== Numeracion por posicion ==\n";
    ArbolGenerico a(1, "Curso");
    armarEjemplo(a);   // ids: 2 U1, 3 T1.1, 4 PDF, 5 Video, 6 T1.2, 7 Quiz, 8 U2, 9-11 temas
    NodoCurso* raiz = a.getRaiz();

    verificar(a.numeracion(raiz) == "" && a.posicion(raiz) == 0, "la raiz no tiene numeracion");
    verificar(a.numeracion(a.buscarPorId(2)) == "1", "la primera unidad es 1");
    verificar(a.numeracion(a.buscarPorId(8)) == "2", "la segunda unidad es 2");
    verificar(a.numeracion(a.buscarPorId(3)) == "1.1", "el primer tema de la unidad 1 es 1.1");
    verificar(a.numeracion(a.buscarPorId(6)) == "1.2", "el segundo tema es 1.2");
    verificar(a.numeracion(a.buscarPorId(5)) == "1.1.2", "el segundo recurso del tema 1.1 es 1.1.2");
    verificar(a.numeracion(a.buscarPorId(9)) == "2.1", "la numeracion reinicia en cada unidad");
    verificar(a.numeracion(a.buscarPorId(11)) == "2.3", "tercer tema de la unidad 2");
    verificar(a.posicion(a.buscarPorId(11)) == 3, "posicion entre hermanos");
    verificar(a.numeracion(nullptr) == "", "numeracion de nullptr");

    verificar(a.buscarPorNumeracion("1") == a.buscarPorId(2), "buscar \"1\"");
    verificar(a.buscarPorNumeracion("1.1.2") == a.buscarPorId(5), "buscar \"1.1.2\"");
    verificar(a.buscarPorNumeracion(" 2.3 ") == a.buscarPorId(11), "se ignoran los espacios");
    verificar(a.buscarPorNumeracion("curso") == raiz && a.buscarPorNumeracion("CURSO") == raiz, "\"curso\" devuelve la raiz");
    verificar(a.buscarPorNumeracion("3") == nullptr, "no existe la unidad 3");
    verificar(a.buscarPorNumeracion("1.5") == nullptr, "no existe el tema 1.5");
    verificar(a.buscarPorNumeracion("0") == nullptr && a.buscarPorNumeracion("-1") == nullptr, "0 y negativos no existen");
    verificar(a.buscarPorNumeracion("1..2") == nullptr && a.buscarPorNumeracion("a.b") == nullptr, "texto mal formado");
    verificar(a.buscarPorNumeracion("") == nullptr, "texto vacio");

    // Para cada nodo: numeracion y busqueda son operaciones inversas.
    bool inversas = true;
    std::vector<NodoCurso*> todos = a.preorden();
    for (std::size_t i = 1; i < todos.size(); i++) {
        if (a.buscarPorNumeracion(a.numeracion(todos[i])) != todos[i]) inversas = false;
    }
    verificar(inversas, "buscarPorNumeracion(numeracion(x)) == x para todos los nodos");

    // Al eliminar un hermano, los demas se renumeran solos.
    a.eliminarHoja(a.buscarPorId(9));
    verificar(a.numeracion(a.buscarPorId(10)) == "2.1", "al borrar el 2.1, el 2.2 pasa a ser 2.1");
}

static void pruebaVisitas() {
    std::cout << "\n== Contador de visitas ==\n";
    ArbolGenerico a(1, "Curso A");
    NodoCurso* raiz = a.getRaiz();
    verificar(raiz->getVisitas() == 0, "un nodo nuevo tiene 0 visitas");
    raiz->registrarVisita();
    raiz->registrarVisita();
    verificar(raiz->getVisitas() == 2, "dos visitas registradas");
    verificar(raiz->deshacerVisita() && raiz->getVisitas() == 1, "deshacer baja a 1");
    verificar(raiz->deshacerVisita() && raiz->getVisitas() == 0, "deshacer baja a 0");
    verificar(!raiz->deshacerVisita() && raiz->getVisitas() == 0, "con 0 visitas no se puede deshacer (no baja de 0)");
}

static void pruebaGestorCursos() {
    std::cout << "\n== GestorCursos: crear y agregar ==\n";
    GestorCursos g;
    std::string error;
    verificar(g.vacio() && g.cantidadCursos() == 0 && g.totalNodos() == 0, "un gestor nuevo esta vacio");

    ArbolGenerico* c1 = g.crearCurso("  Estructuras de Datos ", error);
    verificar(c1 != nullptr && c1->getRaiz()->getNombre() == "Estructuras de Datos", "crearCurso limpia espacios");
    verificar(c1->getRaiz()->getId() == 1, "el primer id es 1");

    error = "";
    verificar(g.crearCurso("ESTRUCTURAS DE DATOS", error) == nullptr && !error.empty(),
              "nombre repetido (con otras mayusculas) -> rechazado con mensaje");
    error = "";
    verificar(g.crearCurso("   ", error) == nullptr && !error.empty(), "nombre vacio -> rechazado con mensaje");
    verificar(g.cantidadCursos() == 1, "los rechazos no crearon cursos");

    NodoCurso* u1 = g.agregarHijo(1, TipoNodo::Unidad, "Unidad 1", error);
    NodoCurso* t1 = g.agregarHijo(u1->getId(), TipoNodo::Tema, "Tema 1.1", error);
    verificar(u1 != nullptr && u1->getId() == 2 && t1 != nullptr && t1->getId() == 3, "los ids automaticos suben: 2 y 3");

    error = "";
    verificar(g.agregarHijo(99, TipoNodo::Tema, "X", error) == nullptr && !error.empty(), "padre inexistente -> rechazado");
    error = "";
    verificar(g.agregarHijo(1, TipoNodo::Curso, "X", error) == nullptr && !error.empty(), "tipo Curso como hijo -> rechazado");
    error = "";
    verificar(g.agregarHijo(1, TipoNodo::Unidad, "  ", error) == nullptr && !error.empty(), "nombre vacio -> rechazado");
    NodoCurso* seguido = g.agregarHijo(1, TipoNodo::Unidad, "Unidad 2", error);
    verificar(seguido != nullptr && seguido->getId() == 4, "tras los rechazos el siguiente id sigue siendo 4 (sin huecos)");

    std::cout << "\n== GestorCursos: varios cursos ==\n";
    ArbolGenerico* c2 = g.crearCurso("Programacion", error);
    NodoCurso* hijoC2 = g.agregarHijo(c2->getRaiz()->getId(), TipoNodo::Unidad, "Unidad A", error);
    verificar(c2->getRaiz()->getId() == 5 && hijoC2->getId() == 6, "los ids son unicos entre cursos (5 y 6)");
    verificar(g.cursoDeNodo(3) == c1 && g.cursoDeNodo(6) == c2, "cursoDeNodo encuentra el curso correcto");
    verificar(g.cursoDeNodo(999) == nullptr, "cursoDeNodo de un id inexistente -> nullptr");
    verificar(g.buscarPorId(6) == hijoC2, "buscarPorId encuentra nodos de cualquier curso");
    verificar(g.buscarCursoPorNombre("PROGRAMACION") == c2, "buscarCursoPorNombre ignora mayusculas");
    verificar(g.buscarCursoPorNombre("Otro") == nullptr, "buscarCursoPorNombre inexistente -> nullptr");
    verificar(g.cantidadCursos() == 2 && g.totalNodos() == 6, "2 cursos y 6 nodos en total (ids 1 al 6)");
    verificar(g.buscarPorNombre("unidad").size() == 3, "buscarPorNombre('unidad') encuentra 3 nodos entre los dos cursos");
    verificar(g.buscarPorNombre("").empty(), "buscarPorNombre('') -> nada");

    std::cout << "\n== GestorCursos: eliminar ==\n";
    error = "";
    verificar(!g.eliminarNodo(2, error) && !error.empty(), "Unidad 1 tiene hijos -> no se elimina (con mensaje)");
    error = "";
    verificar(!g.eliminarNodo(1, error) && !error.empty(), "el curso tiene contenido -> no se elimina (con mensaje)");
    error = "";
    verificar(!g.eliminarNodo(999, error) && !error.empty(), "id inexistente -> false con mensaje");
    verificar(g.eliminarNodo(3, error), "eliminar la hoja 3");
    verificar(g.eliminarNodo(2, error), "ahora la Unidad 1 es hoja y se elimina");
    verificar(g.eliminarNodo(4, error), "eliminar la Unidad 2");
    verificar(g.cantidadCursos() == 2 && c1->getRaiz()->esHoja(), "el curso 1 quedo vacio pero sigue existiendo");
    verificar(g.eliminarNodo(1, error), "eliminar la raiz vacia elimina el curso completo");
    verificar(g.cantidadCursos() == 1 && g.cursoDeNodo(1) == nullptr, "queda 1 curso y el nodo 1 ya no existe");
    verificar(g.buscarCursoPorNombre("Estructuras de Datos") == nullptr, "el nombre del curso eliminado queda libre");
    verificar(g.crearCurso("Estructuras de Datos", error) != nullptr, "se puede volver a crear un curso con ese nombre");
}

static void pruebaIdsExplicitos() {
    std::cout << "\n== GestorCursos: ids explicitos (para cargar de archivo) ==\n";
    GestorCursos g;
    std::string error;
    ArbolGenerico* c = g.crearCursoConId(50, "Curso", error);
    verificar(c != nullptr && c->getRaiz()->getId() == 50, "crearCursoConId(50)");
    NodoCurso* auto1 = g.agregarHijo(50, TipoNodo::Unidad, "U", error);
    verificar(auto1 != nullptr && auto1->getId() == 51, "el siguiente id automatico es 51");
    error = "";
    verificar(g.crearCursoConId(50, "Otro", error) == nullptr && !error.empty(), "id repetido en curso -> rechazado");
    error = "";
    verificar(g.agregarHijoConId(50, 51, TipoNodo::Tema, "X", error) == nullptr && !error.empty(), "id repetido en nodo -> rechazado");
    error = "";
    verificar(g.agregarHijoConId(50, 0, TipoNodo::Tema, "X", error) == nullptr && !error.empty(), "id 0 -> rechazado");
    NodoCurso* manual = g.agregarHijoConId(50, 7, TipoNodo::Tema, "Con id 7", error);
    verificar(manual != nullptr && manual->getId() == 7, "agregarHijoConId(7) funciona con un id menor");
    NodoCurso* auto2 = g.agregarHijo(50, TipoNodo::Tema, "Otro", error);
    verificar(auto2 != nullptr && auto2->getId() == 52, "un id manual menor no retrocede el contador (sigue 52)");
    g.limpiar();
    verificar(g.vacio() && g.totalNodos() == 0, "limpiar() deja todo vacio");
    ArbolGenerico* nuevo = g.crearCurso("Despues de limpiar", error);
    verificar(nuevo != nullptr && nuevo->getRaiz()->getId() == 1, "tras limpiar() los ids vuelven a empezar en 1");
}

static void pruebaMuchos() {
    std::cout << "\n== 300 nodos con padres aleatorios ==\n";
    GestorCursos g;
    std::string error;
    ArbolGenerico* curso = g.crearCurso("Grande", error);
    std::vector<int> existentes;
    existentes.push_back(curso->getRaiz()->getId());

    unsigned int semilla = 4242;
    bool todoAgregado = true;
    for (int i = 0; i < 300; i++) {
        semilla = semilla * 1103515245u + 12345u;
        int padre = existentes[(semilla / 65536u) % existentes.size()];
        NodoCurso* nuevo = g.agregarHijo(padre, TipoNodo::Tema, "nodo", error);
        if (nuevo == nullptr) todoAgregado = false; else existentes.push_back(nuevo->getId());
    }
    verificar(todoAgregado, "se agregaron los 300 nodos sin errores");
    verificar(curso->cantidadNodos() == 301, "cantidadNodos = 301");
    verificar(curso->preorden().size() == 301, "el preorden recorre los 301 nodos");
    verificar(curso->contarDescendientes(curso->getRaiz()) == 300, "la raiz tiene 300 descendientes");

    // Coherencia: para TODO nodo, nivel == largo de la ruta, y la ruta termina en el nodo
    bool coherente = true;
    int sumaHijos = 0;
    std::vector<NodoCurso*> todos = curso->preorden();
    for (std::size_t i = 0; i < todos.size(); i++) {
        std::vector<const NodoCurso*> r = curso->ruta(todos[i]);
        if (static_cast<int>(r.size()) != curso->nivel(todos[i])) coherente = false;
        if (r.back() != todos[i] || r.front() != curso->getRaiz()) coherente = false;
        sumaHijos += static_cast<int>(todos[i]->getHijos().size());
    }
    verificar(coherente, "para los 301 nodos: nivel == largo de la ruta, y la ruta va de la raiz al nodo");
    verificar(sumaHijos == 300, "la suma de hijos de todos los nodos es 300 (cada nodo salvo la raiz tiene un padre)");

    // Eliminar de abajo hacia arriba hasta dejar solo la raiz
    bool eliminoBien = true;
    int rondas = 0;
    while (curso->cantidadNodos() > 1 && rondas < 1000) {
        std::vector<NodoCurso*> hojas = curso->hojas();
        for (std::size_t i = 0; i < hojas.size(); i++) {
            if (hojas[i] == curso->getRaiz()) continue;
            if (!g.eliminarNodo(hojas[i]->getId(), error)) eliminoBien = false;
        }
        rondas++;
    }
    verificar(eliminoBien, "se eliminaron todas las hojas, ronda tras ronda, sin errores");
    verificar(curso->cantidadNodos() == 1 && curso->altura() == 1, "al final solo queda la raiz");
}

int main() {
    std::cout << "=== PRUEBAS DEL ARBOL GENERICO Y GestorCursos ===\n";
    pruebaTipos();
    pruebaSoloRaiz();
    pruebaEjemplo();
    pruebaRutaEntre();
    pruebaBuscarNombre();
    pruebaAgregarInvalido();
    pruebaEliminar();
    pruebaMostrar();
    pruebaNumeracion();
    pruebaNumeroDeCurso();
    pruebaVisitas();
    pruebaGestorCursos();
    pruebaIdsExplicitos();
    pruebaMuchos();

    std::cout << "\n=============================\n";
    std::cout << "Pasaron: " << pasadas << "   Fallaron: " << falladas << "\n";
    if (falladas == 0) {
        std::cout << "TODO CORRECTO\n";
    } else {
        std::cout << "HAY FALLOS: revise las lineas [FALLO]\n";
    }
    return (falladas == 0) ? 0 : 1;
}
