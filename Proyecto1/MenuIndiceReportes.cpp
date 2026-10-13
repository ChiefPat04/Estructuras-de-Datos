// MenuIndiceReportes.cpp - índice de búsqueda (ABB) y reportes

#include "Menus.h"

#include <fstream>
#include <iostream>
#include <vector>
#include "Utils.h"

static const long long CARNE_MAXIMO = 9999999999LL;

// ----- Índice de búsqueda -----

static void buscarEnIndice(const GestorEstudiantes& estudiantes) {
    std::cout << "\n--- Buscar por carné en el ABB ---\n";
    long long carne = Utils::leerEntero("Carné: ", 1, CARNE_MAXIMO);
    const Estudiante* e = estudiantes.buscarPorCarne(carne);
    if (e == nullptr) {
        std::cout << "No existe ningún estudiante con ese carné.\n";
    } else {
        e->mostrar();
    }
}

static void imprimirRecorrido(const std::string& titulo, const std::vector<const Estudiante*>& orden) {
    std::cout << "\n--- " << titulo << " ---\n";
    if (orden.empty()) {
        std::cout << "El índice está vacío.\n";
        return;
    }
    for (std::size_t i = 0; i < orden.size(); i++) {
        std::cout << (i + 1) << ". " << orden[i]->getCarne() << " - " << orden[i]->getNombre() << "\n";
    }
}

static void menorYMayor(const GestorEstudiantes& estudiantes) {
    std::cout << "\n";
    Reportes::menorYMayorCarne(std::cout, estudiantes);
}

static void alturaYCantidad(const GestorEstudiantes& estudiantes) {
    std::cout << "\n--- Datos del ABB ---\n"
              << "Nodos (estudiantes indexados): " << estudiantes.cantidad() << "\n"
              << "Altura: " << estudiantes.alturaIndice() << " niveles\n";
}

void menuIndice(const GestorEstudiantes& estudiantes) {
    while (true) {
        std::cout << "\n===== ÍNDICE DE BÚSQUEDA (ABB por carné) =====\n"
                  << "1. Buscar estudiante por carné\n"
                  << "2. Recorrido inorden\n"
                  << "3. Recorrido preorden\n"
                  << "4. Recorrido postorden\n"
                  << "5. Menor y mayor carné\n"
                  << "6. Altura y cantidad de nodos\n"
                  << "7. Mostrar la forma del árbol\n"
                  << "0. Volver al menú principal\n"
                  << "(Insertar y eliminar se hacen al registrar y eliminar estudiantes.)\n";
        long long opcion = Utils::leerEntero("Opción: ", 0, 7);

        switch (opcion) {
            case 1: buscarEnIndice(estudiantes); break;
            case 2: imprimirRecorrido("Inorden (ordenado por carné)", estudiantes.inorden()); break;
            case 3: imprimirRecorrido("Preorden", estudiantes.preorden()); break;
            case 4: imprimirRecorrido("Postorden", estudiantes.postorden()); break;
            case 5: menorYMayor(estudiantes); break;
            case 6: alturaYCantidad(estudiantes); break;
            case 7:
                std::cout << "\n--- Forma del árbol (raíz a la izquierda, derecha arriba) ---\n";
                estudiantes.mostrarIndice(std::cout);
                break;
            default: return;
        }
        Utils::pausar();
    }
}

// ----- Reportes -----

static void reporteEstructuraCurso(GestorCursos& cursos) {
    ArbolGenerico* curso = elegirCurso(cursos);
    if (curso == nullptr) return;
    std::cout << "\n";
    Reportes::estructuraCurso(std::cout, *curso, cursos.posicionCurso(curso));
}

static void reporteHistorial(GestorEstudiantes& estudiantes, const GestorCursos& cursos) {
    Estudiante* e = pedirEstudiante(estudiantes);
    if (e == nullptr) return;
    std::cout << "\n";
    Reportes::historialEstudiante(std::cout, *e, cursos);
}

static void reporteRecursosHoja(GestorCursos& cursos) {
    ArbolGenerico* curso = elegirCurso(cursos);
    if (curso == nullptr) return;
    std::cout << "\n";
    Reportes::recursosHoja(std::cout, *curso);
}

static void exportarReportes(const GestorEstudiantes& estudiantes, const GestorCursos& cursos,
                             const GestorSolicitudes& solicitudes) {
    const std::string nombreArchivo = "reportes.txt";
    std::ofstream archivo(nombreArchivo.c_str());
    if (!archivo) {
        std::cout << "No se pudo crear el archivo " << nombreArchivo << ".\n";
        return;
    }
    Reportes::todos(archivo, estudiantes, cursos, solicitudes);
    std::cout << "Reportes guardados en " << nombreArchivo << ".\n";
}

void menuReportes(GestorEstudiantes& estudiantes, GestorCursos& cursos,
                  const GestorSolicitudes& solicitudes) {
    while (true) {
        std::cout << "\n===== REPORTES =====\n"
                  << "1. Lista completa de estudiantes\n"
                  << "2. Recorrido del ABB en inorden\n"
                  << "3. Estructura jerárquica de un curso\n"
                  << "4. Historial de navegación de un estudiante\n"
                  << "5. Solicitudes pendientes en cola\n"
                  << "6. Cantidad de nodos del árbol genérico\n"
                  << "7. Altura del árbol genérico\n"
                  << "8. Cantidad de estudiantes registrados\n"
                  << "9. Estudiante con menor y mayor carné\n"
                  << "10. Recursos hoja de un curso\n"
                  << "11. Estadísticas de uso de recursos\n"
                  << "12. Exportar todos los reportes a un archivo\n"
                  << "0. Volver al menú principal\n";
        long long opcion = Utils::leerEntero("Opción: ", 0, 12);

        std::cout << "\n";
        switch (opcion) {
            case 1: Reportes::listaEstudiantes(std::cout, estudiantes); break;
            case 2: Reportes::inordenABB(std::cout, estudiantes); break;
            case 3: reporteEstructuraCurso(cursos); break;
            case 4: reporteHistorial(estudiantes, cursos); break;
            case 5: Reportes::solicitudesPendientes(std::cout, solicitudes, estudiantes, cursos); break;
            case 6: Reportes::cantidadNodos(std::cout, cursos); break;
            case 7: Reportes::alturaArboles(std::cout, cursos); break;
            case 8: Reportes::cantidadEstudiantes(std::cout, estudiantes); break;
            case 9: Reportes::menorYMayorCarne(std::cout, estudiantes); break;
            case 10: reporteRecursosHoja(cursos); break;
            case 11: Reportes::estadisticasUso(std::cout, estudiantes, cursos); break;
            case 12: exportarReportes(estudiantes, cursos, solicitudes); break;
            default: return;
        }
        Utils::pausar();
    }
}
