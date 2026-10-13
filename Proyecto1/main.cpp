// main.cpp - menú principal del sistema

#include <iostream>
#include <string>
#include "Utils.h"
#include "GestorEstudiantes.h"
#include "GestorCursos.h"
#include "GestorNavegacion.h"
#include "GestorSolicitudes.h"
#include "Menus.h"
#include "Persistencia.h"

#ifdef _WIN32
#include <windows.h>
#endif

static void guardarDatos(const GestorEstudiantes& estudiantes, const GestorCursos& cursos,
                         const GestorSolicitudes& solicitudes) {
    std::string error;
    if (Persistencia::guardar(estudiantes, cursos, solicitudes, error)) {
        std::cout << "Datos guardados: " << estudiantes.cantidad() << " estudiantes, "
                  << cursos.cantidadCursos() << " cursos, " << solicitudes.cantidad()
                  << " solicitudes pendientes.\n";
    } else {
        std::cout << "No se pudo guardar: " << error << "\n";
    }
}

static void cargarDatos(GestorEstudiantes& estudiantes, GestorCursos& cursos,
                        GestorSolicitudes& solicitudes) {
    if (!Persistencia::hayDatosGuardados()) {
        std::cout << "No hay datos guardados. Use primero la opción 7.\n";
        return;
    }
    if (!Utils::confirmar("Esto reemplaza los datos actuales. ¿Continuar?")) {
        std::cout << "Operación cancelada.\n";
        return;
    }
    std::string error;
    if (Persistencia::cargar(estudiantes, cursos, solicitudes, error)) {
        std::cout << "Datos cargados: " << estudiantes.cantidad() << " estudiantes, "
                  << cursos.cantidadCursos() << " cursos, " << solicitudes.cantidad()
                  << " solicitudes pendientes.\n";
    } else {
        std::cout << "No se pudo cargar (los datos actuales no se modificaron): " << error << "\n";
    }
}

int main() {
#ifdef _WIN32
    // UTF-8 en la consola para que se vean bien las tildes y la ñ
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    GestorEstudiantes gestorEstudiantes;
    GestorCursos gestorCursos;
    GestorNavegacion navegacion(gestorCursos);
    GestorSolicitudes solicitudes(gestorEstudiantes, gestorCursos);

    bool salir = false;
    while (!salir) {
        std::cout << "\n===== SISTEMA DE ADMINISTRACIÓN ACADÉMICA =====\n"
                  << "1. Manejo de estudiantes\n"
                  << "2. Manejo de cursos y contenidos\n"
                  << "3. Navegación académica\n"
                  << "4. Solicitudes pendientes\n"
                  << "5. Índice de búsqueda\n"
                  << "6. Reportes\n"
                  << "7. Guardar datos\n"
                  << "8. Cargar datos\n"
                  << "9. Salir\n";
        long long opcion = Utils::leerEntero("Opción: ", 1, 9);

        switch (opcion) {
            case 1: menuEstudiantes(gestorEstudiantes); break;
            case 2: menuCursos(gestorCursos); break;
            case 3: menuNavegacion(gestorEstudiantes, gestorCursos, navegacion); break;
            case 4: menuSolicitudes(solicitudes, gestorEstudiantes, gestorCursos); break;
            case 5: menuIndice(gestorEstudiantes); break;
            case 6: menuReportes(gestorEstudiantes, gestorCursos, solicitudes); break;
            case 7:
                guardarDatos(gestorEstudiantes, gestorCursos, solicitudes);
                Utils::pausar();
                break;
            case 8:
                cargarDatos(gestorEstudiantes, gestorCursos, solicitudes);
                Utils::pausar();
                break;
            case 9:
                if (Utils::confirmar("¿Desea salir del sistema?")) {
                    if (Utils::confirmar("¿Guardar los datos antes de salir?")) {
                        guardarDatos(gestorEstudiantes, gestorCursos, solicitudes);
                    }
                    salir = true;
                }
                break;
            default: break;
        }
    }

    std::cout << "\nHasta pronto.\n";
    return 0;
}
