// Menus.h - submenús del sistema

#ifndef MENUS_H
#define MENUS_H

#include <string>
#include "GestorCursos.h"
#include "GestorEstudiantes.h"
#include "GestorNavegacion.h"
#include "GestorSolicitudes.h"
#include "Reportes.h"

void menuEstudiantes(GestorEstudiantes& gestor);
void menuCursos(GestorCursos& gestor);
void menuNavegacion(GestorEstudiantes& estudiantes, GestorCursos& cursos,
                    GestorNavegacion& navegacion);
void menuSolicitudes(GestorSolicitudes& solicitudes, const GestorEstudiantes& estudiantes,
                     const GestorCursos& cursos);

void menuIndice(const GestorEstudiantes& estudiantes);
void menuReportes(GestorEstudiantes& estudiantes, GestorCursos& cursos,
                  const GestorSolicitudes& solicitudes);

// Ayudas compartidas por los menús (están en Menus.cpp)
Estudiante* pedirEstudiante(GestorEstudiantes& estudiantes);
ArbolGenerico* elegirCurso(const GestorCursos& gestor);
NodoCurso* pedirNodo(const ArbolGenerico* arbol, const std::string& mensaje);
NodoCurso* elegirNodo(const GestorCursos& cursos, const std::string& mensaje);

#endif
