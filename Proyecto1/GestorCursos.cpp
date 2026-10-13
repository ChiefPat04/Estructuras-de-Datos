// GestorCursos.cpp

#include "GestorCursos.h"
#include "Utils.h"

GestorCursos::GestorCursos() : proximoId_(1) {}

GestorCursos::~GestorCursos() {
    limpiar();
}

void GestorCursos::limpiar() {
    for (ArbolGenerico* curso : cursos_) {
        delete curso;
    }
    cursos_.clear();
    proximoId_ = 1;
}

void GestorCursos::reservarId(int id) {
    if (id >= proximoId_) proximoId_ = id + 1;
}

ArbolGenerico* GestorCursos::crearCurso(const std::string& nombre, std::string& error) {
    return crearCursoConId(proximoId_, nombre, error);
}

ArbolGenerico* GestorCursos::crearCursoConId(int id, const std::string& nombre,
                                             std::string& error) {
    std::string limpio = Utils::quitarEspacios(nombre);
    if (limpio.empty()) {
        error = "El nombre del curso no puede estar vacío.";
        return nullptr;
    }
    if (id <= 0) {
        error = "El id debe ser un número positivo.";
        return nullptr;
    }
    if (buscarPorId(id) != nullptr) {
        error = "Ya existe un nodo con ese id.";
        return nullptr;
    }
    if (buscarCursoPorNombre(limpio) != nullptr) {
        error = "Ya existe un curso con ese nombre.";
        return nullptr;
    }
    ArbolGenerico* curso = new ArbolGenerico(id, limpio);
    cursos_.push_back(curso);
    reservarId(id);
    return curso;
}

NodoCurso* GestorCursos::agregarHijo(int idPadre, TipoNodo tipo, const std::string& nombre,
                                     std::string& error) {
    return agregarHijoConId(idPadre, proximoId_, tipo, nombre, error);
}

NodoCurso* GestorCursos::agregarHijoConId(int idPadre, int id, TipoNodo tipo,
                                          const std::string& nombre, std::string& error) {
    if (id <= 0) {
        error = "El id debe ser un número positivo.";
        return nullptr;
    }
    if (buscarPorId(id) != nullptr) {
        error = "Ya existe un nodo con ese id.";
        return nullptr;
    }
    if (tipo == TipoNodo::Curso) {
        error = "Un curso solo puede ser la raíz. Use \"crear curso\".";
        return nullptr;
    }
    std::string limpio = Utils::quitarEspacios(nombre);
    if (limpio.empty()) {
        error = "El nombre no puede estar vacío.";
        return nullptr;
    }
    ArbolGenerico* arbol = cursoDeNodo(idPadre);
    if (arbol == nullptr) {
        error = "No existe un nodo padre con ese id.";
        return nullptr;
    }
    NodoCurso* padre = arbol->buscarPorId(idPadre);
    NodoCurso* nuevo = arbol->agregarHijo(padre, id, tipo, limpio);
    if (nuevo == nullptr) {
        error = "No se pudo agregar el nodo.";
        return nullptr;
    }
    reservarId(id);
    return nuevo;
}

bool GestorCursos::eliminarNodo(int id, std::string& error) {
    ArbolGenerico* arbol = cursoDeNodo(id);
    if (arbol == nullptr) {
        error = "No existe un nodo con ese id.";
        return false;
    }
    NodoCurso* nodo = arbol->buscarPorId(id);

    if (nodo == arbol->getRaiz()) {
        if (!nodo->esHoja()) {
            error = "El curso todavía tiene contenido. Elimine primero sus nodos.";
            return false;
        }
        cursos_.remove(arbol);
        delete arbol;
        return true;
    }
    if (!arbol->eliminarHoja(nodo)) {
        error = "Solo se pueden eliminar nodos hoja (sin hijos).";
        return false;
    }
    return true;
}

ArbolGenerico* GestorCursos::cursoDeNodo(int id) const {
    for (ArbolGenerico* curso : cursos_) {
        if (curso->buscarPorId(id) != nullptr) return curso;
    }
    return nullptr;
}

NodoCurso* GestorCursos::buscarPorId(int id) const {
    for (ArbolGenerico* curso : cursos_) {
        NodoCurso* nodo = curso->buscarPorId(id);
        if (nodo != nullptr) return nodo;
    }
    return nullptr;
}

int GestorCursos::posicionCurso(const ArbolGenerico* curso) const {
    int contador = 1;
    for (const ArbolGenerico* actual : cursos_) {
        if (actual == curso) return contador;
        contador++;
    }
    return 0;
}

ArbolGenerico* GestorCursos::buscarCursoPorNombre(const std::string& nombre) const {
    std::string buscado = Utils::aMinusculas(Utils::quitarEspacios(nombre));
    for (ArbolGenerico* curso : cursos_) {
        if (Utils::aMinusculas(curso->getRaiz()->getNombre()) == buscado) return curso;
    }
    return nullptr;
}

std::vector<NodoCurso*> GestorCursos::buscarPorNombre(const std::string& fragmento) const {
    std::vector<NodoCurso*> resultado;
    for (ArbolGenerico* curso : cursos_) {
        std::vector<NodoCurso*> encontrados = curso->buscarPorNombre(fragmento);
        resultado.insert(resultado.end(), encontrados.begin(), encontrados.end());
    }
    return resultado;
}

int GestorCursos::totalNodos() const {
    int total = 0;
    for (const ArbolGenerico* curso : cursos_) {
        total += curso->cantidadNodos();
    }
    return total;
}
