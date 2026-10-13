// pruebaSolicitudes.cpp - pruebas de GestorSolicitudes (tiene su propio main)

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "GestorCursos.h"
#include "GestorEstudiantes.h"
#include "GestorSolicitudes.h"

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

// Devuelve los números de las solicitudes en el orden de atención, p. ej. "2 3 1"
static std::string orden(const GestorSolicitudes& g) {
    std::vector<Solicitud> v = g.todas();
    std::ostringstream texto;
    for (std::size_t i = 0; i < v.size(); i++) {
        if (i > 0) texto << " ";
        texto << v[i].numero;
    }
    return texto.str();
}

int main() {
    GestorEstudiantes estudiantes;
    GestorCursos cursos;
    std::string error;
    estudiantes.registrar(100, "Ana", "ana@estudiantec.cr", error);
    cursos.crearCurso("Curso", error);                              // id 1
    cursos.agregarHijo(1, TipoNodo::Unidad, "Unidad", error);       // id 2

    GestorSolicitudes sol(estudiantes, cursos);

    std::cout << "\n== Validaciones ==\n";
    verificar(!sol.insertar(999, 2, TipoSolicitud::ReporteError, "x", error), "estudiante inexistente falla");
    verificar(!sol.insertar(100, 77, TipoSolicitud::ReporteError, "x", error), "nodo inexistente falla");
    verificar(!sol.insertar(100, 2, TipoSolicitud::ReporteError, "   ", error), "descripción vacía falla");
    verificar(sol.cantidad() == 0, "nada se guardó con datos inválidos");

    std::cout << "\n== Cola vacía ==\n";
    Solicitud s;
    verificar(!sol.atenderSiguiente(s), "atender sin solicitudes devuelve false");
    verificar(!sol.verFrente(s), "ver frente sin solicitudes devuelve false");
    verificar(sol.todas().empty(), "todas() vacío");

    std::cout << "\n== Prioridades por tipo ==\n";
    verificar(prioridadDe(TipoSolicitud::ReporteError) == 0, "reporte de error: alta");
    verificar(prioridadDe(TipoSolicitud::HabilitacionRecurso) == 1, "habilitación: media");
    verificar(prioridadDe(TipoSolicitud::RevisionTarea) == 2, "revisión de tarea: baja");
    verificar(prioridadDe(TipoSolicitud::ConsultaAcademica) == 2, "consulta: baja");

    // Llegan en este orden: 1 tarea, 2 error, 3 consulta, 4 habilitación, 5 error
    sol.insertar(100, 2, TipoSolicitud::RevisionTarea, "t", error);
    sol.insertar(100, 2, TipoSolicitud::ReporteError, "e1", error);
    sol.insertar(100, 2, TipoSolicitud::ConsultaAcademica, "c", error);
    sol.insertar(100, 2, TipoSolicitud::HabilitacionRecurso, "h", error);
    sol.insertar(100, 2, TipoSolicitud::ReporteError, "e2", error);

    std::cout << "\n== Modo por orden de llegada ==\n";
    sol.setModoPrioridad(false);
    verificar(sol.cantidad() == 5, "hay 5 pendientes");
    verificar(orden(sol) == "1 2 3 4 5", "orden de llegada");
    verificar(sol.verFrente(s) && s.numero == 1, "el frente es la primera que llegó");
    verificar(sol.cantidad() == 5, "ver frente no la saca");

    std::cout << "\n== Modo por prioridad ==\n";
    sol.setModoPrioridad(true);
    verificar(orden(sol) == "2 5 4 1 3", "primero altas, luego media, luego bajas (cada nivel por llegada)");
    verificar(orden(sol) == "2 5 4 1 3", "todas() no modifica las colas");

    std::cout << "\n== Atender ==\n";
    verificar(sol.atenderSiguiente(s) && s.numero == 2, "primero se atiende la alta más antigua");
    verificar(sol.atenderSiguiente(s) && s.numero == 5, "luego la otra alta");
    verificar(sol.atenderSiguiente(s) && s.numero == 4, "luego la media");
    sol.setModoPrioridad(false);
    verificar(sol.atenderSiguiente(s) && s.numero == 1, "en modo llegada sale la más antigua");
    verificar(sol.cantidad() == 1 && sol.atendidas() == 4, "quedan 1 pendiente y 4 atendidas");

    std::cout << "\n== Conteo por nivel y limpiar ==\n";
    sol.insertar(100, 2, TipoSolicitud::ReporteError, "e3", error);
    verificar(sol.cantidadEnNivel(0) == 1 && sol.cantidadEnNivel(2) == 1 && sol.cantidadEnNivel(1) == 0,
              "cantidad por nivel");
    verificar(sol.cantidadEnNivel(9) == 0, "nivel fuera de rango devuelve 0");
    sol.limpiar();
    verificar(sol.cantidad() == 0 && sol.atendidas() == 0, "limpiar deja todo en cero");
    sol.insertar(100, 2, TipoSolicitud::RevisionTarea, "t", error);
    sol.verFrente(s);
    verificar(s.numero == 1, "la numeración reinicia después de limpiar");

    std::cout << "\n=============================\n"
              << "Pasaron: " << pasadas << "   Fallaron: " << falladas << "\n"
              << (falladas == 0 ? "TODO CORRECTO" : "HAY FALLOS") << "\n"
              << "=============================\n";
    return falladas == 0 ? 0 : 1;
}
