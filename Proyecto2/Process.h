// Process.h - un proceso del simulador.
// Cada proceso tiene su tabla de paginas (un Splay Tree: clave = pagina
// virtual, valor = Page) y su propio cache local LRU.
//
// Las paginas virtuales se numeran 0, 1, 2, ... en el orden en que se
// asignan. 'siguientePaginaVirtual_' es el numero que recibira la proxima.
// Si FREE libera paginas del medio, quedan "huecos": esas paginas ya no
// estan en la tabla y acceder a ellas es un error.

#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include "Cache.h"
#include "Page.h"
#include "SplayTree.h"

class Process {
private:
    std::string id_;
    long long memoriaSolicitada_;   // bytes pedidos en NEW_PROCESS + ALLOCATE
    int siguientePaginaVirtual_;
    SplayTree<int, Page> tablaDePaginas_;
    Cache cacheLocal_;

public:
    Process(const std::string& id, long long memoriaSolicitada);

    // No se puede copiar: el arbol y el cache no se pueden copiar.
    Process(const Process&) = delete;
    Process& operator=(const Process&) = delete;

    const std::string& getId() const { return id_; }
    long long getMemoriaSolicitada() const { return memoriaSolicitada_; }
    void sumarMemoria(long long bytes) { memoriaSolicitada_ += bytes; }
    int getSiguientePaginaVirtual() const { return siguientePaginaVirtual_; }
    int paginasAsignadas() const { return tablaDePaginas_.contar(); }

    // Devuelve el numero de la proxima pagina virtual y avanza el contador.
    int reservarPaginaVirtual() { return siguientePaginaVirtual_++; }

    SplayTree<int, Page>& getTabla() { return tablaDePaginas_; }
    Cache& getCacheLocal() { return cacheLocal_; }
    const Cache& getCacheLocal() const { return cacheLocal_; }

    // Tamano maximo del cache local: max(1, floor(0.20 * paginasAsignadas)).
    static int capacidadCacheLocal(int paginasAsignadas);

    // Ajusta el cache local al numero actual de paginas asignadas.
    void actualizarCapacidadCache();
};

#endif
