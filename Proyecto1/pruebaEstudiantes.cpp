// pruebaEstudiantes.cpp - pruebas de GestorEstudiantes (tiene su propio main)
// Revisa que la lista y el ABB coincidan después de cada operación.

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "GestorEstudiantes.h"

static int pasadas = 0;
static int falladas = 0;

static void verificar(bool condicion, const std::string& descripcion) {
    if (condicion) {
        pasadas++;
        std::cout << "  [OK]    " << descripcion << "\n";
    } else {
        falladas++;
        std::cout << "  [FALLO] " << descripcion << "\n";
    }
}

static const long long BASE = 2025000000LL;   // para usar carnés cortos: BASE + 3

static std::string correo(const std::string& usuario) {
    return usuario + "@estudiantec.cr";
}

// Lista e índice coinciden: mismo tamaño, cada estudiante se encuentra
// por carné (mismo objeto) y el inorden sale ordenado.
static bool sincronizado(const GestorEstudiantes& g) {
    std::vector<const Estudiante*> ordenados = g.inorden();
    if (ordenados.size() != g.todos().size()) return false;
    for (const Estudiante& e : g.todos()) {
        if (g.buscarPorCarne(e.getCarne()) != &e) return false;
    }
    for (std::size_t i = 1; i < ordenados.size(); i++) {
        if (!(ordenados[i - 1]->getCarne() < ordenados[i]->getCarne())) return false;
    }
    return true;
}

// Convierte un recorrido a texto con los últimos dígitos del carné: "1 2 3"
static std::string carnes(const std::vector<const Estudiante*>& v) {
    std::ostringstream salida;
    for (std::size_t i = 0; i < v.size(); i++) {
        if (i > 0) salida << " ";
        salida << (v[i]->getCarne() - BASE);
    }
    return salida.str();
}

// Cinco estudiantes registrados en este orden de carné: 3, 1, 5, 2, 4
static void llenar(GestorEstudiantes& g) {
    std::string error;
    g.registrar(BASE + 3, "Ana Perez", correo("ana.perez"), error);
    g.registrar(BASE + 1, "Luis Mora", correo("luis.mora"), error);
    g.registrar(BASE + 5, "Mariana Soto", correo("mariana.soto"), error);
    g.registrar(BASE + 2, "Pedro Rojas", correo("pedro.rojas"), error);
    g.registrar(BASE + 4, "Marta Vega", correo("marta.vega"), error);
}

static void pruebaVacio() {
    std::cout << "\n== Gestor vacio ==\n";
    GestorEstudiantes g;
    verificar(g.vacio() && g.cantidad() == 0, "un gestor nuevo esta vacio");
    verificar(g.buscarPorCarne(1) == nullptr, "buscarPorCarne en vacio -> nullptr");
    verificar(g.buscarPorCorreo(correo("x")) == nullptr, "buscarPorCorreo en vacio -> nullptr");
    verificar(g.buscarPorNombre("a").empty(), "buscarPorNombre en vacio -> sin resultados");
    verificar(g.menorCarne() == nullptr && g.mayorCarne() == nullptr, "menor y mayor en vacio -> nullptr");
    verificar(g.inorden().empty(), "inorden vacio");
    verificar(!g.eliminar(1), "eliminar en vacio -> false");
    verificar(g.alturaIndice() == 0, "altura del indice = 0");
    verificar(sincronizado(g), "lista e indice sincronizados");
}

static void pruebaRegistrar() {
    std::cout << "\n== Registrar ==\n";
    GestorEstudiantes g;
    llenar(g);
    verificar(g.cantidad() == 5, "cantidad = 5");
    verificar(g.todos().front().getNombre() == "Ana Perez", "la LISTA conserva el orden de registro (primero Ana)");
    verificar(g.todos().back().getNombre() == "Marta Vega", "la LISTA termina con el ultimo registrado (Marta)");
    verificar(carnes(g.inorden()) == "1 2 3 4 5", "inorden del ABB sale ordenado por carne: 1 2 3 4 5");
    verificar(carnes(g.preorden()) == "3 1 2 5 4", "preorden del ABB: 3 1 2 5 4");
    verificar(carnes(g.postorden()) == "2 1 4 5 3", "postorden del ABB: 2 1 4 5 3");
    verificar(g.menorCarne()->getNombre() == "Luis Mora", "menor carne es Luis (1)");
    verificar(g.mayorCarne()->getNombre() == "Mariana Soto", "mayor carne es Mariana (5)");
    verificar(g.alturaIndice() == 3, "altura del indice = 3");
    verificar(sincronizado(g), "lista e indice sincronizados");
}

static void pruebaRechazos() {
    std::cout << "\n== Registros rechazados (no deben cambiar nada) ==\n";
    GestorEstudiantes g;
    llenar(g);
    std::string error;

    error = "";
    verificar(!g.registrar(BASE + 3, "Otro", correo("otro"), error) && !error.empty(),
              "carne repetido -> rechazado con mensaje");
    error = "";
    verificar(!g.registrar(BASE + 9, "Otro", correo("ana.perez"), error) && !error.empty(),
              "correo repetido -> rechazado con mensaje");
    error = "";
    verificar(!g.registrar(BASE + 9, "Otro", "ANA.PEREZ@estudiantec.cr", error) && !error.empty(),
              "correo repetido con MAYUSCULAS -> rechazado");
    error = "";
    verificar(!g.registrar(BASE + 9, "Otro", "ana@gmail.com", error) && !error.empty(),
              "correo con otro dominio -> rechazado");
    error = "";
    verificar(!g.registrar(BASE + 9, "   ", correo("nuevo"), error) && !error.empty(),
              "nombre vacio (solo espacios) -> rechazado");
    error = "";
    verificar(!g.registrar(0, "Otro", correo("nuevo"), error) && !error.empty(),
              "carne 0 -> rechazado");
    error = "";
    verificar(!g.registrar(-5, "Otro", correo("nuevo"), error) && !error.empty(),
              "carne negativo -> rechazado");
    verificar(g.cantidad() == 5, "despues de los rechazos sigue habiendo 5");
    verificar(sincronizado(g), "lista e indice sincronizados");
}

static void pruebaBuscar() {
    std::cout << "\n== Buscar ==\n";
    GestorEstudiantes g;
    llenar(g);

    const Estudiante* e = g.buscarPorCarne(BASE + 4);
    verificar(e != nullptr && e->getNombre() == "Marta Vega", "buscarPorCarne(4) -> Marta Vega");
    verificar(g.buscarPorCarne(BASE + 99) == nullptr, "buscarPorCarne inexistente -> nullptr");

    verificar(g.buscarPorNombre("ana").size() == 2, "nombre 'ana' encuentra 2 (Ana y Mariana)");
    verificar(g.buscarPorNombre("MARTA").size() == 1, "nombre 'MARTA' (mayusculas) encuentra 1");
    verificar(g.buscarPorNombre("zzz").empty(), "nombre 'zzz' -> sin resultados");
    verificar(g.buscarPorNombre("").empty(), "nombre vacio -> sin resultados");
    verificar(g.buscarPorNombre("   ").empty(), "nombre de solo espacios -> sin resultados");

    const Estudiante* c = g.buscarPorCorreo("LUIS.MORA@estudiantec.cr");
    verificar(c != nullptr && c->getCarne() == BASE + 1, "buscarPorCorreo ignora mayusculas");
    verificar(g.buscarPorCorreo(correo("nadie")) == nullptr, "correo inexistente -> nullptr");
}

static void pruebaModificar() {
    std::cout << "\n== Modificar ==\n";
    GestorEstudiantes g;
    llenar(g);
    std::string error;

    // Nombre
    verificar(g.cambiarNombre(BASE + 3, "  Ana Maria Perez  ", error), "cambiarNombre ok");
    verificar(g.buscarPorCarne(BASE + 3)->getNombre() == "Ana Maria Perez", "el nombre cambio (sin espacios sobrantes)");
    verificar(!g.cambiarNombre(BASE + 3, "   ", error), "nombre vacio -> rechazado");
    verificar(!g.cambiarNombre(BASE + 99, "X", error), "cambiarNombre de carne inexistente -> false");

    // Correo
    verificar(g.cambiarCorreo(BASE + 3, correo("ana.maria"), error), "cambiarCorreo ok");
    verificar(g.buscarPorCorreo(correo("ana.maria")) != nullptr, "se encuentra por el correo nuevo");
    verificar(g.buscarPorCorreo(correo("ana.perez")) == nullptr, "el correo viejo ya no existe");
    verificar(!g.cambiarCorreo(BASE + 3, correo("luis.mora"), error), "correo de OTRO estudiante -> rechazado");
    verificar(g.cambiarCorreo(BASE + 3, correo("ana.maria"), error), "poner su PROPIO correo -> permitido");
    verificar(!g.cambiarCorreo(BASE + 3, "x@gmail.com", error), "correo invalido -> rechazado");

    // Carné (obliga a reindexar el ABB)
    verificar(g.cambiarCarne(BASE + 3, BASE + 10, error), "cambiarCarne 3 -> 10 ok");
    verificar(g.buscarPorCarne(BASE + 3) == nullptr, "el carne viejo ya no se encuentra en el ABB");
    verificar(g.buscarPorCarne(BASE + 10) != nullptr &&
              g.buscarPorCarne(BASE + 10)->getNombre() == "Ana Maria Perez",
              "el carne nuevo se encuentra y es la misma persona");
    verificar(g.mayorCarne()->getCarne() == BASE + 10, "el mayor carne ahora es 10");
    verificar(carnes(g.inorden()) == "1 2 4 5 10", "inorden reordenado: 1 2 4 5 10");
    verificar(!g.cambiarCarne(BASE + 10, BASE + 1, error), "carne nuevo ya ocupado -> rechazado");
    verificar(g.cambiarCarne(BASE + 10, BASE + 10, error), "carne igual al actual -> true sin cambios");
    verificar(!g.cambiarCarne(BASE + 99, BASE + 7, error), "carne actual inexistente -> false");
    verificar(!g.cambiarCarne(BASE + 10, 0, error), "carne nuevo 0 -> rechazado");
    verificar(g.cantidad() == 5, "sigue habiendo 5 estudiantes");
    verificar(sincronizado(g), "lista e indice sincronizados");
}

static void pruebaEliminar() {
    std::cout << "\n== Eliminar ==\n";
    GestorEstudiantes g;
    llenar(g);

    verificar(g.eliminar(BASE + 3), "eliminar Ana (3, la raiz del ABB) -> true");
    verificar(g.buscarPorCarne(BASE + 3) == nullptr, "Ana ya no esta en el ABB");
    verificar(g.buscarPorNombre("Ana Perez").empty(), "Ana ya no esta en la lista");
    verificar(g.cantidad() == 4, "cantidad = 4");
    verificar(sincronizado(g), "lista e indice sincronizados");
    verificar(!g.eliminar(BASE + 3), "eliminar de nuevo -> false");

    verificar(g.eliminar(BASE + 1) && g.eliminar(BASE + 5), "eliminar el menor (1) y el mayor (5)");
    verificar(g.menorCarne()->getCarne() == BASE + 2, "nuevo menor = 2");
    verificar(g.mayorCarne()->getCarne() == BASE + 4, "nuevo mayor = 4");
    verificar(carnes(g.inorden()) == "2 4", "quedan 2 y 4");
    verificar(sincronizado(g), "lista e indice sincronizados");

    verificar(g.eliminar(BASE + 2) && g.eliminar(BASE + 4), "eliminar los dos restantes");
    verificar(g.vacio() && g.alturaIndice() == 0, "gestor vacio otra vez");

    std::string error;
    verificar(g.registrar(BASE + 3, "Ana Nueva", correo("ana.perez"), error),
              "el carne y el correo eliminados se pueden volver a usar");
    verificar(sincronizado(g), "lista e indice sincronizados");
}

static void pruebaLimpiar() {
    std::cout << "\n== limpiar() ==\n";
    GestorEstudiantes g;
    llenar(g);
    g.limpiar();
    verificar(g.vacio() && g.buscarPorCarne(BASE + 1) == nullptr, "limpiar() vacia lista e indice");
    llenar(g);
    verificar(g.cantidad() == 5 && sincronizado(g), "se puede volver a llenar");
}

static void pruebaMuchos() {
    std::cout << "\n== Muchos estudiantes ==\n";
    GestorEstudiantes g;
    std::string error;
    unsigned int semilla = 777;
    int registrados = 0;
    for (int i = 0; i < 400; i++) {
        semilla = semilla * 1103515245u + 12345u;
        long long carne = 1 + static_cast<long long>((semilla / 65536u) % 1000u);
        std::ostringstream nombre, mail;
        nombre << "Estudiante " << carne;
        mail << "est" << carne << "@estudiantec.cr";
        if (g.registrar(carne, nombre.str(), mail.str(), error)) registrados++;
    }
    verificar(g.cantidad() == registrados, "cantidad coincide con los registros aceptados");
    verificar(sincronizado(g), "lista e indice sincronizados tras registrar");

    for (long long c = 2; c <= 1000; c += 2) g.eliminar(c);   // borrar los pares
    bool hayPares = false;
    for (const Estudiante& e : g.todos()) {
        if (e.getCarne() % 2 == 0) hayPares = true;
    }
    verificar(!hayPares, "tras borrar los pares no queda ninguno en la lista");
    verificar(sincronizado(g), "lista e indice sincronizados tras eliminar");

    bool todoBien = true;
    std::vector<long long> restantes;
    for (const Estudiante& e : g.todos()) restantes.push_back(e.getCarne());
    for (std::size_t i = 0; i < restantes.size(); i++) {
        if (!g.cambiarCarne(restantes[i], restantes[i] + 5000, error)) todoBien = false;
    }
    verificar(todoBien, "reindexar todos los carnes restantes (+5000) sin errores");
    verificar(sincronizado(g), "lista e indice sincronizados tras reindexar");
    verificar(g.menorCarne() == nullptr || g.menorCarne()->getCarne() > 5000, "todos los carnes quedaron > 5000");
}

int main() {
    std::cout << "=== PRUEBAS DE GestorEstudiantes ===\n";
    pruebaVacio();
    pruebaRegistrar();
    pruebaRechazos();
    pruebaBuscar();
    pruebaModificar();
    pruebaEliminar();
    pruebaLimpiar();
    pruebaMuchos();

    std::cout << "\n=============================\n";
    std::cout << "Pasaron: " << pasadas << "   Fallaron: " << falladas << "\n";
    if (falladas == 0) {
        std::cout << "TODO CORRECTO\n";
    } else {
        std::cout << "HAY FALLOS: revise las lineas [FALLO]\n";
    }
    return (falladas == 0) ? 0 : 1;
}
