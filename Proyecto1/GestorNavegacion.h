// GestorNavegacion.h
// Aplica las acciones académicas de un estudiante sobre los cursos:
// visitar, retroceder, completar, inscribirse y deshacer.

#ifndef GESTOR_NAVEGACION_H
#define GESTOR_NAVEGACION_H

#include <string>
#include "Estudiante.h"
#include "GestorCursos.h"

class GestorNavegacion {
private:
    GestorCursos& cursos_;

public:
    explicit GestorNavegacion(GestorCursos& cursos);

    // Devuelven false y dejan el motivo en 'error' si no se pudo.
    bool visitar(Estudiante& e, int idNodo, std::string& error);
    bool retroceder(Estudiante& e, std::string& error);
    bool marcarCompletado(Estudiante& e, int idNodo, std::string& error);   // solo Recurso
    bool inscribirse(Estudiante& e, int idNodo, std::string& error);        // solo Actividad
    bool deshacer(Estudiante& e, std::string& mensaje);
};

#endif
