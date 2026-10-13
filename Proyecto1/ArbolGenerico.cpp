// ArbolGenerico.cpp

#include "ArbolGenerico.h"
#include "Utils.h"

#include <algorithm>
#include <sstream>

std::string nombreTipo(TipoNodo tipo) {
    switch (tipo) {
        case TipoNodo::Curso:     return "Curso";
        case TipoNodo::Unidad:    return "Unidad";
        case TipoNodo::Tema:      return "Tema";
        case TipoNodo::Recurso:   return "Recurso";
        case TipoNodo::Actividad: return "Actividad";
    }
    return "Desconocido";
}

bool tipoDesdeTexto(const std::string& texto, TipoNodo& tipo) {
    std::string t = Utils::aMinusculas(Utils::quitarEspacios(texto));
    if (t == "curso")     { tipo = TipoNodo::Curso;     return true; }
    if (t == "unidad")    { tipo = TipoNodo::Unidad;    return true; }
    if (t == "tema")      { tipo = TipoNodo::Tema;      return true; }
    if (t == "recurso")   { tipo = TipoNodo::Recurso;   return true; }
    if (t == "actividad") { tipo = TipoNodo::Actividad; return true; }
    return false;
}

NodoCurso::NodoCurso(int id, TipoNodo tipo, const std::string& nombre, NodoCurso* padre)
    : id_(id), tipo_(tipo), nombre_(nombre), padre_(padre), visitas_(0) {}

bool NodoCurso::deshacerVisita() {
    if (visitas_ <= 0) return false;
    visitas_--;
    return true;
}

ArbolGenerico::ArbolGenerico(int idRaiz, const std::string& nombreCurso)
    : raiz_(nullptr), cantidad_(1) {
    raiz_ = new NodoCurso(idRaiz, TipoNodo::Curso, nombreCurso, nullptr);
}

ArbolGenerico::~ArbolGenerico() {
    destruirRec(raiz_);
}

// Postorden: primero los hijos y al final el nodo.
void ArbolGenerico::destruirRec(NodoCurso* nodo) {
    if (nodo == nullptr) return;
    for (NodoCurso* hijo : nodo->hijos_) {
        destruirRec(hijo);
    }
    delete nodo;
}

// Sube hasta la raíz y comprueba que sea la de este árbol.
bool ArbolGenerico::perteneceAlArbol(const NodoCurso* nodo) const {
    while (nodo != nullptr && nodo->padre_ != nullptr) {
        nodo = nodo->padre_;
    }
    return nodo != nullptr && nodo == raiz_;
}

NodoCurso* ArbolGenerico::agregarHijo(NodoCurso* padre, int id, TipoNodo tipo,
                                      const std::string& nombre) {
    std::string limpio = Utils::quitarEspacios(nombre);
    if (limpio.empty()) return nullptr;
    if (tipo == TipoNodo::Curso) return nullptr;
    if (!perteneceAlArbol(padre)) return nullptr;

    NodoCurso* nuevo = new NodoCurso(id, tipo, limpio, padre);
    padre->hijos_.push_back(nuevo);
    cantidad_++;
    return nuevo;
}

bool ArbolGenerico::eliminarHoja(NodoCurso* nodo) {
    if (nodo == nullptr || nodo == raiz_) return false;
    if (!perteneceAlArbol(nodo)) return false;
    if (!nodo->hijos_.empty()) return false;

    nodo->padre_->hijos_.remove(nodo);
    delete nodo;
    cantidad_--;
    return true;
}

// Ejemplo de línea:  "    1.1 Tipos abstractos  [Tema #1]"
void ArbolGenerico::mostrarRec(std::ostream& salida, const NodoCurso* nodo, int nivel,
                               const std::string& numero, int posicion) {
    salida << std::string(static_cast<std::size_t>((nivel - 1) * 4), ' ');
    if (nivel == 1) {
        salida << nodo->nombre_ << "  [Curso";
        if (posicion > 0) salida << " #" << posicion;
        salida << "]\n";
    } else {
        salida << numero << " " << nodo->nombre_ << "  ["
               << nombreTipo(nodo->tipo_) << " #" << posicion << "]\n";
    }
    int contador = 1;
    for (const NodoCurso* hijo : nodo->hijos_) {
        std::ostringstream numeroHijo;
        if (!numero.empty()) numeroHijo << numero << ".";
        numeroHijo << contador;
        mostrarRec(salida, hijo, nivel + 1, numeroHijo.str(), contador);
        contador++;
    }
}

void ArbolGenerico::mostrar(std::ostream& salida, int numeroCurso) const {
    mostrarRec(salida, raiz_, 1, "", numeroCurso);
}

NodoCurso* ArbolGenerico::buscarIdRec(NodoCurso* nodo, int id) {
    if (nodo == nullptr) return nullptr;
    if (nodo->id_ == id) return nodo;
    for (NodoCurso* hijo : nodo->hijos_) {
        NodoCurso* encontrado = buscarIdRec(hijo, id);
        if (encontrado != nullptr) return encontrado;
    }
    return nullptr;
}

NodoCurso* ArbolGenerico::buscarPorId(int id) const {
    return buscarIdRec(raiz_, id);
}

std::vector<NodoCurso*> ArbolGenerico::buscarPorNombre(const std::string& fragmento) const {
    std::vector<NodoCurso*> resultado;
    std::string buscado = Utils::quitarEspacios(fragmento);
    if (buscado.empty()) return resultado;

    std::vector<NodoCurso*> todos = preorden();
    for (NodoCurso* nodo : todos) {
        if (Utils::contieneTexto(nodo->nombre_, buscado)) {
            resultado.push_back(nodo);
        }
    }
    return resultado;
}

int ArbolGenerico::posicion(const NodoCurso* nodo) const {
    if (nodo == nullptr || nodo->padre_ == nullptr) return 0;
    int contador = 1;
    for (const NodoCurso* hermano : nodo->padre_->hijos_) {
        if (hermano == nodo) return contador;
        contador++;
    }
    return 0;
}

// Se sube hasta la raíz anotando la posición de cada nodo en su nivel.
std::string ArbolGenerico::numeracion(const NodoCurso* nodo) const {
    std::vector<int> posiciones;
    while (nodo != nullptr && nodo->padre_ != nullptr) {
        posiciones.push_back(posicion(nodo));
        nodo = nodo->padre_;
    }
    std::ostringstream texto;
    for (std::size_t i = posiciones.size(); i > 0; i--) {
        texto << posiciones[i - 1];
        if (i > 1) texto << ".";
    }
    return texto.str();
}

// Baja desde la raíz: cada número elige un hijo.
NodoCurso* ArbolGenerico::buscarPorNumeracion(const std::string& texto) const {
    std::string limpio = Utils::aMinusculas(Utils::quitarEspacios(texto));
    if (limpio.empty()) return nullptr;
    if (limpio == "curso") return raiz_;

    NodoCurso* actual = raiz_;
    std::vector<std::string> partes = Utils::dividir(limpio, '.');
    for (std::size_t i = 0; i < partes.size(); i++) {
        long long numero;
        if (!Utils::aEntero(partes[i], numero) || numero < 1 ||
            numero > static_cast<long long>(actual->hijos_.size())) {
            return nullptr;
        }
        std::list<NodoCurso*>::const_iterator it = actual->hijos_.begin();
        std::advance(it, numero - 1);
        actual = *it;
    }
    return actual;
}

// Cuenta los pasos hasta la raíz.
int ArbolGenerico::nivel(const NodoCurso* nodo) const {
    if (nodo == nullptr) return 0;
    int nivel = 1;
    while (nodo->padre_ != nullptr) {
        nodo = nodo->padre_;
        nivel++;
    }
    return nivel;
}

int ArbolGenerico::contarSubarbol(const NodoCurso* nodo) {
    if (nodo == nullptr) return 0;
    int total = 1;
    for (const NodoCurso* hijo : nodo->hijos_) {
        total += contarSubarbol(hijo);
    }
    return total;
}

int ArbolGenerico::contarDescendientes(const NodoCurso* nodo) const {
    if (nodo == nullptr) return 0;
    return contarSubarbol(nodo) - 1;
}

int ArbolGenerico::alturaRec(const NodoCurso* nodo) {
    if (nodo == nullptr) return 0;
    int masAlto = 0;
    for (const NodoCurso* hijo : nodo->hijos_) {
        int h = alturaRec(hijo);
        if (h > masAlto) masAlto = h;
    }
    return 1 + masAlto;
}

int ArbolGenerico::altura() const {
    return alturaRec(raiz_);
}

void ArbolGenerico::preordenRec(NodoCurso* nodo, std::vector<NodoCurso*>& salida) {
    if (nodo == nullptr) return;
    salida.push_back(nodo);
    for (NodoCurso* hijo : nodo->hijos_) {
        preordenRec(hijo, salida);
    }
}

std::vector<NodoCurso*> ArbolGenerico::preorden() const {
    std::vector<NodoCurso*> salida;
    preordenRec(raiz_, salida);
    return salida;
}

std::vector<NodoCurso*> ArbolGenerico::hojas() const {
    std::vector<NodoCurso*> resultado;
    std::vector<NodoCurso*> todos = preorden();
    for (NodoCurso* nodo : todos) {
        if (nodo->hijos_.empty()) resultado.push_back(nodo);
    }
    return resultado;
}

// Sube por los padres y luego invierte para que quede de la raíz al nodo.
std::vector<const NodoCurso*> ArbolGenerico::ruta(const NodoCurso* nodo) const {
    std::vector<const NodoCurso*> camino;
    while (nodo != nullptr) {
        camino.push_back(nodo);
        nodo = nodo->padre_;
    }
    std::reverse(camino.begin(), camino.end());
    return camino;
}

std::vector<const NodoCurso*> ArbolGenerico::rutaEntre(const NodoCurso* origen,
                                                       const NodoCurso* destino) const {
    std::vector<const NodoCurso*> camino;
    if (!perteneceAlArbol(origen) || !perteneceAlArbol(destino)) return camino;

    // Los dos caminos desde la raíz comparten un tramo: llega hasta el ancestro común.
    std::vector<const NodoCurso*> hastaOrigen = ruta(origen);
    std::vector<const NodoCurso*> hastaDestino = ruta(destino);
    std::size_t comun = 0;
    while (comun < hastaOrigen.size() && comun < hastaDestino.size() &&
           hastaOrigen[comun] == hastaDestino[comun]) {
        comun++;
    }

    // Sube del origen hasta el ancestro común (incluido)...
    for (std::size_t i = hastaOrigen.size(); i >= comun; i--) {
        camino.push_back(hastaOrigen[i - 1]);
    }
    // ...y baja hasta el destino.
    for (std::size_t i = comun; i < hastaDestino.size(); i++) {
        camino.push_back(hastaDestino[i]);
    }
    return camino;
}

std::string ArbolGenerico::rutaComoTexto(const NodoCurso* nodo,
                                         const std::string& separador) const {
    std::vector<const NodoCurso*> camino = ruta(nodo);
    std::string texto;
    for (std::size_t i = 0; i < camino.size(); i++) {
        if (i > 0) texto += separador;
        texto += camino[i]->nombre_;
    }
    return texto;
}
