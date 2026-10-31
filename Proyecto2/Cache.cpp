// ============================================================
// Cache.cpp  -  implementacion del cache LRU
// ============================================================

#include "Cache.h"

#include <sstream>

Cache::Cache(int capacidad) : capacidad_(capacidad < 0 ? 0 : capacidad) {}

std::string Cache::claveDe(const std::string& idProceso, int paginaVirtual) {
    // ostringstream en vez de std::to_string, por compatibilidad con MinGW antiguo.
    std::ostringstream texto;
    texto << idProceso << "#" << paginaVirtual;
    return texto.str();
}

void Cache::sacarMenosReciente() {
    if (orden_.empty()) return;
    const EntradaCache& ultima = orden_.back();
    indice_.erase(claveDe(ultima.idProceso, ultima.paginaVirtual));
    orden_.pop_back();
}

EntradaCache* Cache::buscar(const std::string& idProceso, int paginaVirtual) {
    auto it = indice_.find(claveDe(idProceso, paginaVirtual));
    if (it == indice_.end()) {
        return nullptr;
    }
    // Se mueve al frente de la lista: ahora es la mas reciente.
    // splice mueve el nodo sin copiarlo, y el iterador sigue valido.
    orden_.splice(orden_.begin(), orden_, it->second);
    return &(*it->second);
}

bool Cache::insertar(const EntradaCache& entrada, EntradaCache* expulsada) {
    if (capacidad_ == 0) {
        return false;   // un cache de tamano 0 no guarda nada
    }

    std::string clave = claveDe(entrada.idProceso, entrada.paginaVirtual);
    auto it = indice_.find(clave);
    if (it != indice_.end()) {
        // Ya estaba: se actualiza y pasa a ser la mas reciente.
        *(it->second) = entrada;
        orden_.splice(orden_.begin(), orden_, it->second);
        return false;
    }

    bool huboExpulsion = false;
    if (static_cast<int>(orden_.size()) >= capacidad_) {
        if (expulsada != nullptr) {
            *expulsada = orden_.back();
        }
        sacarMenosReciente();
        huboExpulsion = true;
    }

    orden_.push_front(entrada);
    indice_[clave] = orden_.begin();
    return huboExpulsion;
}

bool Cache::eliminar(const std::string& idProceso, int paginaVirtual) {
    auto it = indice_.find(claveDe(idProceso, paginaVirtual));
    if (it == indice_.end()) {
        return false;
    }
    orden_.erase(it->second);
    indice_.erase(it);
    return true;
}

int Cache::eliminarProceso(const std::string& idProceso) {
    int quitadas = 0;
    auto it = orden_.begin();
    while (it != orden_.end()) {
        if (it->idProceso == idProceso) {
            indice_.erase(claveDe(it->idProceso, it->paginaVirtual));
            it = orden_.erase(it);   // erase devuelve el siguiente elemento
            quitadas++;
        } else {
            ++it;
        }
    }
    return quitadas;
}

int Cache::redimensionar(int nuevaCapacidad) {
    capacidad_ = (nuevaCapacidad < 0) ? 0 : nuevaCapacidad;
    int expulsadas = 0;
    while (static_cast<int>(orden_.size()) > capacidad_) {
        sacarMenosReciente();
        expulsadas++;
    }
    return expulsadas;
}

std::vector<EntradaCache> Cache::contenido() const {
    return std::vector<EntradaCache>(orden_.begin(), orden_.end());
}

void Cache::vaciar() {
    orden_.clear();
    indice_.clear();
}

bool Cache::menosReciente(EntradaCache& salida) const {
    if (orden_.empty()) {
        return false;
    }
    salida = orden_.back();
    return true;
}
