// Reportes.h - reportes del sistema
// Cada reporte escribe en un ostream: sirve para la pantalla (std::cout)
// y para un archivo (ofstream).

#ifndef REPORTES_H
#define REPORTES_H

#include <ostream>
#include <string>
#include "GestorCursos.h"
#include "GestorEstudiantes.h"
#include "GestorSolicitudes.h"

// Ejemplo: [Tema 1.1] Tipos abstractos  (Curso > Fundamentos > Tipos abstractos)
std::string descripcionNodo(const GestorCursos& cursos, int idNodo);

namespace Reportes {

    void imprimirSolicitud(std::ostream& salida, const Solicitud& s,
                           const GestorEstudiantes& estudiantes, const GestorCursos& cursos);

    // Los 10 reportes obligatorios
    void listaEstudiantes(std::ostream& salida, const GestorEstudiantes& estudiantes);                 // 1
    void inordenABB(std::ostream& salida, const GestorEstudiantes& estudiantes);                       // 2
    void estructuraCurso(std::ostream& salida, const ArbolGenerico& curso, int numeroCurso);          // 3
    void historialEstudiante(std::ostream& salida, const Estudiante& e, const GestorCursos& cursos);  // 4
    void solicitudesPendientes(std::ostream& salida, const GestorSolicitudes& solicitudes,
                               const GestorEstudiantes& estudiantes, const GestorCursos& cursos);      // 5
    void cantidadNodos(std::ostream& salida, const GestorCursos& cursos);                              // 6
    void alturaArboles(std::ostream& salida, const GestorCursos& cursos);                              // 7
    void cantidadEstudiantes(std::ostream& salida, const GestorEstudiantes& estudiantes);              // 8
    void menorYMayorCarne(std::ostream& salida, const GestorEstudiantes& estudiantes);                 // 9
    void recursosHoja(std::ostream& salida, const ArbolGenerico& curso);                               // 10

    // Extra: contenidos más visitados y actividad de cada estudiante
    void estadisticasUso(std::ostream& salida, const GestorEstudiantes& estudiantes,
                         const GestorCursos& cursos);                                                  // 11

    // Todos los reportes, para exportar a un archivo
    void todos(std::ostream& salida, const GestorEstudiantes& estudiantes,
               const GestorCursos& cursos, const GestorSolicitudes& solicitudes);
}

#endif
