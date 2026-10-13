// GestorEstudiantes.cpp

#include "GestorEstudiantes.h"
#include "Utils.h"

GestorEstudiantes::GestorEstudiantes() {}

std::vector<const Estudiante*> GestorEstudiantes::soloEstudiantes(
    const std::vector<std::pair<long long, Estudiante*> >& pares) {
    std::vector<const Estudiante*> resultado;
    for (std::size_t i = 0; i < pares.size(); i++) {
        resultado.push_back(pares[i].second);
    }
    return resultado;
}

// Primero se valida todo; solo si está bien se modifican las estructuras.
bool GestorEstudiantes::registrar(long long carne, const std::string& nombre,
                                  const std::string& correo, std::string& error) {
    std::string nombreLimpio = Utils::quitarEspacios(nombre);

    if (carne <= 0) {
        error = "El carne debe ser un numero positivo.";
        return false;
    }
    if (nombreLimpio.empty()) {
        error = "El nombre no puede estar vacio.";
        return false;
    }
    if (!Utils::esCorreoValido(correo)) {
        error = "El correo debe tener el formato nombre@estudiantec.cr";
        return false;
    }
    if (indice_.contiene(carne)) {
        error = "Ya existe un estudiante con ese carne.";
        return false;
    }
    if (buscarPorCorreo(correo) != nullptr) {
        error = "Ya existe un estudiante con ese correo.";
        return false;
    }

    lista_.push_back(Estudiante(carne, nombreLimpio, correo));
    indice_.insertar(carne, &lista_.back());
    return true;
}

// Nombre y correo no son la clave del ABB, así que el índice no se toca.
bool GestorEstudiantes::cambiarNombre(long long carne, const std::string& nuevoNombre,
                                      std::string& error) {
    Estudiante* e = buscarPorCarne(carne);
    if (e == nullptr) {
        error = "No existe un estudiante con ese carne.";
        return false;
    }
    std::string limpio = Utils::quitarEspacios(nuevoNombre);
    if (limpio.empty()) {
        error = "El nombre no puede estar vacio.";
        return false;
    }
    e->setNombre(limpio);
    return true;
}

bool GestorEstudiantes::cambiarCorreo(long long carne, const std::string& nuevoCorreo,
                                      std::string& error) {
    Estudiante* e = buscarPorCarne(carne);
    if (e == nullptr) {
        error = "No existe un estudiante con ese carne.";
        return false;
    }
    if (!Utils::esCorreoValido(nuevoCorreo)) {
        error = "El correo debe tener el formato nombre@estudiantec.cr";
        return false;
    }
    const Estudiante* dueno = buscarPorCorreo(nuevoCorreo);
    if (dueno != nullptr && dueno != e) {
        error = "Ese correo ya pertenece a otro estudiante.";
        return false;
    }
    e->setCorreo(nuevoCorreo);
    return true;
}

// El carné es la clave del ABB: hay que sacarlo y volverlo a insertar.
bool GestorEstudiantes::cambiarCarne(long long carneActual, long long carneNuevo,
                                     std::string& error) {
    Estudiante* e = buscarPorCarne(carneActual);
    if (e == nullptr) {
        error = "No existe un estudiante con ese carne.";
        return false;
    }
    if (carneNuevo <= 0) {
        error = "El carne debe ser un numero positivo.";
        return false;
    }
    if (carneNuevo == carneActual) {
        return true;
    }
    if (indice_.contiene(carneNuevo)) {
        error = "Ya existe un estudiante con ese carne.";
        return false;
    }
    indice_.eliminar(carneActual);
    e->setCarne(carneNuevo);
    indice_.insertar(carneNuevo, e);
    return true;
}

// Primero se quita del índice y luego de la lista, para no dejar punteros rotos.
bool GestorEstudiantes::eliminar(long long carne) {
    Estudiante* e = buscarPorCarne(carne);
    if (e == nullptr) {
        return false;
    }
    indice_.eliminar(carne);

    for (std::list<Estudiante>::iterator it = lista_.begin(); it != lista_.end(); ++it) {
        if (&(*it) == e) {
            lista_.erase(it);
            break;
        }
    }
    return true;
}

Estudiante* GestorEstudiantes::buscarPorCarne(long long carne) {
    Estudiante** puntero = indice_.buscar(carne);
    return (puntero == nullptr) ? nullptr : *puntero;
}

const Estudiante* GestorEstudiantes::buscarPorCarne(long long carne) const {
    Estudiante* const* puntero = indice_.buscar(carne);
    return (puntero == nullptr) ? nullptr : *puntero;
}

// Busca por parte del nombre, sin distinguir mayúsculas.
std::vector<const Estudiante*> GestorEstudiantes::buscarPorNombre(
    const std::string& fragmento) const {
    std::vector<const Estudiante*> resultado;
    std::string buscado = Utils::quitarEspacios(fragmento);
    if (buscado.empty()) {
        return resultado;
    }
    for (const Estudiante& e : lista_) {
        if (Utils::contieneTexto(e.getNombre(), buscado)) {
            resultado.push_back(&e);
        }
    }
    return resultado;
}

const Estudiante* GestorEstudiantes::buscarPorCorreo(const std::string& correo) const {
    std::string buscado = Utils::aMinusculas(Utils::quitarEspacios(correo));
    for (const Estudiante& e : lista_) {
        if (Utils::aMinusculas(e.getCorreo()) == buscado) {
            return &e;
        }
    }
    return nullptr;
}

void GestorEstudiantes::limpiar() {
    indice_.vaciar();
    lista_.clear();
}

std::vector<const Estudiante*> GestorEstudiantes::inorden() const {
    return soloEstudiantes(indice_.inorden());
}
std::vector<const Estudiante*> GestorEstudiantes::preorden() const {
    return soloEstudiantes(indice_.preorden());
}
std::vector<const Estudiante*> GestorEstudiantes::postorden() const {
    return soloEstudiantes(indice_.postorden());
}

const Estudiante* GestorEstudiantes::menorCarne() const {
    Estudiante* const* puntero = indice_.minimo();
    return (puntero == nullptr) ? nullptr : *puntero;
}
const Estudiante* GestorEstudiantes::mayorCarne() const {
    Estudiante* const* puntero = indice_.maximo();
    return (puntero == nullptr) ? nullptr : *puntero;
}

int GestorEstudiantes::alturaIndice() const {
    return indice_.altura();
}

void GestorEstudiantes::mostrarIndice(std::ostream& salida) const {
    indice_.mostrar(salida);
}
