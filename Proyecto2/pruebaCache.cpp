// pruebaCache.cpp - pruebas del cache LRU (tiene su propio main, se compila aparte)

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "Cache.h"

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

// Crea una entrada de prueba. El numero de marco fisico es inventado.
static EntradaCache entrada(const std::string& proceso, int pagina) {
    EntradaCache e;
    e.idProceso = proceso;
    e.paginaVirtual = pagina;
    e.paginaFisica = pagina * 10;
    e.ultimaReferencia = 0;
    return e;
}

// Convierte el contenido del cache a texto: "P1:2 P1:1" (mas reciente primero).
static std::string aTexto(const Cache& c) {
    std::ostringstream salida;
    std::vector<EntradaCache> datos = c.contenido();
    for (std::size_t i = 0; i < datos.size(); i++) {
        if (i > 0) salida << " ";
        salida << datos[i].idProceso << ":" << datos[i].paginaVirtual;
    }
    return salida.str();
}

static void pruebaExpulsaLRU() {
    std::cout << "\n== Cache de 2: se expulsa la menos reciente ==\n";
    Cache c(2);
    EntradaCache expulsada;
    verificar(!c.insertar(entrada("P1", 1), &expulsada), "insertar P1:1 no expulsa nada");
    verificar(!c.insertar(entrada("P1", 2), &expulsada), "insertar P1:2 no expulsa nada");
    verificar(aTexto(c) == "P1:2 P1:1", "orden: el mas reciente es P1:2");

    // Usar P1:1 lo vuelve el mas reciente. Asi la menos reciente pasa a ser P1:2.
    verificar(c.buscar("P1", 1) != nullptr, "buscar P1:1 es hit");
    verificar(aTexto(c) == "P1:1 P1:2", "despues de usar P1:1, es el mas reciente");

    verificar(c.insertar(entrada("P1", 3), &expulsada), "insertar P1:3 con cache lleno expulsa");
    verificar(expulsada.paginaVirtual == 2, "se expulsa P1:2 (la menos reciente)");
    verificar(aTexto(c) == "P1:3 P1:1", "quedan P1:3 y P1:1");
    verificar(c.tamano() == 2, "tamano sigue en 2");
}

static void pruebaBuscarNoExiste() {
    std::cout << "\n== Buscar algo que no esta ==\n";
    Cache c(3);
    c.insertar(entrada("P1", 1), nullptr);
    verificar(c.buscar("P1", 9) == nullptr, "buscar P1:9 devuelve nullptr");
    verificar(c.buscar("P2", 1) == nullptr, "buscar P2:1 devuelve nullptr (otro proceso)");
    verificar(c.tamano() == 1, "buscar no cambia el tamano");
}

static void pruebaActualizarDuplicado() {
    std::cout << "\n== Insertar la misma clave dos veces ==\n";
    Cache c(3);
    c.insertar(entrada("P1", 1), nullptr);
    EntradaCache otra = entrada("P1", 1);
    otra.paginaFisica = 999;
    verificar(!c.insertar(otra, nullptr), "insertar de nuevo no expulsa");
    verificar(c.tamano() == 1, "no se duplica: tamano sigue en 1");
    verificar(c.buscar("P1", 1)->paginaFisica == 999, "la entrada se actualizo a 999");
}

static void pruebaEliminar() {
    std::cout << "\n== Eliminar una entrada ==\n";
    Cache c(3);
    c.insertar(entrada("P1", 1), nullptr);
    c.insertar(entrada("P1", 2), nullptr);
    verificar(c.eliminar("P1", 1), "eliminar P1:1 devuelve true");
    verificar(!c.eliminar("P1", 1), "eliminar P1:1 otra vez devuelve false");
    verificar(aTexto(c) == "P1:2", "solo queda P1:2");
}

static void pruebaEliminarProceso() {
    std::cout << "\n== Eliminar todas las entradas de un proceso ==\n";
    Cache c(5);
    c.insertar(entrada("P1", 1), nullptr);
    c.insertar(entrada("P2", 1), nullptr);
    c.insertar(entrada("P1", 2), nullptr);
    verificar(c.eliminarProceso("P1") == 2, "eliminarProceso P1 quita 2 entradas");
    verificar(aTexto(c) == "P2:1", "queda solo P2:1");
    verificar(c.eliminarProceso("P9") == 0, "eliminarProceso de un proceso sin entradas quita 0");
}

static void pruebaRedimensionar() {
    std::cout << "\n== Reducir el tamano del cache ==\n";
    Cache c(3);
    c.insertar(entrada("P1", 1), nullptr);
    c.insertar(entrada("P1", 2), nullptr);
    c.insertar(entrada("P1", 3), nullptr);
    verificar(c.redimensionar(1) == 2, "reducir a 1 expulsa 2 entradas");
    verificar(c.tamano() == 1 && c.capacidad() == 1, "tamano y capacidad son 1");
    verificar(aTexto(c) == "P1:3", "queda la mas reciente: P1:3");
    verificar(c.redimensionar(4) == 0, "agrandar no expulsa nada");
    verificar(c.capacidad() == 4, "la capacidad ahora es 4");
}

static void pruebaCapacidadCero() {
    std::cout << "\n== Cache de tamano 0 ==\n";
    Cache c(0);
    verificar(!c.insertar(entrada("P1", 1), nullptr), "insertar en cache de 0 devuelve false");
    verificar(c.tamano() == 0, "el cache de 0 queda vacio");
}

// menosReciente (la usa el reemplazo de paginas para elegir la victima).
static void pruebaMenosReciente() {
    std::cout << "\n== Menos reciente (victima LRU) ==\n";
    Cache c(3);
    EntradaCache victima;
    verificar(!c.menosReciente(victima), "un cache vacio no tiene victima");
    c.insertar(entrada("A", 0), nullptr);
    c.insertar(entrada("A", 1), nullptr);
    c.insertar(entrada("A", 2), nullptr);
    verificar(c.menosReciente(victima) && victima.paginaVirtual == 0,
              "la victima es la entrada usada hace mas tiempo (A:0)");
    verificar(c.tamano() == 3, "consultar la victima no la saca del cache");
    c.buscar("A", 0);
    verificar(c.menosReciente(victima) && victima.paginaVirtual == 1,
              "al usar A:0, la victima pasa a ser A:1");
}

int main() {
    std::cout << "=== PRUEBAS DEL CACHE LRU ===\n";

    pruebaExpulsaLRU();
    pruebaBuscarNoExiste();
    pruebaActualizarDuplicado();
    pruebaEliminar();
    pruebaEliminarProceso();
    pruebaRedimensionar();
    pruebaCapacidadCero();
    pruebaMenosReciente();

    std::cout << "\nResultado: " << pasadas << " aprobadas, "
              << falladas << " falladas.\n";
    return (falladas == 0) ? 0 : 1;
}
