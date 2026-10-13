// GestorCursos.h
// Guarda todos los cursos: una lista de árboles genéricos.
// Los ids son únicos en todo el sistema, así un id identifica
// un nodo sin importar de qué curso sea.

#ifndef GESTOR_CURSOS_H
#define GESTOR_CURSOS_H

#include <list>
#include <string>
#include <vector>
#include "ArbolGenerico.h"

class GestorCursos {
private:
    std::list<ArbolGenerico*> cursos_;
    int proximoId_;

    void reservarId(int id);

public:
    GestorCursos();
    ~GestorCursos();

    GestorCursos(const GestorCursos&) = delete;
    GestorCursos& operator=(const GestorCursos&) = delete;

    // Devuelven nullptr si fallan y dejan el motivo en 'error'.
    ArbolGenerico* crearCurso(const std::string& nombre, std::string& error);
    NodoCurso* agregarHijo(int idPadre, TipoNodo tipo, const std::string& nombre,
                           std::string& error);

    // Con id elegido; se usan al cargar datos desde archivo.
    ArbolGenerico* crearCursoConId(int id, const std::string& nombre, std::string& error);
    NodoCurso* agregarHijoConId(int idPadre, int id, TipoNodo tipo,
                                const std::string& nombre, std::string& error);

    // Solo elimina hojas. Si es la raíz y no tiene contenido, elimina el curso.
    bool eliminarNodo(int id, std::string& error);

    NodoCurso* buscarPorId(int id) const;
    ArbolGenerico* cursoDeNodo(int id) const;
    int posicionCurso(const ArbolGenerico* curso) const;   // 1 = primer curso, 0 = no está
    ArbolGenerico* buscarCursoPorNombre(const std::string& nombre) const;
    std::vector<NodoCurso*> buscarPorNombre(const std::string& fragmento) const;

    const std::list<ArbolGenerico*>& cursos() const { return cursos_; }
    int cantidadCursos() const { return static_cast<int>(cursos_.size()); }
    int totalNodos() const;
    bool vacio() const { return cursos_.empty(); }

    void limpiar();
};

#endif
