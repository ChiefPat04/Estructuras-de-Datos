// Reportes.cpp

#include "Reportes.h"

#include <algorithm>
#include <sstream>
#include <vector>

std::string descripcionNodo(const GestorCursos& cursos, int idNodo) {
    const NodoCurso* nodo = cursos.buscarPorId(idNodo);
    if (nodo == nullptr) {
        return "[nodo eliminado]";
    }
    const ArbolGenerico* arbol = cursos.cursoDeNodo(idNodo);
    if (nodo == arbol->getRaiz()) {
        return "[Curso] " + nodo->getNombre();
    }
    return "[" + nombreTipo(nodo->getTipo()) + " " + arbol->numeracion(nodo) + "] " +
           nodo->getNombre() + "  (" + arbol->rutaComoTexto(nodo) + ")";
}

namespace Reportes {

void imprimirSolicitud(std::ostream& salida, const Solicitud& s,
                       const GestorEstudiantes& estudiantes, const GestorCursos& cursos) {
    const Estudiante* e = estudiantes.buscarPorCarne(s.carne);
    salida << "  Solicitud #" << s.numero << " - " << nombreTipoSolicitud(s.tipo)
           << " (prioridad " << nombrePrioridad(prioridadDe(s.tipo)) << ")\n"
           << "    Estudiante: " << s.carne << " - "
           << (e != nullptr ? e->getNombre() : "(eliminado)") << "\n"
           << "    Contenido:  " << descripcionNodo(cursos, s.idNodo) << "\n"
           << "    Detalle:    " << s.descripcion << "\n";
}

// 1. Recorre la lista: orden de registro
void listaEstudiantes(std::ostream& salida, const GestorEstudiantes& estudiantes) {
    salida << "--- Reporte 1: Lista completa de estudiantes ---\n";
    if (estudiantes.vacio()) {
        salida << "No hay estudiantes registrados.\n";
        return;
    }
    int numero = 1;
    for (const Estudiante& e : estudiantes.todos()) {
        salida << numero << ". " << e.getCarne() << " | " << e.getNombre()
               << " | " << e.getCorreo() << "\n";
        numero++;
    }
}

// 2. Recorre el ABB: queda ordenado por carné
void inordenABB(std::ostream& salida, const GestorEstudiantes& estudiantes) {
    salida << "--- Reporte 2: Recorrido del ABB en inorden (por carné) ---\n";
    std::vector<const Estudiante*> orden = estudiantes.inorden();
    if (orden.empty()) {
        salida << "No hay estudiantes registrados.\n";
        return;
    }
    for (std::size_t i = 0; i < orden.size(); i++) {
        salida << (i + 1) << ". " << orden[i]->getCarne() << " | " << orden[i]->getNombre() << "\n";
    }
}

// 3
void estructuraCurso(std::ostream& salida, const ArbolGenerico& curso, int numeroCurso) {
    salida << "--- Reporte 3: Estructura jerárquica del curso ---\n";
    curso.mostrar(salida, numeroCurso);
}

// 4
void historialEstudiante(std::ostream& salida, const Estudiante& e, const GestorCursos& cursos) {
    salida << "--- Reporte 4: Historial de navegación de " << e.getNombre()
           << " (" << e.getCarne() << ") ---\n";
    std::vector<int> paginas = e.getHistorial().paginas();
    if (paginas.empty()) {
        salida << "El historial está vacío.\n";
        return;
    }
    for (std::size_t i = 0; i < paginas.size(); i++) {
        salida << "  " << (i == 0 ? "-> " : "   ") << descripcionNodo(cursos, paginas[i]) << "\n";
    }
    salida << "(-> es la página actual; arriba lo más reciente)\n";
}

// 5
void solicitudesPendientes(std::ostream& salida, const GestorSolicitudes& solicitudes,
                           const GestorEstudiantes& estudiantes, const GestorCursos& cursos) {
    salida << "--- Reporte 5: Solicitudes pendientes en cola ---\n";
    std::vector<Solicitud> todas = solicitudes.todas();
    if (todas.empty()) {
        salida << "No hay solicitudes pendientes.\n";
        return;
    }
    salida << "Modo de atención: "
           << (solicitudes.getModoPrioridad() ? "por prioridad" : "por orden de llegada") << "\n";
    for (std::size_t i = 0; i < todas.size(); i++) {
        imprimirSolicitud(salida, todas[i], estudiantes, cursos);
    }
    salida << "Total en espera: " << solicitudes.cantidad() << "\n";
}

// 6
void cantidadNodos(std::ostream& salida, const GestorCursos& cursos) {
    salida << "--- Reporte 6: Cantidad de nodos del árbol genérico ---\n";
    if (cursos.vacio()) {
        salida << "No hay cursos registrados.\n";
        return;
    }
    for (const ArbolGenerico* curso : cursos.cursos()) {
        salida << "  " << curso->getRaiz()->getNombre() << ": "
               << curso->cantidadNodos() << " nodos\n";
    }
    salida << "Total de nodos: " << cursos.totalNodos() << "\n";
}

// 7
void alturaArboles(std::ostream& salida, const GestorCursos& cursos) {
    salida << "--- Reporte 7: Altura del árbol genérico ---\n";
    if (cursos.vacio()) {
        salida << "No hay cursos registrados.\n";
        return;
    }
    for (const ArbolGenerico* curso : cursos.cursos()) {
        salida << "  " << curso->getRaiz()->getNombre() << ": "
               << curso->altura() << " niveles\n";
    }
}

// 8
void cantidadEstudiantes(std::ostream& salida, const GestorEstudiantes& estudiantes) {
    salida << "--- Reporte 8: Cantidad de estudiantes registrados ---\n"
           << "Estudiantes: " << estudiantes.cantidad() << "\n";
}

// 9. El menor y el mayor salen de los extremos del ABB
void menorYMayorCarne(std::ostream& salida, const GestorEstudiantes& estudiantes) {
    salida << "--- Reporte 9: Estudiante con menor y mayor carné ---\n";
    const Estudiante* menor = estudiantes.menorCarne();
    const Estudiante* mayor = estudiantes.mayorCarne();
    if (menor == nullptr) {
        salida << "No hay estudiantes registrados.\n";
        return;
    }
    salida << "Menor carné: " << menor->getCarne() << " - " << menor->getNombre() << "\n"
           << "Mayor carné: " << mayor->getCarne() << " - " << mayor->getNombre() << "\n";
}

// 10
void recursosHoja(std::ostream& salida, const ArbolGenerico& curso) {
    salida << "--- Reporte 10: Recursos hoja del curso \"" << curso.getRaiz()->getNombre() << "\" ---\n";
    std::vector<NodoCurso*> hojas = curso.hojas();
    for (const NodoCurso* hoja : hojas) {
        salida << "  [" << nombreTipo(hoja->getTipo()) << " " << curso.numeracion(hoja) << "] "
               << hoja->getNombre() << "\n      ruta: " << curso.rutaComoTexto(hoja) << "\n";
    }
    salida << "Total de hojas: " << hojas.size() << "\n";
}

// Para ordenar los nodos de más a menos visitados.
static bool masVisitado(const NodoCurso* a, const NodoCurso* b) {
    return a->getVisitas() > b->getVisitas();
}

// 11 (extra)
void estadisticasUso(std::ostream& salida, const GestorEstudiantes& estudiantes,
                     const GestorCursos& cursos) {
    salida << "--- Reporte 11: Estadísticas de uso de recursos ---\n";

    // Junta todos los nodos con visitas, de todos los cursos
    std::vector<const NodoCurso*> visitados;
    int totalVisitas = 0;
    for (const ArbolGenerico* curso : cursos.cursos()) {
        for (const NodoCurso* nodo : curso->preorden()) {
            totalVisitas += nodo->getVisitas();
            if (nodo->getVisitas() > 0) visitados.push_back(nodo);
        }
    }
    salida << "Visitas registradas en total: " << totalVisitas << "\n";

    // stable_sort mantiene el orden del árbol cuando hay empate
    std::stable_sort(visitados.begin(), visitados.end(), masVisitado);
    salida << "\nContenidos más visitados (hasta 5):\n";
    if (visitados.empty()) {
        salida << "  Nadie ha visitado contenidos todavía.\n";
    }
    for (std::size_t i = 0; i < visitados.size() && i < 5; i++) {
        salida << "  " << (i + 1) << ". " << descripcionNodo(cursos, visitados[i]->getId())
               << " - " << visitados[i]->getVisitas() << " visita(s)\n";
    }

    salida << "\nActividad por estudiante:\n";
    if (estudiantes.vacio()) {
        salida << "  No hay estudiantes registrados.\n";
        return;
    }
    for (const Estudiante& e : estudiantes.todos()) {
        const HistorialAcademico& h = e.getHistorial();
        salida << "  " << e.getCarne() << " | " << e.getNombre() << ": "
               << h.cantidadPaginas() << " página(s) en el historial, "
               << h.completados().size() << " completado(s), "
               << h.inscripciones().size() << " inscripción(es)\n";
    }
}

void todos(std::ostream& salida, const GestorEstudiantes& estudiantes,
           const GestorCursos& cursos, const GestorSolicitudes& solicitudes) {
    salida << "===== REPORTES DEL SISTEMA =====\n\n";
    listaEstudiantes(salida, estudiantes);
    salida << "\n";
    inordenABB(salida, estudiantes);
    salida << "\n";
    for (const ArbolGenerico* curso : cursos.cursos()) {
        estructuraCurso(salida, *curso, cursos.posicionCurso(curso));
        salida << "\n";
    }
    for (const Estudiante& e : estudiantes.todos()) {
        historialEstudiante(salida, e, cursos);
        salida << "\n";
    }
    solicitudesPendientes(salida, solicitudes, estudiantes, cursos);
    salida << "\n";
    cantidadNodos(salida, cursos);
    salida << "\n";
    alturaArboles(salida, cursos);
    salida << "\n";
    cantidadEstudiantes(salida, estudiantes);
    salida << "\n";
    menorYMayorCarne(salida, estudiantes);
    salida << "\n";
    for (const ArbolGenerico* curso : cursos.cursos()) {
        recursosHoja(salida, *curso);
        salida << "\n";
    }
    estadisticasUso(salida, estudiantes, cursos);
}

}
