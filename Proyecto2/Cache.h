// Cache.h - cache con politica LRU (Least Recently Used: se saca la
// entrada que hace mas tiempo no se usa).
// Sirve para el cache local de cada proceso y para el cache global.
//
// Estructura interna:
//   - std::list guarda las entradas en orden de uso: el frente es la mas
//     reciente y el final es la menos reciente (la que se expulsa).
//   - std::unordered_map busca una entrada por (idProceso, paginaVirtual)
//     en tiempo casi constante, y guarda un iterador a la lista.
// Asi, buscar, insertar, mover al frente y expulsar cuestan O(1).

#ifndef CACHE_H
#define CACHE_H

#include <list>
#include <string>
#include <unordered_map>
#include <vector>

struct EntradaCache {
    std::string idProceso;
    int paginaVirtual;
    int paginaFisica;
    long long ultimaReferencia;   // "reloj" de la simulacion en el ultimo uso
};

class Cache {
private:
    int capacidad_;
    std::list<EntradaCache> orden_;
    std::unordered_map<std::string, std::list<EntradaCache>::iterator> indice_;

    // Texto unico para cada (idProceso, paginaVirtual). Ejemplo: "P1#2"
    static std::string claveDe(const std::string& idProceso, int paginaVirtual);

    // Saca la entrada menos reciente (la del final de la lista).
    void sacarMenosReciente();

public:
    explicit Cache(int capacidad = 1);

    // No se puede copiar: el indice apuntaria a la lista de otro cache.
    Cache(const Cache&) = delete;
    Cache& operator=(const Cache&) = delete;

    // Busca una entrada. Si existe, la marca como la mas reciente y devuelve
    // un puntero a ella. Si no existe, devuelve nullptr.
    EntradaCache* buscar(const std::string& idProceso, int paginaVirtual);

    // Agrega una entrada como la mas reciente. Si ya existia (mismo proceso y
    // pagina), la actualiza. Si el cache esta lleno, expulsa la menos reciente:
    // en ese caso devuelve true y, si 'expulsada' no es nullptr, copia ahi la
    // entrada expulsada.
    bool insertar(const EntradaCache& entrada, EntradaCache* expulsada);

    // Quita una entrada. Devuelve false si no estaba.
    bool eliminar(const std::string& idProceso, int paginaVirtual);

    // Quita todas las entradas de un proceso. Devuelve cuantas quito.
    int eliminarProceso(const std::string& idProceso);

    // Cambia la capacidad. Si queda mas pequeno, expulsa las menos recientes.
    // Devuelve cuantas expulso.
    int redimensionar(int nuevaCapacidad);

    // Copia en 'salida' la entrada menos reciente (la que LRU expulsaria),
    // sin quitarla. Devuelve false si el cache esta vacio.
    bool menosReciente(EntradaCache& salida) const;

    // Entradas en orden de uso: primero la mas reciente.
    std::vector<EntradaCache> contenido() const;

    void vaciar();
    int capacidad() const { return capacidad_; }
    int tamano() const { return static_cast<int>(orden_.size()); }
};

#endif
