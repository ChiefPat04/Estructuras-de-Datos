// MemoryManager.h - el administrador de memoria del simulador.
// Coordina:
//   - el AVL de paginas fisicas libres (clave = numero de pagina fisica),
//   - la tabla global de procesos (unordered_map: id -> Process),
//   - el cache global LRU,
//   - las metricas (accesos, hits y misses),
//   - extra: el reemplazo de paginas a memoria secundaria ("swap"). Esta
//     apagado por defecto; se enciende con PAGINAS_SWAP n (n > 0).
//
// Cada operacion devuelve true si salio bien. Si falla, devuelve false y
// deja en 'error' un mensaje que empieza con el tipo de error del enunciado
// (por ejemplo "Proceso inexistente: P9"). Cuando una operacion falla, la
// memoria queda exactamente como estaba.

#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <memory>
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>
#include "AVLTree.h"
#include "Cache.h"
#include "Page.h"
#include "Process.h"

// Lo que se informa de un ACCESS.
struct ResultadoAcceso {
    int paginaVirtual = 0;
    int desplazamiento = 0;
    int paginaFisica = -1;
    bool hitLocal = false;
    bool hitGlobal = false;
    bool falloPagina = false; // la pagina estaba en swap (solo con reemplazo activo)
    std::string detalle;      // texto para mostrar: "MISS en cache local, HIT en cache global"
};

struct Metricas {
    long long accesos = 0;
    long long hitsLocal = 0;
    long long missesLocal = 0;
    long long hitsGlobal = 0;
    long long missesGlobal = 0;
    long long fallosPagina = 0;   // extra: accesos a paginas que estaban en swap
    long long reemplazos = 0;     // extra: paginas que LRU paso de memoria fisica a swap
};

class MemoryManager {
private:
    long long memoriaTotal_;
    int tamanoPagina_;
    int totalPaginas_;
    AVLTree<int, Page> paginasLibres_;
    std::unordered_map<std::string, std::unique_ptr<Process> > procesos_;
    std::vector<std::string> ordenProcesos_;   // para mostrarlos en orden de creacion
    Cache cacheGlobal_;
    long long reloj_;                          // sube 1 en cada acceso
    Metricas metricas_;

    // ---- Extra: reemplazo de paginas ----
    int capacidadSwap_;                  // paginas que caben en swap (0 = sin reemplazo)
    int paginasEnSwap_;                  // paginas de procesos que estan ahora en swap
    Cache marcos_;                       // paginas en memoria fisica, en orden LRU:
                                         // la menos reciente es la victima del reemplazo
    std::vector<std::string> eventos_;   // fallos y reemplazos de la ultima operacion

    // nullptr si el proceso no existe.
    Process* buscarProceso(const std::string& id) const;

    // Toma 'cantidad' paginas del AVL (las de numero mas bajo) y las agrega
    // a la tabla del proceso. Antes hay que revisar que alcancen.
    void darPaginas(Process& proceso, long long cantidad);

    // Revisa si alcanzan las paginas para NEW_PROCESS o ALLOCATE (con o sin
    // swap). Si no alcanzan, deja el mensaje en 'error'.
    bool alcanzanPaginas(const std::string& id, long long necesarias,
                         bool adicionales, std::string& error) const;

    // Devuelve una pagina fisica libre (la de numero mas bajo). Si no hay
    // y el reemplazo esta activo, primero pasa a swap la victima LRU.
    int obtenerMarco();
    void expulsarVictima();

    // Registra que la pagina se acaba de usar (para elegir la victima LRU).
    void marcarUso(const std::string& id, int paginaVirtual, int paginaFisica);

public:
    MemoryManager(long long memoriaTotal, int tamanoPagina, int tamanoCacheGlobal);

    MemoryManager(const MemoryManager&) = delete;
    MemoryManager& operator=(const MemoryManager&) = delete;

    // Limites para que un archivo con valores enormes no congele el programa.
    static const int MAX_PAGINAS = 1048576;       // 2^20 paginas fisicas
    static const int MAX_TAMANO_PAGINA = 1073741824;  // 1 GB
    static const int MAX_CACHE_GLOBAL = 1048576;
    static const int MAX_PAGINAS_SWAP = 1048576;

    // Revisa los parametros iniciales antes de crear el simulador.
    // Recibe long long para poder detectar valores demasiado grandes.
    static bool configValida(long long memoriaTotal, long long tamanoPagina,
                             long long tamanoCacheGlobal, std::string& error);

    // Extra: activa el reemplazo de paginas con un swap de 'paginas' paginas
    // (0 lo deja apagado). Solo se puede antes de la primera operacion.
    bool configurarSwap(long long paginas, std::string& error);

    // ---- Operaciones del enunciado ----
    bool nuevoProceso(const std::string& id, long long memoria, std::string& error);
    bool finalizarProceso(const std::string& id, std::string& error);
    bool acceder(const std::string& id, long long direccion,
                 ResultadoAcceso& resultado, std::string& error);
    bool asignarMemoria(const std::string& id, long long bytes, std::string& error);
    bool liberarMemoria(const std::string& id, long long direccionInicial,
                        long long bytes, std::string& error);

    // ---- Consultas ----
    long long getMemoriaTotal() const { return memoriaTotal_; }
    int getTamanoPagina() const { return tamanoPagina_; }
    int paginasTotales() const { return totalPaginas_; }
    int paginasLibres() const { return paginasLibres_.contar(); }
    int paginasOcupadas() const { return totalPaginas_ - paginasLibres(); }
    double porcentajeUtilizacion() const;
    bool existeProceso(const std::string& id) const { return buscarProceso(id) != nullptr; }
    Metricas getMetricas() const { return metricas_; }
    const Cache& getCacheGlobal() const { return cacheGlobal_; }
    const Cache* cacheLocalDe(const std::string& id) const;   // nullptr si no existe
    int paginasDe(const std::string& id) const;               // -1 si no existe

    // ---- Extra: reemplazo de paginas ----
    bool reemplazoActivo() const { return capacidadSwap_ > 0; }
    int getCapacidadSwap() const { return capacidadSwap_; }
    int paginasEnSwap() const { return paginasEnSwap_; }
    bool estaEnSwap(const std::string& id, int paginaVirtual) const;
    int paginasEnSwapDe(const std::string& id) const;   // 0 si no existe
    // Fallos de pagina y reemplazos que ocurrieron en la ultima operacion.
    const std::vector<std::string>& getEventos() const { return eventos_; }

    // ---- Salida ----
    void mostrarEstado(std::ostream& salida) const;
    void mostrarMetricas(std::ostream& salida) const;

    // ---- Extras: visualizacion y exportacion ----
    void mostrarArbolLibres(std::ostream& salida) const;               // ARBOL AVL
    bool mostrarArbolProceso(const std::string& id, std::ostream& salida) const;  // ARBOL SPLAY
    void escribirMetricasCsv(std::ostream& salida) const;              // EXPORTAR
};

#endif
