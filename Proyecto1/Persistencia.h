// Persistencia.h - guardar y cargar los datos en archivos de texto
// Se usan cuatro archivos en la carpeta del programa, con campos separados por '|':
//   datos_estudiantes.txt   carne|nombre|correo
//   datos_cursos.txt        id|idPadre|tipo|nombre|visitas   (padres antes que hijos)
//   datos_historiales.txt   P|carne|idNodo  A|carne|tipoAccion|idNodo
//                           C|carne|idNodo  I|carne|idNodo
//   datos_solicitudes.txt   config|modoPrioridad|proximoNumero|atendidas
//                           numero|carne|idNodo|tipo|descripcion

#ifndef PERSISTENCIA_H
#define PERSISTENCIA_H

#include <string>
#include "GestorCursos.h"
#include "GestorEstudiantes.h"
#include "GestorSolicitudes.h"

namespace Persistencia {

    // Devuelve false y deja el motivo en 'error' si algo falla.
    bool guardar(const GestorEstudiantes& estudiantes, const GestorCursos& cursos,
                 const GestorSolicitudes& solicitudes, std::string& error);

    // Si algún archivo falta o está dañado, no se modifica nada.
    bool cargar(GestorEstudiantes& estudiantes, GestorCursos& cursos,
                GestorSolicitudes& solicitudes, std::string& error);

    bool hayDatosGuardados();
}

#endif
