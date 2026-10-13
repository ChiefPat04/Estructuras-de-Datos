// HistorialAcademico.cpp

#include "HistorialAcademico.h"

namespace {

// Busca un id en una lista y lo quita. Devuelve false si no estaba.
bool quitarDeLista(std::list<int>& lista, int id) {
    for (std::list<int>::iterator it = lista.begin(); it != lista.end(); ++it) {
        if (*it == id) {
            lista.erase(it);
            return true;
        }
    }
    return false;
}

bool estaEnLista(const std::list<int>& lista, int id) {
    for (int valor : lista) {
        if (valor == id) return true;
    }
    return false;
}

}

void HistorialAcademico::registrarVisita(int idNodo) {
    paginas_.push(idNodo);
    registrarAccion(TipoAccion::Visita, idNodo);
}

void HistorialAcademico::agregarPagina(int idNodo) {
    paginas_.push(idNodo);
}

// Saca la página actual para volver a la anterior.
bool HistorialAcademico::retroceder() {
    if (paginas_.size() < 2) return false;
    paginas_.pop();
    return true;
}

void HistorialAcademico::limpiar() {
    while (!paginas_.empty()) {
        paginas_.pop();
    }
}

bool HistorialAcademico::quitarPaginaSiEsActual(int idNodo) {
    if (paginas_.empty() || paginas_.top() != idNodo) return false;
    paginas_.pop();
    return true;
}

int HistorialAcademico::paginaActual() const {
    return paginas_.empty() ? 0 : paginas_.top();
}

// Se copia la pila y se va vaciando la copia.
std::vector<int> HistorialAcademico::paginas() const {
    std::vector<int> resultado;
    std::stack<int> copia = paginas_;
    while (!copia.empty()) {
        resultado.push_back(copia.top());
        copia.pop();
    }
    return resultado;
}

bool HistorialAcademico::marcarCompletado(int idNodo) {
    if (estaEnLista(completados_, idNodo)) return false;
    completados_.push_back(idNodo);
    return true;
}

bool HistorialAcademico::quitarCompletado(int idNodo) {
    return quitarDeLista(completados_, idNodo);
}

bool HistorialAcademico::estaCompletado(int idNodo) const {
    return estaEnLista(completados_, idNodo);
}

bool HistorialAcademico::inscribir(int idNodo) {
    if (estaEnLista(inscripciones_, idNodo)) return false;
    inscripciones_.push_back(idNodo);
    return true;
}

bool HistorialAcademico::quitarInscripcion(int idNodo) {
    return quitarDeLista(inscripciones_, idNodo);
}

bool HistorialAcademico::estaInscrito(int idNodo) const {
    return estaEnLista(inscripciones_, idNodo);
}

void HistorialAcademico::registrarAccion(TipoAccion tipo, int idNodo) {
    Accion a;
    a.tipo = tipo;
    a.idNodo = idNodo;
    acciones_.push(a);
}

bool HistorialAcademico::sacarAccion(Accion& accion) {
    if (acciones_.empty()) return false;
    accion = acciones_.top();
    acciones_.pop();
    return true;
}

std::vector<Accion> HistorialAcademico::acciones() const {
    std::vector<Accion> resultado;
    std::stack<Accion> copia = acciones_;
    while (!copia.empty()) {
        resultado.push_back(copia.top());
        copia.pop();
    }
    return resultado;
}
