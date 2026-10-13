// MenuNavegacion.cpp - navegación académica de un estudiante

#include "Menus.h"

#include <iostream>
#include <vector>
#include "Utils.h"

static void visitar(Estudiante& e, GestorCursos& cursos, GestorNavegacion& nav) {
    std::cout << "\n--- Visitar un contenido ---\n";
    NodoCurso* nodo = elegirNodo(cursos, "Número del nodo a visitar");
    if (nodo == nullptr) return;

    std::string error;
    if (nav.visitar(e, nodo->getId(), error)) {
        std::cout << "Visita registrada: " << descripcionNodo(cursos, nodo->getId()) << "\n";
    } else {
        std::cout << "No se pudo visitar: " << error << "\n";
    }
}

static void mostrarHistorial(const Estudiante& e, const GestorCursos& cursos) {
    std::cout << "\n--- Historial de " << e.getNombre() << " ---\n";
    std::vector<int> paginas = e.getHistorial().paginas();
    if (paginas.empty()) {
        std::cout << "El historial está vacío.\n";
        return;
    }
    for (std::size_t i = 0; i < paginas.size(); i++) {
        std::cout << "  " << (i == 0 ? "-> " : "   ") << descripcionNodo(cursos, paginas[i]) << "\n";
    }
    std::cout << "(-> es la página actual; arriba lo más reciente)\n";
}

static void retroceder(Estudiante& e, GestorCursos& cursos, GestorNavegacion& nav) {
    std::string error;
    if (nav.retroceder(e, error)) {
        std::cout << "Página actual: "
                  << descripcionNodo(cursos, e.getHistorial().paginaActual()) << "\n";
    } else {
        std::cout << error << "\n";
    }
}

static void limpiarHistorial(Estudiante& e) {
    if (e.getHistorial().cantidadPaginas() == 0) {
        std::cout << "El historial ya está vacío.\n";
        return;
    }
    if (Utils::confirmar("¿Seguro que desea limpiar el historial?")) {
        e.getHistorial().limpiar();
        std::cout << "Historial limpiado.\n";
    }
}

static void completar(Estudiante& e, GestorCursos& cursos, GestorNavegacion& nav) {
    std::cout << "\n--- Marcar recurso como completado ---\n";
    NodoCurso* nodo = elegirNodo(cursos, "Número del recurso");
    if (nodo == nullptr) return;

    std::string error;
    if (nav.marcarCompletado(e, nodo->getId(), error)) {
        std::cout << "Completado: " << descripcionNodo(cursos, nodo->getId()) << "\n";
    } else {
        std::cout << "No se pudo marcar: " << error << "\n";
    }
}

static void inscribirse(Estudiante& e, GestorCursos& cursos, GestorNavegacion& nav) {
    std::cout << "\n--- Inscribirse en una actividad ---\n";
    NodoCurso* nodo = elegirNodo(cursos, "Número de la actividad");
    if (nodo == nullptr) return;

    std::string error;
    if (nav.inscribirse(e, nodo->getId(), error)) {
        std::cout << "Inscrito en: " << descripcionNodo(cursos, nodo->getId()) << "\n";
    } else {
        std::cout << "No se pudo inscribir: " << error << "\n";
    }
}

static void deshacer(Estudiante& e, GestorNavegacion& nav) {
    std::string mensaje;
    nav.deshacer(e, mensaje);
    std::cout << mensaje << "\n";
}

static void mostrarProgreso(const Estudiante& e, const GestorCursos& cursos) {
    const HistorialAcademico& h = e.getHistorial();
    std::cout << "\n--- Progreso de " << e.getNombre() << " ---\n"
              << "Recursos completados: " << h.completados().size() << "\n";
    for (int id : h.completados()) {
        std::cout << "  " << descripcionNodo(cursos, id) << "\n";
    }
    std::cout << "Actividades inscritas: " << h.inscripciones().size() << "\n";
    for (int id : h.inscripciones()) {
        std::cout << "  " << descripcionNodo(cursos, id) << "\n";
    }
    std::cout << "Acciones que se pueden deshacer: " << h.cantidadAcciones() << "\n";
}

void menuNavegacion(GestorEstudiantes& estudiantes, GestorCursos& cursos,
                    GestorNavegacion& navegacion) {
    std::cout << "\n--- Navegación académica ---\n";
    Estudiante* e = pedirEstudiante(estudiantes);
    if (e == nullptr) {
        Utils::pausar();
        return;
    }
    if (cursos.vacio()) {
        std::cout << "No hay cursos. Cree uno en el menú de cursos.\n";
        Utils::pausar();
        return;
    }

    while (true) {
        std::cout << "\n===== NAVEGACIÓN DE " << e->getNombre() << " =====\n"
                  << "1. Visitar un contenido\n"
                  << "2. Mostrar historial\n"
                  << "3. Retroceder a la página anterior\n"
                  << "4. Limpiar historial\n"
                  << "5. Marcar recurso como completado\n"
                  << "6. Inscribirse en una actividad\n"
                  << "7. Deshacer última acción\n"
                  << "8. Ver progreso\n"
                  << "0. Volver al menú principal\n";
        long long opcion = Utils::leerEntero("Opción: ", 0, 8);

        switch (opcion) {
            case 1: visitar(*e, cursos, navegacion); break;
            case 2: mostrarHistorial(*e, cursos); break;
            case 3: retroceder(*e, cursos, navegacion); break;
            case 4: limpiarHistorial(*e); break;
            case 5: completar(*e, cursos, navegacion); break;
            case 6: inscribirse(*e, cursos, navegacion); break;
            case 7: deshacer(*e, navegacion); break;
            case 8: mostrarProgreso(*e, cursos); break;
            default: return;
        }
        Utils::pausar();
    }
}
