// ============================================================
// MemoryManager.cpp  -  implementacion del administrador de memoria
// ============================================================

#include "MemoryManager.h"

#include <cstddef>
#include <iomanip>
#include <ios>
#include <sstream>
#include <utility>

// Funciones de ayuda que solo se usan en este archivo.
namespace {

    // Convierte un numero a texto (ostringstream: compatible con MinGW antiguo).
    std::string texto(long long numero) {
        std::ostringstream salida;
        salida << numero;
        return salida.str();
    }

    // Cuantas paginas hacen falta para 'bytes' bytes, redondeando hacia arriba.
    // Ejemplo con paginas de 1024: 4096 -> 4, 1500 -> 2.
    long long paginasPara(long long bytes, int tamanoPagina) {
        return (bytes + tamanoPagina - 1) / tamanoPagina;
    }

    // Muestra un cache como "[P1:2->2 P1:0->0]" (mas reciente primero).
    // Cada entrada es proceso:paginaVirtual->paginaFisica.
    std::string contenidoCache(const Cache& cache) {
        std::ostringstream salida;
        std::vector<EntradaCache> entradas = cache.contenido();
        salida << "[";
        for (std::size_t i = 0; i < entradas.size(); i++) {
            if (i > 0) salida << " ";
            salida << entradas[i].idProceso << ":" << entradas[i].paginaVirtual
                   << "->" << entradas[i].paginaFisica;
        }
        salida << "]";
        return salida.str();
    }

    // Paginas virtuales de un proceso que estan en swap: "[3 4]".
    std::string listaSwap(Process& proceso) {
        std::vector<std::pair<int, Page> > paginas = proceso.getTabla().recorrido();
        std::ostringstream salida;
        salida << "[";
        bool primera = true;
        for (std::size_t i = 0; i < paginas.size(); i++) {
            if (!paginas[i].second.esValida()) {
                if (!primera) salida << " ";
                salida << paginas[i].first;
                primera = false;
            }
        }
        salida << "]";
        return salida.str();
    }

    int contarEnSwap(Process& proceso) {
        std::vector<std::pair<int, Page> > paginas = proceso.getTabla().recorrido();
        int cantidad = 0;
        for (std::size_t i = 0; i < paginas.size(); i++) {
            if (!paginas[i].second.esValida()) cantidad++;
        }
        return cantidad;
    }
}

// ------------------------------------------------------------
// Construccion y configuracion
// ------------------------------------------------------------

MemoryManager::MemoryManager(long long memoriaTotal, int tamanoPagina,
                             int tamanoCacheGlobal)
    : memoriaTotal_(memoriaTotal), tamanoPagina_(tamanoPagina),
      totalPaginas_(static_cast<int>(memoriaTotal / tamanoPagina)),
      paginasLibres_(), procesos_(), ordenProcesos_(),
      cacheGlobal_(tamanoCacheGlobal), reloj_(0), metricas_(),
      capacidadSwap_(0), paginasEnSwap_(0), marcos_(totalPaginas_), eventos_() {
    // Al inicio todas las paginas fisicas estan libres.
    for (int fisica = 0; fisica < totalPaginas_; fisica++) {
        paginasLibres_.insertar(fisica, Page(-1, fisica, ""));
    }
}

bool MemoryManager::configValida(long long memoriaTotal, long long tamanoPagina,
                                 long long tamanoCacheGlobal, std::string& error) {
    if (tamanoPagina <= 0) {
        error = "Tamano de pagina invalido: TAMANO_PAGINA debe ser mayor que 0";
        return false;
    }
    if (tamanoPagina > MAX_TAMANO_PAGINA) {
        error = "Tamano de pagina invalido: TAMANO_PAGINA no puede pasar de "
              + texto(MAX_TAMANO_PAGINA) + " bytes";
        return false;
    }
    if (memoriaTotal <= 0) {
        error = "Archivo de entrada incorrecto: MEMORIA_TOTAL debe ser mayor que 0";
        return false;
    }
    if (tamanoPagina > memoriaTotal) {
        error = "Tamano de pagina invalido: TAMANO_PAGINA (" + texto(tamanoPagina)
              + ") es mayor que MEMORIA_TOTAL (" + texto(memoriaTotal) + ")";
        return false;
    }
    if (memoriaTotal % tamanoPagina != 0) {
        error = "Tamano de pagina invalido: MEMORIA_TOTAL (" + texto(memoriaTotal)
              + ") no es multiplo de TAMANO_PAGINA (" + texto(tamanoPagina) + ")";
        return false;
    }
    if (memoriaTotal / tamanoPagina > MAX_PAGINAS) {
        error = "Archivo de entrada incorrecto: la memoria tendria "
              + texto(memoriaTotal / tamanoPagina) + " paginas; el maximo es "
              + texto(MAX_PAGINAS);
        return false;
    }
    if (tamanoCacheGlobal <= 0) {
        error = "Archivo de entrada incorrecto: TAMANO_CACHE_GLOBAL debe ser mayor que 0";
        return false;
    }
    if (tamanoCacheGlobal > MAX_CACHE_GLOBAL) {
        error = "Archivo de entrada incorrecto: TAMANO_CACHE_GLOBAL no puede pasar de "
              + texto(MAX_CACHE_GLOBAL);
        return false;
    }
    return true;
}

// ------------------------------------------------------------
// Ayudas privadas
// ------------------------------------------------------------

Process* MemoryManager::buscarProceso(const std::string& id) const {
    auto it = procesos_.find(id);
    return (it == procesos_.end()) ? nullptr : it->second.get();
}

void MemoryManager::darPaginas(Process& proceso, long long cantidad) {
    for (long long i = 0; i < cantidad; i++) {
        int fisica = obtenerMarco();                    // la pagina libre mas baja
        int virtual_ = proceso.reservarPaginaVirtual();
        proceso.getTabla().insertar(virtual_, Page(virtual_, fisica, proceso.getId()));
        marcarUso(proceso.getId(), virtual_, fisica);
    }
    proceso.actualizarCapacidadCache();
}

bool MemoryManager::alcanzanPaginas(const std::string& id, long long necesarias,
                                    bool adicionales, std::string& error) const {
    std::string paginasTexto = adicionales ? " paginas mas" : " paginas";

    if (!reemplazoActivo()) {
        if (necesarias > paginasLibres()) {
            error = "Memoria insuficiente: " + id + " necesita " + texto(necesarias)
                  + paginasTexto + " y solo hay " + texto(paginasLibres()) + " libres";
            return false;
        }
        return true;
    }

    // Con reemplazo: cada pagina que falte en memoria fisica obliga a pasar
    // otra a swap, asi que alcanzan las libres de memoria fisica mas las
    // libres de swap. Las paginas nuevas siempre entran a memoria fisica, por
    // eso una sola operacion no puede pedir mas paginas que las que hay.
    long long libresSwap = capacidadSwap_ - paginasEnSwap_;
    if (necesarias > totalPaginas_) {
        error = "Memoria insuficiente: " + id + " necesita " + texto(necesarias) + paginasTexto
              + " y la memoria fisica solo tiene " + texto(totalPaginas_);
        return false;
    }
    if (necesarias > paginasLibres() + libresSwap) {
        error = "Memoria insuficiente: " + id + " necesita " + texto(necesarias) + paginasTexto
              + "; hay " + texto(paginasLibres()) + " libres en memoria fisica y "
              + texto(libresSwap) + " libres en swap";
        return false;
    }
    return true;
}

// ------------------------------------------------------------
// Extra: reemplazo de paginas
// ------------------------------------------------------------

bool MemoryManager::configurarSwap(long long paginas, std::string& error) {
    if (paginas < 0 || paginas > MAX_PAGINAS_SWAP) {
        error = "Archivo de entrada incorrecto: PAGINAS_SWAP debe estar entre 0 y "
              + texto(MAX_PAGINAS_SWAP);
        return false;
    }
    if (!procesos_.empty() || metricas_.accesos > 0) {
        error = "Archivo de entrada incorrecto: PAGINAS_SWAP debe ir antes de la primera operacion";
        return false;
    }
    capacidadSwap_ = static_cast<int>(paginas);
    return true;
}

int MemoryManager::obtenerMarco() {
    if (paginasLibres_.vacio() && reemplazoActivo()) {
        expulsarVictima();
    }
    int fisica = -1;
    Page libre;
    paginasLibres_.extraerMinimo(fisica, libre);
    return fisica;
}

void MemoryManager::expulsarVictima() {
    // La victima es la pagina en memoria fisica usada hace mas tiempo (LRU).
    EntradaCache victima;
    if (!marcos_.menosReciente(victima)) return;
    marcos_.eliminar(victima.idProceso, victima.paginaVirtual);

    Process* duenio = buscarProceso(victima.idProceso);
    if (duenio == nullptr) return;
    Page* pagina = duenio->getTabla().buscarSinSplay(victima.paginaVirtual);
    if (pagina == nullptr) return;

    int fisica = pagina->getPaginaFisica();
    pagina->pasarASwap();
    paginasEnSwap_++;
    metricas_.reemplazos++;

    // Ningun cache puede apuntar a una pagina que ya no esta en memoria fisica.
    duenio->getCacheLocal().eliminar(victima.idProceso, victima.paginaVirtual);
    cacheGlobal_.eliminar(victima.idProceso, victima.paginaVirtual);

    paginasLibres_.insertar(fisica, Page(-1, fisica, ""));
    eventos_.push_back("Reemplazo de pagina: LRU pasa " + victima.idProceso + ":"
                       + texto(victima.paginaVirtual) + " (pagina fisica "
                       + texto(fisica) + ") a swap");
}

void MemoryManager::marcarUso(const std::string& id, int paginaVirtual, int paginaFisica) {
    EntradaCache entrada;
    entrada.idProceso = id;
    entrada.paginaVirtual = paginaVirtual;
    entrada.paginaFisica = paginaFisica;
    entrada.ultimaReferencia = reloj_;
    marcos_.insertar(entrada, nullptr);
}

int MemoryManager::paginasEnSwapDe(const std::string& id) const {
    Process* proceso = buscarProceso(id);
    return (proceso == nullptr) ? 0 : contarEnSwap(*proceso);
}

bool MemoryManager::estaEnSwap(const std::string& id, int paginaVirtual) const {
    Process* proceso = buscarProceso(id);
    if (proceso == nullptr) return false;
    Page* pagina = proceso->getTabla().buscarSinSplay(paginaVirtual);
    return pagina != nullptr && !pagina->esValida();
}

// ------------------------------------------------------------
// NEW_PROCESS idProceso memoria
// ------------------------------------------------------------

bool MemoryManager::nuevoProceso(const std::string& id, long long memoria,
                                 std::string& error) {
    eventos_.clear();
    if (id.empty()) {
        error = "Archivo de entrada incorrecto: falta el id del proceso";
        return false;
    }
    if (buscarProceso(id) != nullptr) {
        error = "Proceso duplicado: " + id + " ya existe";
        return false;
    }
    if (memoria <= 0) {
        error = "Archivo de entrada incorrecto: la memoria debe ser mayor que 0";
        return false;
    }

    long long necesarias = paginasPara(memoria, tamanoPagina_);
    if (!alcanzanPaginas(id, necesarias, false, error)) {
        return false;
    }

    std::unique_ptr<Process> nuevo(new Process(id, memoria));
    darPaginas(*nuevo, necesarias);
    procesos_.emplace(id, std::move(nuevo));
    ordenProcesos_.push_back(id);
    return true;
}

// ------------------------------------------------------------
// END_PROCESS idProceso
// ------------------------------------------------------------

bool MemoryManager::finalizarProceso(const std::string& id, std::string& error) {
    eventos_.clear();
    Process* proceso = buscarProceso(id);
    if (proceso == nullptr) {
        error = "Proceso inexistente: " + id;
        return false;
    }

    // 1 y 5. Devolver todas sus paginas fisicas al AVL de libres.
    std::vector<std::pair<int, Page> > paginas = proceso->getTabla().recorrido();
    for (std::size_t i = 0; i < paginas.size(); i++) {
        const Page& pagina = paginas[i].second;
        if (pagina.esValida()) {
            int fisica = pagina.getPaginaFisica();
            paginasLibres_.insertar(fisica, Page(-1, fisica, ""));
            marcos_.eliminar(id, paginas[i].first);
        } else {
            paginasEnSwap_--;   // extra: estaba en swap; se libera su lugar
        }
    }

    // 4. Quitar sus entradas del cache global.
    cacheGlobal_.eliminarProceso(id);

    // 2 y 3. Al borrar el proceso se destruyen su tabla y su cache local.
    procesos_.erase(id);
    for (std::size_t i = 0; i < ordenProcesos_.size(); i++) {
        if (ordenProcesos_[i] == id) {
            ordenProcesos_.erase(ordenProcesos_.begin() + static_cast<long>(i));
            break;
        }
    }
    return true;
}

// ------------------------------------------------------------
// ACCESS idProceso direccion
// ------------------------------------------------------------

bool MemoryManager::acceder(const std::string& id, long long direccion,
                            ResultadoAcceso& resultado, std::string& error) {
    eventos_.clear();
    // 1. El proceso debe existir.
    Process* proceso = buscarProceso(id);
    if (proceso == nullptr) {
        error = "Proceso inexistente: " + id;
        return false;
    }
    if (direccion < 0) {
        error = "Direccion invalida: " + texto(direccion) + " es negativa";
        return false;
    }

    long long paginaLarga = direccion / tamanoPagina_;
    int desplazamiento = static_cast<int>(direccion % tamanoPagina_);

    // 2. La pagina virtual debe estar dentro del espacio que se le dio al proceso.
    if (paginaLarga >= proceso->getSiguientePaginaVirtual()) {
        error = "Direccion invalida: " + texto(direccion) + " (pagina virtual "
              + texto(paginaLarga) + ") esta fuera del espacio de " + id;
        return false;
    }
    int paginaVirtual = static_cast<int>(paginaLarga);

    // 3. Buscar primero en el cache local.
    EntradaCache* enLocal = proceso->getCacheLocal().buscar(id, paginaVirtual);

    // 4. Si no esta, buscar en el cache global.
    EntradaCache* enGlobal = nullptr;
    if (enLocal == nullptr) {
        enGlobal = cacheGlobal_.buscar(id, paginaVirtual);
    }

    // 5 y 7. Buscar en el Splay Tree. Buscar hace splay: la pagina sube a la raiz.
    // Se hace en todos los accesos, aunque haya sido hit en un cache.
    Page* pagina = proceso->getTabla().buscar(paginaVirtual);
    if (pagina == nullptr) {
        // La pagina estaba dentro del espacio, pero se libero con FREE.
        error = "Acceso a pagina no valida: la pagina virtual " + texto(paginaVirtual)
              + " de " + id + " fue liberada";
        return false;
    }

    // Extra (reemplazo de paginas): si la pagina esta en swap es un fallo de
    // pagina. Se trae a memoria fisica; si no hay paginas fisicas libres, LRU
    // pasa otra a swap. (Una pagina en swap nunca esta en un cache.)
    if (!pagina->esValida()) {
        metricas_.fallosPagina++;
        paginasEnSwap_--;
        eventos_.push_back("Fallo de pagina: " + id + ":" + texto(paginaVirtual)
                           + " estaba en swap");
        int fisica = obtenerMarco();
        pagina->cargarEnMarco(fisica);
        eventos_.push_back(id + ":" + texto(paginaVirtual)
                           + " se cargo en la pagina fisica " + texto(fisica));
        resultado.falloPagina = true;
    }

    // 6. Actualizar metricas y la informacion de la pagina.
    reloj_++;
    metricas_.accesos++;
    pagina->registrarReferencia(reloj_);
    marcarUso(id, paginaVirtual, pagina->getPaginaFisica());

    resultado.paginaVirtual = paginaVirtual;
    resultado.desplazamiento = desplazamiento;
    resultado.paginaFisica = pagina->getPaginaFisica();
    resultado.hitLocal = (enLocal != nullptr);
    resultado.hitGlobal = (enGlobal != nullptr);

    if (enLocal != nullptr) {
        metricas_.hitsLocal++;
        enLocal->ultimaReferencia = reloj_;
        resultado.detalle = "HIT en cache local";
        return true;
    }

    // 8. Miss local: la pagina entra a los caches (LRU expulsa si estan llenos).
    metricas_.missesLocal++;

    EntradaCache nueva;
    nueva.idProceso = id;
    nueva.paginaVirtual = paginaVirtual;
    nueva.paginaFisica = pagina->getPaginaFisica();
    nueva.ultimaReferencia = reloj_;

    EntradaCache expulsada;
    if (enGlobal != nullptr) {
        metricas_.hitsGlobal++;
        enGlobal->ultimaReferencia = reloj_;
        resultado.detalle = "MISS en cache local, HIT en cache global";
    } else {
        metricas_.missesGlobal++;
        resultado.detalle = "MISS en cache local, MISS en cache global (se obtuvo del Splay Tree)";
        if (cacheGlobal_.insertar(nueva, &expulsada)) {
            resultado.detalle += "\n  Cache global lleno: LRU expulsa " + expulsada.idProceso
                               + ":" + texto(expulsada.paginaVirtual);
        }
    }

    if (proceso->getCacheLocal().insertar(nueva, &expulsada)) {
        resultado.detalle += "\n  Cache local lleno: LRU expulsa " + expulsada.idProceso
                           + ":" + texto(expulsada.paginaVirtual);
    }
    return true;
}

// ------------------------------------------------------------
// ALLOCATE idProceso bytes
// ------------------------------------------------------------

bool MemoryManager::asignarMemoria(const std::string& id, long long bytes,
                                   std::string& error) {
    eventos_.clear();
    Process* proceso = buscarProceso(id);
    if (proceso == nullptr) {
        error = "Proceso inexistente: " + id;
        return false;
    }
    if (bytes <= 0) {
        error = "Archivo de entrada incorrecto: la memoria a asignar debe ser mayor que 0";
        return false;
    }

    long long necesarias = paginasPara(bytes, tamanoPagina_);
    if (!alcanzanPaginas(id, necesarias, true, error)) {
        return false;
    }

    darPaginas(*proceso, necesarias);   // tambien ajusta el cache local
    proceso->sumarMemoria(bytes);
    return true;
}

// ------------------------------------------------------------
// FREE idProceso direccionInicial bytes
// ------------------------------------------------------------

bool MemoryManager::liberarMemoria(const std::string& id, long long direccionInicial,
                                   long long bytes, std::string& error) {
    eventos_.clear();
    Process* proceso = buscarProceso(id);
    if (proceso == nullptr) {
        error = "Proceso inexistente: " + id;
        return false;
    }
    if (direccionInicial < 0) {
        error = "Direccion invalida: " + texto(direccionInicial) + " es negativa";
        return false;
    }
    if (bytes <= 0) {
        error = "Liberacion de memoria no asignada: la cantidad de bytes debe ser mayor que 0";
        return false;
    }

    // 1. Rango de paginas virtuales afectadas.
    long long primera = direccionInicial / tamanoPagina_;
    long long ultima = (direccionInicial + bytes - 1) / tamanoPagina_;

    // Primero se revisa TODO el rango. Si alguna pagina no esta asignada,
    // no se libera nada (asi la memoria no queda a medias).
    for (long long v = primera; v <= ultima; v++) {
        bool asignada = v < proceso->getSiguientePaginaVirtual()
                     && proceso->getTabla().contiene(static_cast<int>(v));
        if (!asignada) {
            error = "Liberacion de memoria no asignada: la pagina virtual " + texto(v)
                  + " de " + id + " no esta asignada";
            return false;
        }
    }

    for (long long v = primera; v <= ultima; v++) {
        int paginaVirtual = static_cast<int>(v);
        Page* pagina = proceso->getTabla().buscar(paginaVirtual);
        if (pagina->esValida()) {
            int fisica = pagina->getPaginaFisica();
            paginasLibres_.insertar(fisica, Page(-1, fisica, ""));    // 3 y 4. AVL de libres
            marcos_.eliminar(id, paginaVirtual);
        } else {
            paginasEnSwap_--;   // extra: estaba en swap; se libera su lugar
        }

        proceso->getTabla().eliminar(paginaVirtual);              // 2. Splay Tree
        proceso->getCacheLocal().eliminar(id, paginaVirtual);     // 5. caches
        cacheGlobal_.eliminar(id, paginaVirtual);
    }

    proceso->actualizarCapacidadCache();                          // 6.
    return true;
}

// ------------------------------------------------------------
// Consultas y salida
// ------------------------------------------------------------

double MemoryManager::porcentajeUtilizacion() const {
    if (totalPaginas_ == 0) return 0.0;
    return 100.0 * paginasOcupadas() / totalPaginas_;
}

const Cache* MemoryManager::cacheLocalDe(const std::string& id) const {
    const Process* proceso = buscarProceso(id);
    return (proceso == nullptr) ? nullptr : &proceso->getCacheLocal();
}

int MemoryManager::paginasDe(const std::string& id) const {
    const Process* proceso = buscarProceso(id);
    return (proceso == nullptr) ? -1 : proceso->paginasAsignadas();
}

void MemoryManager::mostrarEstado(std::ostream& salida) const {
    salida << "  Paginas fisicas: " << totalPaginas_
           << " | libres: " << paginasLibres()
           << " | ocupadas: " << paginasOcupadas() << "\n";
    if (reemplazoActivo()) {
        salida << "  Swap (memoria secundaria): " << paginasEnSwap_ << "/"
               << capacidadSwap_ << " paginas usadas\n";
    }

    salida << "  Paginas libres (AVL en orden): ";
    std::vector<std::pair<int, Page> > libres = paginasLibres_.inOrden();
    if (libres.empty()) salida << "ninguna";
    for (std::size_t i = 0; i < libres.size(); i++) {
        if (i > 0) salida << " ";
        salida << libres[i].first;
    }
    salida << "\n";

    salida << "  Procesos activos:";
    if (ordenProcesos_.empty()) salida << " ninguno";
    salida << "\n";
    for (std::size_t i = 0; i < ordenProcesos_.size(); i++) {
        const Process* proceso = buscarProceso(ordenProcesos_[i]);
        salida << "    " << proceso->getId()
               << " | memoria solicitada: " << proceso->getMemoriaSolicitada() << " bytes"
               << " | paginas: " << proceso->paginasAsignadas()
               << " | cache local (" << proceso->getCacheLocal().tamano() << "/"
               << proceso->getCacheLocal().capacidad() << "): "
               << contenidoCache(proceso->getCacheLocal());
        if (reemplazoActivo()) {
            salida << " | en swap: " << listaSwap(*buscarProceso(ordenProcesos_[i]));
        }
        salida << "\n";
    }

    salida << "  Cache global (" << cacheGlobal_.tamano() << "/"
           << cacheGlobal_.capacidad() << "): " << contenidoCache(cacheGlobal_) << "\n";

    salida << "  Hits/misses -> local: " << metricas_.hitsLocal << "/" << metricas_.missesLocal
           << " | global: " << metricas_.hitsGlobal << "/" << metricas_.missesGlobal << "\n";
    if (reemplazoActivo()) {
        salida << "  Fallos de pagina: " << metricas_.fallosPagina
               << " | reemplazos: " << metricas_.reemplazos << "\n";
    }
}

void MemoryManager::mostrarMetricas(std::ostream& salida) const {
    salida << "  Total de accesos:           " << metricas_.accesos << "\n";
    salida << "  Hits en cache local:        " << metricas_.hitsLocal << "\n";
    salida << "  Misses en cache local:      " << metricas_.missesLocal << "\n";
    salida << "  Hits en cache global:       " << metricas_.hitsGlobal << "\n";
    salida << "  Misses en cache global:     " << metricas_.missesGlobal << "\n";
    salida << "  Paginas asignadas:          " << paginasOcupadas() << "\n";
    salida << "  Paginas libres:             " << paginasLibres() << "\n";
    if (reemplazoActivo()) {
        salida << "  Fallos de pagina:           " << metricas_.fallosPagina << "\n";
        salida << "  Paginas reemplazadas:       " << metricas_.reemplazos << "\n";
        salida << "  Paginas en swap:            " << paginasEnSwap_ << "/" << capacidadSwap_ << "\n";
    }

    // Se guarda el formato del flujo y se restaura al final, para no afectar
    // lo que se imprima despues.
    std::ios_base::fmtflags formatoAnterior = salida.flags();
    std::streamsize precisionAnterior = salida.precision();
    salida << "  Utilizacion de memoria:     " << std::fixed << std::setprecision(2)
           << porcentajeUtilizacion() << "%\n";
    salida.flags(formatoAnterior);
    salida.precision(precisionAnterior);
}

// ------------------------------------------------------------
// Extras: visualizacion y exportacion
// ------------------------------------------------------------

void MemoryManager::mostrarArbolLibres(std::ostream& salida) const {
    if (paginasLibres_.vacio()) {
        salida << "  (no hay paginas libres)\n";
        return;
    }
    paginasLibres_.mostrar(salida);
}

bool MemoryManager::mostrarArbolProceso(const std::string& id, std::ostream& salida) const {
    Process* proceso = buscarProceso(id);
    if (proceso == nullptr) return false;
    if (proceso->getTabla().vacio()) {
        salida << "  (el proceso no tiene paginas asignadas)\n";
        return true;
    }
    proceso->getTabla().mostrar(salida);
    return true;
}

void MemoryManager::escribirMetricasCsv(std::ostream& salida) const {
    // Primera tabla: metricas generales (dos columnas: metrica, valor).
    salida << "metrica,valor\n";
    salida << "memoria_total," << memoriaTotal_ << "\n";
    salida << "tamano_pagina," << tamanoPagina_ << "\n";
    salida << "paginas_fisicas," << totalPaginas_ << "\n";
    salida << "accesos_totales," << metricas_.accesos << "\n";
    salida << "hits_cache_local," << metricas_.hitsLocal << "\n";
    salida << "misses_cache_local," << metricas_.missesLocal << "\n";
    salida << "hits_cache_global," << metricas_.hitsGlobal << "\n";
    salida << "misses_cache_global," << metricas_.missesGlobal << "\n";
    salida << "paginas_asignadas," << paginasOcupadas() << "\n";
    salida << "paginas_libres," << paginasLibres() << "\n";
    if (reemplazoActivo()) {
        salida << "paginas_swap_capacidad," << capacidadSwap_ << "\n";
        salida << "paginas_en_swap," << paginasEnSwap_ << "\n";
        salida << "fallos_de_pagina," << metricas_.fallosPagina << "\n";
        salida << "paginas_reemplazadas," << metricas_.reemplazos << "\n";
    }

    std::ios_base::fmtflags formatoAnterior = salida.flags();
    std::streamsize precisionAnterior = salida.precision();
    salida << "utilizacion_porcentaje," << std::fixed << std::setprecision(2)
           << porcentajeUtilizacion() << "\n";
    salida.flags(formatoAnterior);
    salida.precision(precisionAnterior);

    // Segunda tabla, separada por una linea vacia: un renglon por proceso activo.
    salida << "\n";
    salida << "proceso,memoria_solicitada,paginas_asignadas,cache_local_usada,cache_local_capacidad";
    if (reemplazoActivo()) salida << ",paginas_en_swap";
    salida << "\n";
    for (std::size_t i = 0; i < ordenProcesos_.size(); i++) {
        Process* proceso = buscarProceso(ordenProcesos_[i]);
        salida << proceso->getId() << "," << proceso->getMemoriaSolicitada() << ","
               << proceso->paginasAsignadas() << ","
               << proceso->getCacheLocal().tamano() << ","
               << proceso->getCacheLocal().capacidad();
        if (reemplazoActivo()) salida << "," << contarEnSwap(*proceso);
        salida << "\n";
    }
}
