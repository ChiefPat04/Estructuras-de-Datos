// HistorialAcademico.h - historial de un estudiante
// Dos pilas: las páginas visitadas (para retroceder) y las acciones
// realizadas (para deshacer). También guarda los recursos completados
// y las actividades en las que está inscrito.

#ifndef HISTORIAL_ACADEMICO_H
#define HISTORIAL_ACADEMICO_H

#include <list>
#include <stack>
#include <vector>

enum class TipoAccion { Visita, Completado, Inscripcion };

struct Accion {
    TipoAccion tipo;
    int idNodo;
};

class HistorialAcademico {
private:
    std::stack<int> paginas_;
    std::stack<Accion> acciones_;
    std::list<int> completados_;
    std::list<int> inscripciones_;

public:
    // Visitas
    void registrarVisita(int idNodo);
    void agregarPagina(int idNodo);          // sin registrar acción (para cargar datos)
    bool retroceder();                       // necesita al menos 2 páginas
    void limpiar();                          // vacía solo las páginas
    bool quitarPaginaSiEsActual(int idNodo);
    int cantidadPaginas() const { return static_cast<int>(paginas_.size()); }
    int paginaActual() const;                // 0 si no hay
    std::vector<int> paginas() const;        // la más reciente primero

    // Completados e inscripciones (devuelven false si ya estaban o no estaban)
    bool marcarCompletado(int idNodo);
    bool quitarCompletado(int idNodo);
    bool estaCompletado(int idNodo) const;
    bool inscribir(int idNodo);
    bool quitarInscripcion(int idNodo);
    bool estaInscrito(int idNodo) const;
    const std::list<int>& completados() const { return completados_; }
    const std::list<int>& inscripciones() const { return inscripciones_; }

    // Pila de acciones
    void registrarAccion(TipoAccion tipo, int idNodo);
    bool sacarAccion(Accion& accion);
    int cantidadAcciones() const { return static_cast<int>(acciones_.size()); }
    std::vector<Accion> acciones() const;    // la más reciente primero
};

#endif
