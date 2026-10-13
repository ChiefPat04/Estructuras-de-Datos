// GestorSolicitudes.h
// Solicitudes de atención en colas. Hay tres colas, una por nivel de prioridad.
// Modo llegada: se atiende por orden de llegada (cola normal).
// Modo prioridad: se atiende primero el nivel más urgente (cola con prioridad);
// dentro de un mismo nivel se respeta el orden de llegada.

#ifndef GESTOR_SOLICITUDES_H
#define GESTOR_SOLICITUDES_H

#include <queue>
#include <string>
#include <vector>
#include "GestorCursos.h"
#include "GestorEstudiantes.h"

enum class TipoSolicitud { RevisionTarea, ConsultaAcademica, HabilitacionRecurso, ReporteError };

// 0 = alta (reporte de error), 1 = media (habilitación de recurso),
// 2 = baja (revisión de tarea y consulta académica)
int prioridadDe(TipoSolicitud tipo);
std::string nombreTipoSolicitud(TipoSolicitud tipo);
std::string nombrePrioridad(int nivel);

struct Solicitud {
    int numero;            // orden de llegada
    long long carne;       // estudiante que la hizo
    int idNodo;            // nodo del curso al que se refiere
    TipoSolicitud tipo;
    std::string descripcion;
};

class GestorSolicitudes {
private:
    static const int NIVELES = 3;

    const GestorEstudiantes& estudiantes_;
    const GestorCursos& cursos_;
    std::queue<Solicitud> colas_[NIVELES];
    int proximoNumero_;
    int atendidas_;
    bool modoPrioridad_;

    // Nivel de la cola que tiene la siguiente solicitud (-1 si no hay).
    static int nivelSiguiente(const std::queue<Solicitud> colas[], bool prioridad);
    std::vector<Solicitud> ordenar(bool prioridad) const;

public:
    GestorSolicitudes(const GestorEstudiantes& estudiantes, const GestorCursos& cursos);

    // Devuelve false y deja el motivo en 'error' si el estudiante o el nodo no existen.
    bool insertar(long long carne, int idNodo, TipoSolicitud tipo,
                  const std::string& descripcion, std::string& error);

    bool atenderSiguiente(Solicitud& atendida);
    bool verFrente(Solicitud& frente) const;
    std::vector<Solicitud> todas() const;       // en el orden en que se atenderían
    std::vector<Solicitud> enOrdenDeLlegada() const;
    int cantidad() const;
    int cantidadEnNivel(int nivel) const;
    int atendidas() const { return atendidas_; }

    bool getModoPrioridad() const { return modoPrioridad_; }
    void setModoPrioridad(bool activo) { modoPrioridad_ = activo; }

    // Para cargar datos: guarda la solicitud tal cual, sin validar ni renumerar.
    void restaurar(const Solicitud& s);
    void setContadores(int proximoNumero, int atendidas);
    int getProximoNumero() const { return proximoNumero_; }

    void limpiar();
};

#endif
