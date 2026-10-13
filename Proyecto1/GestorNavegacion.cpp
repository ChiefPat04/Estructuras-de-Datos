// GestorNavegacion.cpp

#include "GestorNavegacion.h"

GestorNavegacion::GestorNavegacion(GestorCursos& cursos) : cursos_(cursos) {}

bool GestorNavegacion::visitar(Estudiante& e, int idNodo, std::string& error) {
    NodoCurso* nodo = cursos_.buscarPorId(idNodo);
    if (nodo == nullptr) {
        error = "No existe un nodo con ese id.";
        return false;
    }
    if (nodo->getTipo() == TipoNodo::Curso) {
        error = "Elija una unidad, tema, recurso o actividad (no el curso).";
        return false;
    }
    nodo->registrarVisita();
    e.getHistorial().registrarVisita(idNodo);
    return true;
}

bool GestorNavegacion::retroceder(Estudiante& e, std::string& error) {
    if (!e.getHistorial().retroceder()) {
        error = "No hay una página anterior en el historial.";
        return false;
    }
    return true;
}

bool GestorNavegacion::marcarCompletado(Estudiante& e, int idNodo, std::string& error) {
    NodoCurso* nodo = cursos_.buscarPorId(idNodo);
    if (nodo == nullptr) {
        error = "No existe un nodo con ese id.";
        return false;
    }
    if (nodo->getTipo() != TipoNodo::Recurso) {
        error = "Solo se pueden marcar como completados los recursos.";
        return false;
    }
    if (!e.getHistorial().marcarCompletado(idNodo)) {
        error = "Ese recurso ya estaba completado.";
        return false;
    }
    e.getHistorial().registrarAccion(TipoAccion::Completado, idNodo);
    return true;
}

bool GestorNavegacion::inscribirse(Estudiante& e, int idNodo, std::string& error) {
    NodoCurso* nodo = cursos_.buscarPorId(idNodo);
    if (nodo == nullptr) {
        error = "No existe un nodo con ese id.";
        return false;
    }
    if (nodo->getTipo() != TipoNodo::Actividad) {
        error = "Solo hay inscripción en actividades.";
        return false;
    }
    if (!e.getHistorial().inscribir(idNodo)) {
        error = "Ya está inscrito en esa actividad.";
        return false;
    }
    e.getHistorial().registrarAccion(TipoAccion::Inscripcion, idNodo);
    return true;
}

// Revierte la última acción de la pila de acciones.
bool GestorNavegacion::deshacer(Estudiante& e, std::string& mensaje) {
    Accion accion;
    if (!e.getHistorial().sacarAccion(accion)) {
        mensaje = "No hay acciones para deshacer.";
        return false;
    }
    if (accion.tipo == TipoAccion::Visita) {
        NodoCurso* nodo = cursos_.buscarPorId(accion.idNodo);
        if (nodo != nullptr) nodo->deshacerVisita();
        e.getHistorial().quitarPaginaSiEsActual(accion.idNodo);
        mensaje = "Se deshizo una visita.";
    } else if (accion.tipo == TipoAccion::Completado) {
        e.getHistorial().quitarCompletado(accion.idNodo);
        mensaje = "Se deshizo un recurso completado.";
    } else {
        e.getHistorial().quitarInscripcion(accion.idNodo);
        mensaje = "Se deshizo una inscripción.";
    }
    return true;
}
