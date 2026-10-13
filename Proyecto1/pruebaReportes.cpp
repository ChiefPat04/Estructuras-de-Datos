// pruebaReportes.cpp - pruebas de Reportes (tiene su propio main)
// Cada reporte se escribe en un ostringstream y se revisa su texto.

#include <iostream>
#include <sstream>
#include <string>
#include "GestorCursos.h"
#include "GestorEstudiantes.h"
#include "GestorNavegacion.h"
#include "GestorSolicitudes.h"
#include "Reportes.h"

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

static bool contiene(const std::string& texto, const std::string& fragmento) {
    return texto.find(fragmento) != std::string::npos;
}

int main() {
    GestorEstudiantes estudiantes;
    GestorCursos cursos;
    GestorNavegacion nav(cursos);
    GestorSolicitudes solicitudes(estudiantes, cursos);
    std::string error;

    std::cout << "\n== Reportes con el sistema vacío ==\n";
    {
        std::ostringstream s;
        Reportes::listaEstudiantes(s, estudiantes);
        verificar(contiene(s.str(), "No hay estudiantes"), "1: lista vacía");
    }
    {
        std::ostringstream s;
        Reportes::menorYMayorCarne(s, estudiantes);
        verificar(contiene(s.str(), "No hay estudiantes"), "9: sin estudiantes");
    }
    {
        std::ostringstream s;
        Reportes::solicitudesPendientes(s, solicitudes, estudiantes, cursos);
        verificar(contiene(s.str(), "No hay solicitudes"), "5: sin solicitudes");
    }
    {
        std::ostringstream s;
        Reportes::cantidadNodos(s, cursos);
        verificar(contiene(s.str(), "No hay cursos"), "6: sin cursos");
    }
    {
        std::ostringstream s;
        Reportes::estadisticasUso(s, estudiantes, cursos);
        verificar(contiene(s.str(), "Visitas registradas en total: 0") &&
                  contiene(s.str(), "Nadie ha visitado") && contiene(s.str(), "No hay estudiantes"),
                  "11: estadísticas con el sistema vacío");
    }

    // Datos de prueba
    estudiantes.registrar(50, "Eva", "eva@estudiantec.cr", error);
    estudiantes.registrar(30, "Ana", "ana@estudiantec.cr", error);
    estudiantes.registrar(70, "Luis", "luis@estudiantec.cr", error);
    cursos.crearCurso("Datos", error);                                 // 1
    cursos.agregarHijo(1, TipoNodo::Unidad, "U1", error);              // 2
    cursos.agregarHijo(2, TipoNodo::Tema, "T1", error);                // 3
    cursos.agregarHijo(3, TipoNodo::Recurso, "PDF", error);            // 4
    cursos.agregarHijo(3, TipoNodo::Recurso, "Video", error);          // 5
    nav.visitar(*estudiantes.buscarPorCarne(30), 3, error);
    nav.visitar(*estudiantes.buscarPorCarne(30), 4, error);
    nav.visitar(*estudiantes.buscarPorCarne(70), 4, error);   // el PDF lleva 2 visitas
    solicitudes.insertar(30, 4, TipoSolicitud::ReporteError, "roto", error);
    solicitudes.insertar(70, 5, TipoSolicitud::RevisionTarea, "duda", error);

    std::cout << "\n== Reportes con datos ==\n";
    std::ostringstream s1;
    Reportes::listaEstudiantes(s1, estudiantes);
    verificar(contiene(s1.str(), "1. 50 | Eva") && contiene(s1.str(), "3. 70 | Luis"),
              "1: la lista sale en orden de registro");

    std::ostringstream s2;
    Reportes::inordenABB(s2, estudiantes);
    verificar(contiene(s2.str(), "1. 30 | Ana") && contiene(s2.str(), "3. 70 | Luis"),
              "2: el inorden sale ordenado por carné");

    std::ostringstream s3;
    Reportes::estructuraCurso(s3, *cursos.cursos().front(), 1);
    verificar(contiene(s3.str(), "Datos  [Curso #1]") && contiene(s3.str(), "        1.1 T1  [Tema #1]"),
              "3: estructura con indentación");

    std::ostringstream s4;
    Reportes::historialEstudiante(s4, *estudiantes.buscarPorCarne(30), cursos);
    verificar(contiene(s4.str(), "-> [Recurso 1.1.1] PDF"), "4: la página actual es la última visitada");
    std::ostringstream s4b;
    Reportes::historialEstudiante(s4b, *estudiantes.buscarPorCarne(50), cursos);
    verificar(contiene(s4b.str(), "vacío"), "4: historial vacío de otro estudiante");

    std::ostringstream s5;
    Reportes::solicitudesPendientes(s5, solicitudes, estudiantes, cursos);
    verificar(contiene(s5.str(), "Solicitud #1") && contiene(s5.str(), "Total en espera: 2"),
              "5: solicitudes pendientes");

    std::ostringstream s6;
    Reportes::cantidadNodos(s6, cursos);
    verificar(contiene(s6.str(), "Datos: 5 nodos") && contiene(s6.str(), "Total de nodos: 5"),
              "6: cantidad de nodos");

    std::ostringstream s7;
    Reportes::alturaArboles(s7, cursos);
    verificar(contiene(s7.str(), "Datos: 4 niveles"), "7: altura del árbol");

    std::ostringstream s8;
    Reportes::cantidadEstudiantes(s8, estudiantes);
    verificar(contiene(s8.str(), "Estudiantes: 3"), "8: cantidad de estudiantes");

    std::ostringstream s9;
    Reportes::menorYMayorCarne(s9, estudiantes);
    verificar(contiene(s9.str(), "Menor carné: 30 - Ana") && contiene(s9.str(), "Mayor carné: 70 - Luis"),
              "9: menor y mayor carné");

    std::ostringstream s10;
    Reportes::recursosHoja(s10, *cursos.cursos().front());
    verificar(contiene(s10.str(), "PDF") && contiene(s10.str(), "Video") && contiene(s10.str(), "Total de hojas: 2"),
              "10: recursos hoja");

    std::ostringstream s11;
    Reportes::estadisticasUso(s11, estudiantes, cursos);
    verificar(contiene(s11.str(), "Visitas registradas en total: 3"), "11: total de visitas");
    verificar(contiene(s11.str(), "1. [Recurso 1.1.1] PDF") && contiene(s11.str(), "2 visita(s)"),
              "11: el recurso más visitado va primero");
    verificar(contiene(s11.str(), "2. [Tema 1.1] T1") && !contiene(s11.str(), "Video  ("),
              "11: el segundo es el tema y los no visitados no aparecen");
    verificar(contiene(s11.str(), "30 | Ana: 2 página(s)") && contiene(s11.str(), "50 | Eva: 0 página(s)") &&
              contiene(s11.str(), "70 | Luis: 1 página(s)"),
              "11: actividad por estudiante");

    std::cout << "\n== Exportar todo ==\n";
    std::ostringstream todo;
    Reportes::todos(todo, estudiantes, cursos, solicitudes);
    bool estanLos11 = true;
    for (int i = 1; i <= 11; i++) {
        std::ostringstream marca;
        marca << "Reporte " << i << ":";
        if (!contiene(todo.str(), marca.str())) estanLos11 = false;
    }
    verificar(estanLos11, "el reporte completo incluye los 11 reportes");

    std::cout << "\n== Nodo eliminado en el historial ==\n";
    cursos.eliminarNodo(4, error);
    verificar(contiene(descripcionNodo(cursos, 4), "eliminado"), "un nodo borrado se muestra como eliminado");

    std::cout << "\n=============================\n"
              << "Pasaron: " << pasadas << "   Fallaron: " << falladas << "\n"
              << (falladas == 0 ? "TODO CORRECTO" : "HAY FALLOS") << "\n"
              << "=============================\n";
    return falladas == 0 ? 0 : 1;
}
