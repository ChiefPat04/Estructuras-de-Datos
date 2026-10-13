// GestorSolicitudes.cpp

#include "GestorSolicitudes.h"
#include "Utils.h"

int prioridadDe(TipoSolicitud tipo) {
    switch (tipo) {
        case TipoSolicitud::ReporteError:         return 0;
        case TipoSolicitud::HabilitacionRecurso:  return 1;
        default:                                  return 2;
    }
}

std::string nombreTipoSolicitud(TipoSolicitud tipo) {
    switch (tipo) {
        case TipoSolicitud::RevisionTarea:        return "Revisión de tarea";
        case TipoSolicitud::ConsultaAcademica:    return "Consulta académica";
        case TipoSolicitud::HabilitacionRecurso:  return "Habilitación de recurso";
        case TipoSolicitud::ReporteError:         return "Reporte de error en material";
    }
    return "Desconocido";
}

std::string nombrePrioridad(int nivel) {
    if (nivel == 0) return "alta";
    if (nivel == 1) return "media";
    return "baja";
}

GestorSolicitudes::GestorSolicitudes(const GestorEstudiantes& estudiantes,
                                     const GestorCursos& cursos)
    : estudiantes_(estudiantes), cursos_(cursos),
      proximoNumero_(1), atendidas_(0), modoPrioridad_(true) {}

bool GestorSolicitudes::insertar(long long carne, int idNodo, TipoSolicitud tipo,
                                 const std::string& descripcion, std::string& error) {
    std::string texto = Utils::quitarEspacios(descripcion);
    if (estudiantes_.buscarPorCarne(carne) == nullptr) {
        error = "No existe un estudiante con ese carné.";
        return false;
    }
    if (cursos_.buscarPorId(idNodo) == nullptr) {
        error = "No existe un nodo con ese id.";
        return false;
    }
    if (texto.empty()) {
        error = "La descripción no puede estar vacía.";
        return false;
    }
    Solicitud s;
    s.numero = proximoNumero_;
    s.carne = carne;
    s.idNodo = idNodo;
    s.tipo = tipo;
    s.descripcion = texto;
    colas_[prioridadDe(tipo)].push(s);
    proximoNumero_++;
    return true;
}

// En modo prioridad gana el primer nivel con solicitudes; en modo llegada,
// la solicitud de menor número entre los frentes de las tres colas.
int GestorSolicitudes::nivelSiguiente(const std::queue<Solicitud> colas[], bool prioridad) {
    int mejor = -1;
    for (int nivel = 0; nivel < NIVELES; nivel++) {
        if (colas[nivel].empty()) continue;
        if (mejor == -1) {
            mejor = nivel;
        } else if (!prioridad && colas[nivel].front().numero < colas[mejor].front().numero) {
            mejor = nivel;
        }
    }
    return mejor;
}

bool GestorSolicitudes::atenderSiguiente(Solicitud& atendida) {
    int nivel = nivelSiguiente(colas_, modoPrioridad_);
    if (nivel == -1) return false;
    atendida = colas_[nivel].front();
    colas_[nivel].pop();
    atendidas_++;
    return true;
}

bool GestorSolicitudes::verFrente(Solicitud& frente) const {
    int nivel = nivelSiguiente(colas_, modoPrioridad_);
    if (nivel == -1) return false;
    frente = colas_[nivel].front();
    return true;
}

std::vector<Solicitud> GestorSolicitudes::todas() const {
    return ordenar(modoPrioridad_);
}

std::vector<Solicitud> GestorSolicitudes::enOrdenDeLlegada() const {
    return ordenar(false);
}

// Se trabaja sobre copias de las colas para no modificar las originales.
std::vector<Solicitud> GestorSolicitudes::ordenar(bool prioridad) const {
    std::queue<Solicitud> copia[NIVELES];
    for (int i = 0; i < NIVELES; i++) {
        copia[i] = colas_[i];
    }
    std::vector<Solicitud> resultado;
    int nivel = nivelSiguiente(copia, prioridad);
    while (nivel != -1) {
        resultado.push_back(copia[nivel].front());
        copia[nivel].pop();
        nivel = nivelSiguiente(copia, prioridad);
    }
    return resultado;
}

int GestorSolicitudes::cantidad() const {
    int total = 0;
    for (int i = 0; i < NIVELES; i++) {
        total += static_cast<int>(colas_[i].size());
    }
    return total;
}

int GestorSolicitudes::cantidadEnNivel(int nivel) const {
    if (nivel < 0 || nivel >= NIVELES) return 0;
    return static_cast<int>(colas_[nivel].size());
}

void GestorSolicitudes::restaurar(const Solicitud& s) {
    colas_[prioridadDe(s.tipo)].push(s);
}

void GestorSolicitudes::setContadores(int proximoNumero, int atendidas) {
    proximoNumero_ = proximoNumero;
    atendidas_ = atendidas;
}

void GestorSolicitudes::limpiar() {
    for (int i = 0; i < NIVELES; i++) {
        colas_[i] = std::queue<Solicitud>();
    }
    proximoNumero_ = 1;
    atendidas_ = 0;
}
