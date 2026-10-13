// pruebaNavegacion.cpp - pruebas de HistorialAcademico y GestorNavegacion
// (tiene su propio main, se compila aparte)

#include <iostream>
#include <string>
#include <vector>
#include "GestorCursos.h"
#include "GestorEstudiantes.h"
#include "GestorNavegacion.h"

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

// Curso de prueba: ids 1 Curso, 2 Unidad, 3 Tema, 4 Recurso, 5 Recurso, 6 Actividad
static void armarCurso(GestorCursos& g) {
    std::string error;
    g.crearCurso("Curso", error);
    g.agregarHijo(1, TipoNodo::Unidad, "Unidad", error);
    g.agregarHijo(2, TipoNodo::Tema, "Tema", error);
    g.agregarHijo(3, TipoNodo::Recurso, "PDF", error);
    g.agregarHijo(3, TipoNodo::Recurso, "Video", error);
    g.agregarHijo(3, TipoNodo::Actividad, "Quiz", error);
}

static void pruebaPila() {
    std::cout << "\n== Historial: pila de páginas ==\n";
    HistorialAcademico h;
    verificar(h.cantidadPaginas() == 0 && h.paginaActual() == 0, "empieza vacío");
    verificar(!h.retroceder(), "no se puede retroceder sin páginas");

    h.registrarVisita(3);
    verificar(!h.retroceder(), "con una sola página tampoco se puede retroceder");
    h.registrarVisita(4);
    h.registrarVisita(5);
    verificar(h.paginaActual() == 5, "la página actual es la última visitada");

    std::vector<int> p = h.paginas();
    verificar(p.size() == 3 && p[0] == 5 && p[1] == 4 && p[2] == 3, "paginas() va de la más reciente a la más antigua");
    verificar(h.cantidadPaginas() == 3, "la copia no vacía la pila original");

    verificar(h.retroceder() && h.paginaActual() == 4, "retroceder vuelve a la anterior");
    h.limpiar();
    verificar(h.cantidadPaginas() == 0, "limpiar vacía el historial");
    verificar(h.cantidadAcciones() == 3, "limpiar no borra las acciones");
}

static void pruebaListas() {
    std::cout << "\n== Historial: completados e inscripciones ==\n";
    HistorialAcademico h;
    verificar(h.marcarCompletado(4), "marcar completado");
    verificar(!h.marcarCompletado(4), "no se repite un completado");
    verificar(h.estaCompletado(4) && !h.estaCompletado(5), "estaCompletado");
    verificar(h.quitarCompletado(4) && !h.quitarCompletado(4), "quitar completado una sola vez");
    verificar(h.inscribir(6) && !h.inscribir(6), "inscribir no repite");
    verificar(h.estaInscrito(6), "estaInscrito");
    verificar(h.quitarInscripcion(6) && h.inscripciones().empty(), "quitar inscripción");
}

static void pruebaNavegacion() {
    std::cout << "\n== GestorNavegacion ==\n";
    GestorCursos cursos;
    armarCurso(cursos);
    GestorNavegacion nav(cursos);
    Estudiante e(2025001, "Ana", "ana@estudiantec.cr");
    std::string error;

    verificar(nav.visitar(e, 3, error), "visitar un tema");
    verificar(cursos.buscarPorId(3)->getVisitas() == 1, "la visita se cuenta en el nodo");
    verificar(!nav.visitar(e, 99, error), "visitar un id inexistente falla");
    verificar(!nav.visitar(e, 1, error), "no se puede visitar el curso");
    verificar(nav.visitar(e, 4, error), "visitar un recurso");
    verificar(e.getHistorial().cantidadPaginas() == 2, "las visitas válidas están en el historial");

    verificar(nav.retroceder(e, error) && e.getHistorial().paginaActual() == 3, "retroceder");
    verificar(!nav.retroceder(e, error), "retroceder con una página falla");

    verificar(!nav.marcarCompletado(e, 3, error), "un tema no se marca como completado");
    verificar(nav.marcarCompletado(e, 4, error), "completar un recurso");
    verificar(!nav.marcarCompletado(e, 4, error), "completar dos veces falla");
    verificar(!nav.inscribirse(e, 4, error), "un recurso no tiene inscripción");
    verificar(nav.inscribirse(e, 6, error), "inscribirse en una actividad");
    verificar(!nav.inscribirse(e, 6, error), "inscribirse dos veces falla");
    verificar(!nav.inscribirse(e, 99, error), "inscribirse en un id inexistente falla");

    std::string mensaje;
    verificar(nav.deshacer(e, mensaje) && !e.getHistorial().estaInscrito(6), "deshacer la inscripción");
    verificar(nav.deshacer(e, mensaje) && !e.getHistorial().estaCompletado(4), "deshacer el completado");
    verificar(e.getHistorial().cantidadAcciones() == 2, "quedan las dos visitas por deshacer");

    nav.visitar(e, 5, error);
    int visitasAntes = cursos.buscarPorId(5)->getVisitas();
    verificar(nav.deshacer(e, mensaje), "deshacer una visita");
    verificar(cursos.buscarPorId(5)->getVisitas() == visitasAntes - 1, "la visita se resta del nodo");
    verificar(e.getHistorial().paginaActual() == 3, "la página sale del historial si era la actual");

    nav.deshacer(e, mensaje);
    nav.deshacer(e, mensaje);
    verificar(!nav.deshacer(e, mensaje), "deshacer sin acciones falla");
    verificar(cursos.buscarPorId(3)->getVisitas() == 0, "al deshacer todo, las visitas vuelven a 0");

    std::cout << "\n== Historial por estudiante ==\n";
    GestorEstudiantes ge;
    ge.registrar(1, "Uno", "uno@estudiantec.cr", error);
    ge.registrar(2, "Dos", "dos@estudiantec.cr", error);
    nav.visitar(*ge.buscarPorCarne(1), 3, error);
    verificar(ge.buscarPorCarne(1)->getHistorial().cantidadPaginas() == 1, "el estudiante 1 tiene una página");
    verificar(ge.buscarPorCarne(2)->getHistorial().cantidadPaginas() == 0, "el estudiante 2 no se ve afectado");
}

int main() {
    pruebaPila();
    pruebaListas();
    pruebaNavegacion();

    std::cout << "\n=============================\n"
              << "Pasaron: " << pasadas << "   Fallaron: " << falladas << "\n"
              << (falladas == 0 ? "TODO CORRECTO" : "HAY FALLOS") << "\n"
              << "=============================\n";
    return falladas == 0 ? 0 : 1;
}
