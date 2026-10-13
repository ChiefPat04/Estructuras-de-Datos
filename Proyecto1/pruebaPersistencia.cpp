// pruebaPersistencia.cpp - pruebas de guardar y cargar (tiene su propio main)
// Crea archivos datos_*.txt en la carpeta actual y los borra al terminar.

#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "GestorCursos.h"
#include "GestorEstudiantes.h"
#include "GestorNavegacion.h"
#include "GestorSolicitudes.h"
#include "Persistencia.h"
#include "Reportes.h"
#include "Utils.h"

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

static void escribir(const std::string& archivo, const std::string& contenido) {
    std::ofstream salida(archivo.c_str());
    salida << contenido;
}

static void borrarArchivos() {
    std::remove("datos_estudiantes.txt");
    std::remove("datos_cursos.txt");
    std::remove("datos_historiales.txt");
    std::remove("datos_solicitudes.txt");
}

static std::string reportes(const GestorEstudiantes& e, const GestorCursos& c, const GestorSolicitudes& s) {
    std::ostringstream salida;
    Reportes::todos(salida, e, c, s);
    return salida.str();
}

static void pruebaUtils() {
    std::cout << "\n== Utils.dividir y Utils.aEntero ==\n";
    std::vector<std::string> p = Utils::dividir("a|b||c", '|');
    verificar(p.size() == 4 && p[0] == "a" && p[2] == "" && p[3] == "c", "dividir conserva los campos vacíos");
    verificar(Utils::dividir("", '|').size() == 1, "dividir un texto vacío da un campo");
    long long v = 0;
    verificar(Utils::aEntero("123", v) && v == 123, "aEntero acepta un número");
    verificar(Utils::aEntero(" 45 ", v) && v == 45, "aEntero ignora espacios");
    verificar(!Utils::aEntero("12a", v) && !Utils::aEntero("", v) && !Utils::aEntero("1 2", v), "aEntero rechaza texto inválido");
}

int main() {
    borrarArchivos();
    pruebaUtils();

    GestorEstudiantes est;
    GestorCursos cur;
    GestorNavegacion nav(cur);
    GestorSolicitudes sol(est, cur);
    std::string error;

    std::cout << "\n== Cargar sin archivos ==\n";
    verificar(!Persistencia::hayDatosGuardados(), "no hay datos guardados al inicio");
    verificar(!Persistencia::cargar(est, cur, sol, error) && error.find("No se encontró") != std::string::npos,
              "cargar sin archivos falla con un mensaje");

    // Sistema de prueba
    est.registrar(50, "Eva | Soto", "eva@estudiantec.cr", error);       // lleva un '|' en el nombre
    est.registrar(30, "Ana", "ana@estudiantec.cr", error);
    est.registrar(70, "Luis", "luis@estudiantec.cr", error);
    cur.crearCurso("Datos", error);                              // 1
    cur.agregarHijo(1, TipoNodo::Unidad, "U1", error);           // 2
    cur.agregarHijo(2, TipoNodo::Tema, "T1", error);             // 3
    cur.agregarHijo(3, TipoNodo::Recurso, "PDF", error);         // 4
    cur.agregarHijo(3, TipoNodo::Actividad, "Quiz", error);      // 5
    cur.crearCurso("Redes", error);                              // 6
    cur.agregarHijo(6, TipoNodo::Unidad, "R1", error);           // 7

    Estudiante* ana = est.buscarPorCarne(30);
    nav.visitar(*ana, 3, error);
    nav.visitar(*ana, 4, error);
    nav.visitar(*ana, 5, error);
    nav.marcarCompletado(*ana, 4, error);
    nav.inscribirse(*ana, 5, error);
    nav.visitar(*est.buscarPorCarne(50), 3, error);
    sol.insertar(30, 4, TipoSolicitud::ReporteError, "roto", error);
    sol.insertar(70, 5, TipoSolicitud::RevisionTarea, "duda", error);
    sol.insertar(50, 3, TipoSolicitud::HabilitacionRecurso, "habilitar", error);
    Solicitud atendida;
    sol.setModoPrioridad(false);
    sol.atenderSiguiente(atendida);                              // sale la #1
    sol.setModoPrioridad(true);

    std::cout << "\n== Guardar ==\n";
    verificar(Persistencia::guardar(est, cur, sol, error), "guardar funciona");
    verificar(Persistencia::hayDatosGuardados(), "quedan los cuatro archivos");

    std::cout << "\n== Cargar en un sistema nuevo ==\n";
    GestorEstudiantes est2;
    GestorCursos cur2;
    GestorNavegacion nav2(cur2);
    GestorSolicitudes sol2(est2, cur2);
    verificar(Persistencia::cargar(est2, cur2, sol2, error), "cargar funciona");
    verificar(est2.cantidad() == 3 && cur2.cantidadCursos() == 2 && cur2.totalNodos() == 7, "cantidades iguales");
    verificar(est2.buscarPorCarne(50)->getNombre() == "Eva / Soto", "el '|' de un nombre se reemplaza por '/'");
    est.buscarPorCarne(50)->setNombre("Eva / Soto");   // para poder comparar con lo cargado
    verificar(reportes(est, cur, sol) == reportes(est2, cur2, sol2), "los 10 reportes salen idénticos");
    verificar(cur2.buscarPorId(3)->getVisitas() == 2, "las visitas de los nodos se conservan");
    verificar(sol2.getModoPrioridad() && sol2.atendidas() == 1 && sol2.cantidad() == 2, "estado de las colas");
    verificar(sol2.getProximoNumero() == 4, "la numeración sigue donde iba");

    std::cout << "\n== El historial funciona igual después de cargar ==\n";
    Estudiante* ana2 = est2.buscarPorCarne(30);
    verificar(ana2->getHistorial().paginaActual() == 5, "la página actual es la misma");
    verificar(ana2->getHistorial().estaCompletado(4) && ana2->getHistorial().estaInscrito(5), "completados e inscripciones");
    std::string mensaje;
    verificar(nav2.deshacer(*ana2, mensaje) && !ana2->getHistorial().estaInscrito(5), "deshacer revierte la última acción (inscripción)");
    verificar(nav2.deshacer(*ana2, mensaje) && !ana2->getHistorial().estaCompletado(4), "y luego el completado");
    verificar(nav2.deshacer(*ana2, mensaje) && ana2->getHistorial().paginaActual() == 4, "y luego la última visita");
    verificar(nav2.retroceder(*ana2, error) && ana2->getHistorial().paginaActual() == 3, "retroceder funciona");

    Solicitud s;
    sol2.atenderSiguiente(s);
    verificar(s.numero == 2 || s.numero == 3, "se puede atender una solicitud cargada");
    verificar(sol2.insertar(30, 4, TipoSolicitud::ConsultaAcademica, "x", error) && sol2.cantidad() == 2,
              "se pueden agregar solicitudes nuevas");

    std::cout << "\n== Archivos dañados no modifican los datos actuales ==\n";
    std::string antes = reportes(est, cur, sol);

    escribir("datos_cursos.txt", "1|0|Curso|Datos|0\n2|99|Unidad|Huérfana|0\n");
    verificar(!Persistencia::cargar(est, cur, sol, error) && error.find("datos_cursos.txt, línea 2") != std::string::npos,
              "un nodo con padre inexistente se rechaza indicando la línea");
    verificar(reportes(est, cur, sol) == antes, "los datos actuales siguen intactos");

    Persistencia::guardar(est, cur, sol, error);
    escribir("datos_estudiantes.txt", "50|Eva|eva@estudiantec.cr\nabc|Mal|mal@estudiantec.cr\n");
    verificar(!Persistencia::cargar(est, cur, sol, error) && error.find("datos_estudiantes.txt, línea 2") != std::string::npos,
              "un carné que no es número se rechaza");
    verificar(reportes(est, cur, sol) == antes, "los datos actuales siguen intactos (estudiantes)");

    Persistencia::guardar(est, cur, sol, error);
    escribir("datos_estudiantes.txt", "50|Eva|eva@estudiantec.cr\n50|Otra|otra@estudiantec.cr\n");
    verificar(!Persistencia::cargar(est, cur, sol, error), "un carné repetido se rechaza");

    Persistencia::guardar(est, cur, sol, error);
    escribir("datos_historiales.txt", "P|999|3\n");
    verificar(!Persistencia::cargar(est, cur, sol, error), "un historial de un estudiante inexistente se rechaza");

    Persistencia::guardar(est, cur, sol, error);
    escribir("datos_solicitudes.txt", "1|30|4|0|sin config\n");
    verificar(!Persistencia::cargar(est, cur, sol, error), "solicitudes sin línea de configuración se rechazan");

    Persistencia::guardar(est, cur, sol, error);
    escribir("datos_solicitudes.txt", "config|1|3|0\n1|30|4|9|tipo malo\n");
    verificar(!Persistencia::cargar(est, cur, sol, error), "un tipo de solicitud inválido se rechaza");
    verificar(reportes(est, cur, sol) == antes, "los datos actuales siguen intactos al final");

    std::cout << "\n== Archivos con formato de Windows (CRLF) ==\n";
    Persistencia::guardar(est, cur, sol, error);
    escribir("datos_estudiantes.txt", "50|Eva|eva@estudiantec.cr\r\n30|Ana|ana@estudiantec.cr\r\n");
    GestorEstudiantes est3;
    GestorCursos cur3;
    GestorSolicitudes sol3(est3, cur3);
    verificar(Persistencia::cargar(est3, cur3, sol3, error) && est3.cantidad() == 2, "se ignoran los '\\r' de fin de línea");

    std::cout << "\n== Sistema vacío ==\n";
    GestorEstudiantes vE;
    GestorCursos vC;
    GestorSolicitudes vS(vE, vC);
    verificar(Persistencia::guardar(vE, vC, vS, error), "guardar un sistema vacío");
    verificar(Persistencia::cargar(est3, cur3, sol3, error) && est3.vacio() && cur3.vacio() && sol3.cantidad() == 0,
              "cargarlo deja todo vacío");

    borrarArchivos();
    std::cout << "\n=============================\n"
              << "Pasaron: " << pasadas << "   Fallaron: " << falladas << "\n"
              << (falladas == 0 ? "TODO CORRECTO" : "HAY FALLOS") << "\n"
              << "=============================\n";
    return falladas == 0 ? 0 : 1;
}
