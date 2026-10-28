// ============================================================
// Process.cpp  -  implementacion de la clase Process
// ============================================================

#include "Process.h"

Process::Process(const std::string& id, long long memoriaSolicitada)
    : id_(id), memoriaSolicitada_(memoriaSolicitada),
      siguientePaginaVirtual_(0), tablaDePaginas_(), cacheLocal_(1) {}

// floor(0.20 * n) es lo mismo que n / 5 con division entera (para n >= 0),
// asi se evita usar decimales. Si el resultado es 0, se usa 1.
int Process::capacidadCacheLocal(int paginasAsignadas) {
    int capacidad = paginasAsignadas / 5;
    return (capacidad < 1) ? 1 : capacidad;
}

void Process::actualizarCapacidadCache() {
    cacheLocal_.redimensionar(capacidadCacheLocal(paginasAsignadas()));
}
