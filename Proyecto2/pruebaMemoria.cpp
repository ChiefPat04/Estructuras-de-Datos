// pruebaMemoria.cpp - pruebas del simulador (Page, Process y MemoryManager).
// Cubre los 12 casos de prueba minimos del enunciado.
// Tiene su propio main, se compila aparte con probar_memoria.bat

#include <iostream>
#include <string>
#include <vector>
#include "MemoryManager.h"

static int pasadas = 0;
static int falladas = 0;

// Registra el resultado de una comprobacion.
static void verificar(bool condicion, const std::string& descripcion) {
    if (condicion) {
        pasadas++;
        std::cout << "  [OK]    " << descripcion << "\n";
    } else {
        falladas++;
        std::cout << "  [FALLO] " << descripcion << "\n";
    }
}

// Dice si el texto empieza con 'inicio' (para revisar el tipo de error).
static bool empiezaCon(const std::string& texto, const std::string& inicio) {
    return texto.compare(0, inicio.size(), inicio) == 0;
}

// Dice si el cache tiene la entrada (proceso, paginaVirtual).
static bool cacheTiene(const Cache& cache, const std::string& id, int pagina) {
    std::vector<EntradaCache> entradas = cache.contenido();
    for (std::size_t i = 0; i < entradas.size(); i++) {
        if (entradas[i].idProceso == id && entradas[i].paginaVirtual == pagina) return true;
    }
    return false;
}

// Configuracion del enunciado: 65536 / 1024 = 64 paginas fisicas.
static const long long MEMORIA = 65536;
static const int PAGINA = 1024;

static void caso01CrearProcesoValido() {
    std::cout << "\n== 1. Creacion de un proceso valido ==\n";
    MemoryManager m(MEMORIA, PAGINA, 8);
    std::string error;
    verificar(m.paginasTotales() == 64, "hay 64 paginas fisicas");
    verificar(m.nuevoProceso("P1", 4096, error), "NEW_PROCESS P1 4096 funciona");
    verificar(m.existeProceso("P1"), "P1 queda registrado");
    verificar(m.paginasDe("P1") == 4, "P1 tiene 4 paginas (4096 / 1024)");
    verificar(m.paginasLibres() == 60 && m.paginasOcupadas() == 4, "quedan 60 libres y 4 ocupadas");
    verificar(m.cacheLocalDe("P1")->capacidad() == 1, "cache local de P1 = max(1, floor(4*0.2)) = 1");
    verificar(m.nuevoProceso("P2", 1500, error) && m.paginasDe("P2") == 2,
              "1500 bytes redondea hacia arriba a 2 paginas");
    verificar(!m.nuevoProceso("P1", 1024, error) && empiezaCon(error, "Proceso duplicado"),
              "crear P1 otra vez da 'Proceso duplicado'");
}

static void caso02MemoriaInsuficiente() {
    std::cout << "\n== 2. Creacion de proceso con memoria insuficiente ==\n";
    MemoryManager m(MEMORIA, PAGINA, 8);
    std::string error;
    m.nuevoProceso("P1", 4096, error);
    verificar(!m.nuevoProceso("P2", 100000, error), "NEW_PROCESS P2 100000 falla");
    verificar(empiezaCon(error, "Memoria insuficiente"), "el error es 'Memoria insuficiente'");
    verificar(!m.existeProceso("P2"), "P2 no se crea");
    verificar(m.paginasLibres() == 60, "las paginas libres no cambian (60)");
}

static void caso03FinalizarProceso() {
    std::cout << "\n== 3. Finalizacion de proceso ==\n";
    MemoryManager m(MEMORIA, PAGINA, 8);
    std::string error;
    ResultadoAcceso r;
    m.nuevoProceso("P1", 4096, error);
    m.acceder("P1", 0, r, error);
    verificar(m.getCacheGlobal().tamano() == 1, "antes de terminar, P1 tiene 1 entrada en el cache global");
    verificar(m.finalizarProceso("P1", error), "END_PROCESS P1 funciona");
    verificar(!m.existeProceso("P1"), "P1 ya no existe");
    verificar(m.paginasLibres() == 64, "sus 4 paginas vuelven al AVL (64 libres)");
    verificar(m.getCacheGlobal().tamano() == 0, "sus entradas salen del cache global");
    verificar(!m.finalizarProceso("P1", error) && empiezaCon(error, "Proceso inexistente"),
              "terminar P1 otra vez da 'Proceso inexistente'");
}

static void caso04AccesoExitoso() {
    std::cout << "\n== 4. Acceso exitoso a pagina ==\n";
    MemoryManager m(MEMORIA, PAGINA, 8);
    std::string error;
    ResultadoAcceso r;
    m.nuevoProceso("P1", 4096, error);

    verificar(m.acceder("P1", 2500, r, error), "ACCESS P1 2500 funciona");
    verificar(r.paginaVirtual == 2 && r.desplazamiento == 452,
              "pagina virtual 2, desplazamiento 452 (2500 = 2*1024 + 452)");
    verificar(r.paginaFisica == 2, "pagina fisica 2 (P1 recibio las fisicas 0, 1, 2, 3)");
    verificar(!r.hitLocal && !r.hitGlobal, "el primer acceso es MISS en ambos caches");

    verificar(m.acceder("P1", 2100, r, error) && r.hitLocal, "ACCESS P1 2100 (misma pagina) es HIT local");

    Metricas met = m.getMetricas();
    verificar(met.accesos == 2, "total de accesos = 2");
    verificar(met.hitsLocal == 1 && met.missesLocal == 1, "local: 1 hit y 1 miss");
    verificar(met.hitsGlobal == 0 && met.missesGlobal == 1, "global: 0 hits y 1 miss");
}

static void caso05DireccionInvalida() {
    std::cout << "\n== 5. Acceso a direccion invalida ==\n";
    MemoryManager m(MEMORIA, PAGINA, 8);
    std::string error;
    ResultadoAcceso r;
    m.nuevoProceso("P1", 4096, error);
    verificar(!m.acceder("P1", -5, r, error) && empiezaCon(error, "Direccion invalida"),
              "ACCESS P1 -5 da 'Direccion invalida'");
    verificar(!m.acceder("P1", 9000, r, error) && empiezaCon(error, "Direccion invalida"),
              "ACCESS P1 9000 (pagina 8, P1 solo tiene 0-3) da 'Direccion invalida'");
    verificar(!m.acceder("P9", 0, r, error) && empiezaCon(error, "Proceso inexistente"),
              "ACCESS P9 0 da 'Proceso inexistente'");
    verificar(m.getMetricas().accesos == 0, "los accesos con error no cuentan en las metricas");
}

static void caso06AsignacionAdicional() {
    std::cout << "\n== 6. Asignacion adicional de memoria ==\n";
    MemoryManager m(MEMORIA, PAGINA, 8);
    std::string error;
    ResultadoAcceso r;
    m.nuevoProceso("P1", 4096, error);
    verificar(m.asignarMemoria("P1", 2048, error), "ALLOCATE P1 2048 funciona");
    verificar(m.paginasDe("P1") == 6 && m.paginasLibres() == 58, "P1 pasa a 6 paginas, quedan 58 libres");
    verificar(m.acceder("P1", 5000, r, error) && r.paginaVirtual == 4,
              "la nueva pagina virtual 4 se puede acceder");
    verificar(m.asignarMemoria("P1", 4096, error) && m.cacheLocalDe("P1")->capacidad() == 2,
              "con 10 paginas el cache local sube a 2");
    verificar(!m.asignarMemoria("P1", 100000, error) && empiezaCon(error, "Memoria insuficiente"),
              "ALLOCATE demasiado grande da 'Memoria insuficiente'");
    verificar(!m.asignarMemoria("P9", 1024, error) && empiezaCon(error, "Proceso inexistente"),
              "ALLOCATE a un proceso que no existe da 'Proceso inexistente'");
}

static void caso07LiberacionParcial() {
    std::cout << "\n== 7. Liberacion parcial de memoria ==\n";
    MemoryManager m(MEMORIA, PAGINA, 8);
    std::string error;
    ResultadoAcceso r;
    m.nuevoProceso("P1", 4096, error);
    m.acceder("P1", 2500, r, error);   // la pagina 2 entra a los caches

    verificar(m.liberarMemoria("P1", 2048, 2048, error), "FREE P1 2048 2048 funciona");
    verificar(m.paginasDe("P1") == 2 && m.paginasLibres() == 62,
              "se liberan las paginas 2 y 3: P1 queda con 2 y hay 62 libres");
    verificar(!cacheTiene(m.getCacheGlobal(), "P1", 2), "la pagina 2 sale del cache global");
    verificar(!cacheTiene(*m.cacheLocalDe("P1"), "P1", 2), "la pagina 2 sale del cache local");
    verificar(!m.acceder("P1", 2500, r, error) && empiezaCon(error, "Acceso a pagina no valida"),
              "ACCESS a la pagina liberada da 'Acceso a pagina no valida'");
    verificar(m.acceder("P1", 100, r, error), "la pagina 0 sigue funcionando");
}

static void caso08LiberacionInvalida() {
    std::cout << "\n== 8. Liberacion invalida ==\n";
    MemoryManager m(MEMORIA, PAGINA, 8);
    std::string error;
    m.nuevoProceso("P1", 4096, error);
    verificar(!m.liberarMemoria("P1", 0, 100000, error)
              && empiezaCon(error, "Liberacion de memoria no asignada"),
              "FREE fuera del espacio del proceso da 'Liberacion de memoria no asignada'");
    verificar(m.paginasDe("P1") == 4, "no se libero nada: P1 sigue con 4 paginas");

    m.liberarMemoria("P1", 1024, 1024, error);   // libera la pagina 1
    verificar(!m.liberarMemoria("P1", 0, 2048, error)
              && empiezaCon(error, "Liberacion de memoria no asignada"),
              "FREE de un rango con una pagina ya liberada da error");
    verificar(m.paginasDe("P1") == 3, "y la pagina 0 tampoco se libera (todo o nada)");
    verificar(!m.liberarMemoria("P9", 0, 1024, error) && empiezaCon(error, "Proceso inexistente"),
              "FREE a un proceso que no existe da 'Proceso inexistente'");
}

static void caso09LlenadoCacheLocal() {
    std::cout << "\n== 9. Llenado del cache local ==\n";
    MemoryManager m(MEMORIA, PAGINA, 8);
    std::string error;
    ResultadoAcceso r;
    m.nuevoProceso("P1", 10240, error);   // 10 paginas -> cache local de 2
    verificar(m.cacheLocalDe("P1")->capacidad() == 2, "cache local de P1 = floor(10*0.2) = 2");
    m.acceder("P1", 0, r, error);
    m.acceder("P1", 1024, r, error);
    verificar(m.cacheLocalDe("P1")->tamano() == 2, "despues de 2 paginas distintas, el cache local esta lleno");
    m.acceder("P1", 2048, r, error);
    verificar(m.cacheLocalDe("P1")->tamano() == 2, "con una tercera pagina sigue en 2 (no crece)");
    verificar(r.detalle.find("Cache local lleno") != std::string::npos, "se informa la expulsion del cache local");
    verificar(!cacheTiene(*m.cacheLocalDe("P1"), "P1", 0), "la pagina 0 (la menos reciente) fue expulsada");
}

static void caso10LlenadoCacheGlobal() {
    std::cout << "\n== 10. Llenado del cache global ==\n";
    MemoryManager m(MEMORIA, PAGINA, 2);     // cache global de 2
    std::string error;
    ResultadoAcceso r;
    m.nuevoProceso("P1", 4096, error);
    m.nuevoProceso("P2", 4096, error);
    m.acceder("P1", 0, r, error);
    m.acceder("P2", 0, r, error);
    verificar(m.getCacheGlobal().tamano() == 2, "con 2 accesos distintos el cache global esta lleno");
    m.acceder("P1", 1024, r, error);
    verificar(m.getCacheGlobal().tamano() == 2, "un tercer acceso no lo hace crecer");
    verificar(r.detalle.find("Cache global lleno") != std::string::npos, "se informa la expulsion del cache global");
}

static void caso11ReemplazoLRU() {
    std::cout << "\n== 11. Reemplazo por LRU ==\n";
    MemoryManager m(MEMORIA, PAGINA, 2);     // cache global de 2
    std::string error;
    ResultadoAcceso r;
    m.nuevoProceso("P1", 4096, error);       // cache local de 1
    m.acceder("P1", 0, r, error);            // global: [0]
    m.acceder("P1", 1024, r, error);         // global: [1 0]
    m.acceder("P1", 2048, r, error);         // lleno: sale 0 -> [2 1]
    verificar(!cacheTiene(m.getCacheGlobal(), "P1", 0), "al entrar la pagina 2, sale la 0 (la menos reciente)");
    verificar(cacheTiene(m.getCacheGlobal(), "P1", 1) && cacheTiene(m.getCacheGlobal(), "P1", 2),
              "quedan las paginas 1 y 2");

    m.acceder("P1", 1024, r, error);         // pagina 1: hit global -> [1 2]
    verificar(r.hitGlobal, "acceder a la pagina 1 es HIT en el cache global");
    m.acceder("P1", 3072, r, error);         // pagina 3: sale 2 (ahora la menos reciente) -> [3 1]
    verificar(!cacheTiene(m.getCacheGlobal(), "P1", 2), "al entrar la pagina 3, sale la 2 (la 1 se uso hace poco)");
    verificar(cacheTiene(m.getCacheGlobal(), "P1", 1), "la pagina 1 se queda porque se uso recientemente");
}

static void caso12MultiplesProcesos() {
    std::cout << "\n== 12. Multiples procesos activos ==\n";
    MemoryManager m(MEMORIA, PAGINA, 8);
    std::string error;
    ResultadoAcceso r;
    m.nuevoProceso("P1", 4096, error);       // fisicas 0-3
    m.nuevoProceso("P2", 8192, error);       // fisicas 4-11
    verificar(m.paginasLibres() == 52, "P1 (4) + P2 (8) ocupan 12 paginas: quedan 52");

    verificar(m.acceder("P1", 0, r, error) && r.paginaFisica == 0, "P1 pagina 0 esta en la fisica 0");
    verificar(m.acceder("P2", 0, r, error) && r.paginaFisica == 4, "P2 pagina 0 esta en la fisica 4");
    verificar(cacheTiene(m.getCacheGlobal(), "P1", 0) && cacheTiene(m.getCacheGlobal(), "P2", 0),
              "el cache global distingue P1:0 de P2:0");
    verificar(m.acceder("P2", 10, r, error) && r.hitLocal, "P2 vuelve a su pagina 0: HIT local");

    m.finalizarProceso("P1", error);
    verificar(m.existeProceso("P2") && m.paginasDe("P2") == 8, "terminar P1 no afecta a P2");
    verificar(!cacheTiene(m.getCacheGlobal(), "P1", 0) && cacheTiene(m.getCacheGlobal(), "P2", 0),
              "solo salen del cache global las entradas de P1");

    verificar(m.nuevoProceso("P3", 1024, error) && m.acceder("P3", 0, r, error) && r.paginaFisica == 0,
              "un proceso nuevo reutiliza la pagina fisica 0 que libero P1");
}

// ============================================================
// Extra: reemplazo de paginas (swap)
// ============================================================

// Con reemplazo, cada pagina de un proceso esta en memoria fisica o en swap.
// Revisa que las cuentas cuadren y que el swap no pase de su capacidad.
static bool cuentasCuadran(const MemoryManager& m, const std::vector<std::string>& ids) {
    int enFisica = 0;
    int enSwap = 0;
    for (std::size_t i = 0; i < ids.size(); i++) {
        if (!m.existeProceso(ids[i])) continue;
        enFisica += m.paginasDe(ids[i]) - m.paginasEnSwapDe(ids[i]);
        enSwap += m.paginasEnSwapDe(ids[i]);
    }
    return enFisica == m.paginasOcupadas() && enSwap == m.paginasEnSwap()
        && m.paginasEnSwap() <= m.getCapacidadSwap()
        && m.paginasOcupadas() + m.paginasLibres() == m.paginasTotales();
}

static void extraReemplazoConfiguracion() {
    std::cout << "\n== Extra: configurar el reemplazo de paginas ==\n";
    MemoryManager m(MEMORIA, PAGINA, 8);
    std::string error;
    verificar(!m.reemplazoActivo(), "por defecto el reemplazo esta apagado");
    verificar(!m.configurarSwap(-1, error) && !m.reemplazoActivo(), "un swap negativo se rechaza");
    verificar(m.configurarSwap(16, error) && m.reemplazoActivo() && m.getCapacidadSwap() == 16,
              "PAGINAS_SWAP 16 lo activa");
    m.nuevoProceso("P1", 1024, error);
    verificar(!m.configurarSwap(8, error) && m.getCapacidadSwap() == 16,
              "despues de la primera operacion ya no se puede cambiar");

    MemoryManager sinSwap(4096, PAGINA, 2);
    sinSwap.nuevoProceso("P1", 4096, error);
    verificar(!sinSwap.nuevoProceso("P2", 1024, error) && empiezaCon(error, "Memoria insuficiente")
              && sinSwap.paginasEnSwap() == 0,
              "sin swap, la memoria llena sigue dando Memoria insuficiente");
}

static void extraReemplazoVictimaYFallo() {
    std::cout << "\n== Extra: victima LRU, fallo de pagina y swap lleno ==\n";
    // 4 paginas fisicas, swap de 4, cache global de 2.
    MemoryManager m(4096, PAGINA, 2);
    std::string error;
    std::vector<std::string> ids;
    ids.push_back("P1"); ids.push_back("P2"); ids.push_back("P3"); ids.push_back("P5");
    m.configurarSwap(4, error);
    m.nuevoProceso("P1", 2048, error);    // fisicas 0 y 1
    m.nuevoProceso("P2", 2048, error);    // fisicas 2 y 3: memoria llena
    ResultadoAcceso r;
    m.acceder("P1", 0, r, error);         // P1:0 pasa a ser la mas reciente

    verificar(m.nuevoProceso("P3", 2048, error), "con swap, un proceso entra aunque la memoria este llena");
    verificar(m.estaEnSwap("P1", 1) && m.estaEnSwap("P2", 0) && !m.estaEnSwap("P1", 0),
              "LRU pasa a swap P1:1 y P2:0; P1:0 se salva porque se acababa de usar");
    verificar(m.paginasEnSwap() == 2 && m.getMetricas().reemplazos == 2 && m.paginasLibres() == 0,
              "2 paginas en swap, 2 reemplazos y 0 paginas fisicas libres");
    verificar(m.getEventos().size() == 2, "la operacion informa los 2 reemplazos");
    verificar(cuentasCuadran(m, ids), "cada pagina esta en memoria fisica o en swap");

    ResultadoAcceso fallo;
    verificar(m.acceder("P1", 1024, fallo, error) && fallo.falloPagina && fallo.paginaFisica == 3,
              "ACCESS a P1:1 es fallo de pagina y la carga en la pagina fisica 3");
    verificar(m.estaEnSwap("P2", 1) && !m.estaEnSwap("P1", 1),
              "para hacerle espacio, LRU pasa P2:1 a swap");
    verificar(m.getMetricas().fallosPagina == 1 && m.getMetricas().reemplazos == 3,
              "1 fallo de pagina y 3 reemplazos en total");
    ResultadoAcceso otra;
    verificar(m.acceder("P1", 1024, otra, error) && !otra.falloPagina && otra.hitLocal,
              "el siguiente acceso a P1:1 ya es HIT en cache local");

    verificar(m.nuevoProceso("P5", 1024, error) && m.estaEnSwap("P1", 0)
              && !cacheTiene(m.getCacheGlobal(), "P1", 0),
              "si la victima estaba en un cache, tambien sale del cache");
    verificar(!m.nuevoProceso("P6", 2048, error) && empiezaCon(error, "Memoria insuficiente")
              && error.find("libres en swap") != std::string::npos && m.paginasEnSwap() == 3,
              "si tampoco cabe en swap: Memoria insuficiente y nada cambia");
    verificar(!m.nuevoProceso("P7", 5000, error) && error.find("solo tiene 4") != std::string::npos,
              "no se puede pedir de una vez mas paginas que las fisicas");

    verificar(m.liberarMemoria("P1", 0, 1024, error) && m.paginasEnSwap() == 2 && m.paginasLibres() == 0,
              "FREE de una pagina en swap libera su lugar en swap");
    verificar(m.finalizarProceso("P2", error) && m.paginasEnSwap() == 0 && m.paginasLibres() == 0,
              "END_PROCESS de un proceso con todo en swap vacia el swap");
    verificar(cuentasCuadran(m, ids), "las cuentas siguen cuadrando");
    m.finalizarProceso("P1", error);
    m.finalizarProceso("P3", error);
    m.finalizarProceso("P5", error);
    verificar(m.paginasLibres() == 4 && m.paginasEnSwap() == 0,
              "al terminar todos, las 4 paginas fisicas quedan libres y el swap vacio");
}

static void extraReemplazoEstres() {
    std::cout << "\n== Extra: reemplazo con 1000 accesos ==\n";
    // 8 paginas fisicas y swap de 6; los procesos piden 13 paginas en total.
    MemoryManager m(8192, PAGINA, 3);
    std::string error;
    m.configurarSwap(6, error);
    std::vector<std::string> ids;
    ids.push_back("P1"); ids.push_back("P2"); ids.push_back("P3"); ids.push_back("P4");
    int paginas[4] = {3, 3, 3, 4};
    bool creados = true;
    for (int i = 0; i < 4; i++) {
        creados = creados && m.nuevoProceso(ids[i], paginas[i] * 1024LL, error);
    }
    verificar(creados && m.paginasEnSwap() == 5, "13 paginas en 8 fisicas: 5 quedan en swap");

    bool todoBien = true;
    unsigned int semilla = 12345;
    for (int k = 0; k < 1000; k++) {
        semilla = semilla * 1103515245u + 12345u;            // numeros "al azar" repetibles
        int p = static_cast<int>((semilla >> 16) % 4);
        int direccion = static_cast<int>((semilla >> 4) % (paginas[p] * 1024));
        ResultadoAcceso r;
        if (!m.acceder(ids[p], direccion, r, error) || r.paginaFisica < 0
            || !cuentasCuadran(m, ids)) {
            todoBien = false;
        }
    }
    verificar(todoBien, "los 1000 accesos funcionan y las cuentas siempre cuadran");
    verificar(m.getMetricas().fallosPagina > 0 && m.paginasEnSwap() == 5,
              "hubo fallos de pagina y el swap mantiene 5 paginas");
    for (int i = 0; i < 4; i++) m.finalizarProceso(ids[i], error);
    verificar(m.paginasLibres() == 8 && m.paginasEnSwap() == 0, "al final todo queda libre");
}

int main() {
    std::cout << "=== PRUEBAS DEL SIMULADOR DE MEMORIA ===\n";

    caso01CrearProcesoValido();
    caso02MemoriaInsuficiente();
    caso03FinalizarProceso();
    caso04AccesoExitoso();
    caso05DireccionInvalida();
    caso06AsignacionAdicional();
    caso07LiberacionParcial();
    caso08LiberacionInvalida();
    caso09LlenadoCacheLocal();
    caso10LlenadoCacheGlobal();
    caso11ReemplazoLRU();
    caso12MultiplesProcesos();
    extraReemplazoConfiguracion();
    extraReemplazoVictimaYFallo();
    extraReemplazoEstres();

    std::cout << "\nResultado: " << pasadas << " aprobadas, "
              << falladas << " falladas.\n";
    return (falladas == 0) ? 0 : 1;
}
