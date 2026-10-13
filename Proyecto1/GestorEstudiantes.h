// GestorEstudiantes.h
// La lista (std::list) guarda los estudiantes en orden de registro.
// El ABB es un índice por carné: guarda punteros a los elementos de la lista.
// Se usa list y no vector porque así los punteros no se invalidan al crecer.
// Todo método que agrega, cambia o quita estudiantes actualiza las dos estructuras.

#ifndef GESTOR_ESTUDIANTES_H
#define GESTOR_ESTUDIANTES_H

#include <list>
#include <ostream>
#include <string>
#include <utility>
#include <vector>
#include "ABB.h"
#include "Estudiante.h"

class GestorEstudiantes {
private:
    std::list<Estudiante> lista_;
    ABB<long long, Estudiante*> indice_;   // carné -> puntero al estudiante

    static std::vector<const Estudiante*> soloEstudiantes(
        const std::vector<std::pair<long long, Estudiante*> >& pares);

public:
    GestorEstudiantes();

    // No se puede copiar: el índice apuntaría a los estudiantes del original.
    GestorEstudiantes(const GestorEstudiantes&) = delete;
    GestorEstudiantes& operator=(const GestorEstudiantes&) = delete;

    // Devuelven true si funcionó; si no, dejan el motivo en 'error'.
    bool registrar(long long carne, const std::string& nombre,
                   const std::string& correo, std::string& error);
    bool cambiarNombre(long long carne, const std::string& nuevoNombre,
                       std::string& error);
    bool cambiarCorreo(long long carne, const std::string& nuevoCorreo,
                       std::string& error);
    bool cambiarCarne(long long carneActual, long long carneNuevo,
                      std::string& error);
    bool eliminar(long long carne);

    // Por carné usa el ABB; por nombre y correo recorre la lista.
    Estudiante* buscarPorCarne(long long carne);
    const Estudiante* buscarPorCarne(long long carne) const;
    std::vector<const Estudiante*> buscarPorNombre(const std::string& fragmento) const;
    const Estudiante* buscarPorCorreo(const std::string& correo) const;

    const std::list<Estudiante>& todos() const { return lista_; }
    int cantidad() const { return static_cast<int>(lista_.size()); }
    bool vacio() const { return lista_.empty(); }
    void limpiar();

    // Recorridos y datos del ABB
    std::vector<const Estudiante*> inorden() const;
    std::vector<const Estudiante*> preorden() const;
    std::vector<const Estudiante*> postorden() const;
    const Estudiante* menorCarne() const;
    const Estudiante* mayorCarne() const;
    int alturaIndice() const;
    void mostrarIndice(std::ostream& salida) const;
};

#endif
