// MenuSolicitudes.cpp - solicitudes pendientes (cola y cola con prioridad)

#include "Menus.h"

#include <iostream>
#include <vector>
#include "Utils.h"

static const long long CARNE_MAXIMO = 9999999999LL;

static void nuevaSolicitud(GestorSolicitudes& solicitudes, const GestorEstudiantes& estudiantes,
                           const GestorCursos& cursos) {
    std::cout << "\n--- Nueva solicitud ---\n";
    long long carne = Utils::leerEntero("Carné del estudiante: ", 1, CARNE_MAXIMO);
    if (estudiantes.buscarPorCarne(carne) == nullptr) {
        std::cout << "No existe ningún estudiante con ese carné.\n";
        return;
    }
    if (cursos.vacio()) {
        std::cout << "No hay cursos registrados.\n";
        return;
    }
    NodoCurso* nodo = elegirNodo(cursos, "Número del contenido relacionado");
    if (nodo == nullptr) return;

    std::cout << "Tipo de solicitud:\n"
              << "  1. Revisión de tarea (prioridad baja)\n"
              << "  2. Consulta académica (prioridad baja)\n"
              << "  3. Habilitación de recurso (prioridad media)\n"
              << "  4. Reporte de error en material (prioridad alta)\n";
    long long opcion = Utils::leerEntero("Tipo: ", 1, 4);
    TipoSolicitud tipo = TipoSolicitud::RevisionTarea;
    if (opcion == 2) tipo = TipoSolicitud::ConsultaAcademica;
    if (opcion == 3) tipo = TipoSolicitud::HabilitacionRecurso;
    if (opcion == 4) tipo = TipoSolicitud::ReporteError;

    std::string detalle = Utils::leerTexto("Descripción: ");
    std::string error;
    if (solicitudes.insertar(carne, nodo->getId(), tipo, detalle, error)) {
        std::cout << "Solicitud agregada a la cola.\n";
    } else {
        std::cout << "No se pudo agregar: " << error << "\n";
    }
}

static void atender(GestorSolicitudes& solicitudes, const GestorEstudiantes& estudiantes,
                    const GestorCursos& cursos) {
    Solicitud s;
    if (solicitudes.atenderSiguiente(s)) {
        std::cout << "\nSe atendió:\n";
        Reportes::imprimirSolicitud(std::cout, s, estudiantes, cursos);
    } else {
        std::cout << "No hay solicitudes pendientes.\n";
    }
}

static void verFrente(const GestorSolicitudes& solicitudes, const GestorEstudiantes& estudiantes,
                      const GestorCursos& cursos) {
    Solicitud s;
    if (solicitudes.verFrente(s)) {
        std::cout << "\nSiguiente en ser atendida:\n";
        Reportes::imprimirSolicitud(std::cout, s, estudiantes, cursos);
    } else {
        std::cout << "No hay solicitudes pendientes.\n";
    }
}

static void mostrarTodas(const GestorSolicitudes& solicitudes, const GestorEstudiantes& estudiantes,
                         const GestorCursos& cursos) {
    std::vector<Solicitud> todas = solicitudes.todas();
    if (todas.empty()) {
        std::cout << "No hay solicitudes pendientes.\n";
        return;
    }
    std::cout << "\nPendientes, en el orden en que se atenderán:\n";
    for (std::size_t i = 0; i < todas.size(); i++) {
        Reportes::imprimirSolicitud(std::cout, todas[i], estudiantes, cursos);
    }
}

static void contar(const GestorSolicitudes& solicitudes) {
    std::cout << "\nSolicitudes en espera: " << solicitudes.cantidad() << "\n";
    for (int nivel = 0; nivel < 3; nivel++) {
        std::cout << "  Prioridad " << nombrePrioridad(nivel) << ": "
                  << solicitudes.cantidadEnNivel(nivel) << "\n";
    }
    std::cout << "Ya atendidas: " << solicitudes.atendidas() << "\n";
}

static void cambiarModo(GestorSolicitudes& solicitudes) {
    bool prioridad = !solicitudes.getModoPrioridad();
    solicitudes.setModoPrioridad(prioridad);
    std::cout << "Ahora se atiende por "
              << (prioridad ? "PRIORIDAD (alta, media, baja)" : "ORDEN DE LLEGADA") << ".\n";
}

void menuSolicitudes(GestorSolicitudes& solicitudes, const GestorEstudiantes& estudiantes,
                     const GestorCursos& cursos) {
    while (true) {
        std::cout << "\n===== SOLICITUDES PENDIENTES =====\n"
                  << "Modo de atención: "
                  << (solicitudes.getModoPrioridad() ? "por prioridad" : "por orden de llegada") << "\n"
                  << "1. Nueva solicitud\n"
                  << "2. Atender la siguiente\n"
                  << "3. Mostrar la solicitud al frente\n"
                  << "4. Mostrar todas las pendientes\n"
                  << "5. Contar solicitudes en espera\n"
                  << "6. Cambiar modo de atención (prioridad / llegada)\n"
                  << "0. Volver al menú principal\n";
        long long opcion = Utils::leerEntero("Opción: ", 0, 6);

        switch (opcion) {
            case 1: nuevaSolicitud(solicitudes, estudiantes, cursos); break;
            case 2: atender(solicitudes, estudiantes, cursos); break;
            case 3: verFrente(solicitudes, estudiantes, cursos); break;
            case 4: mostrarTodas(solicitudes, estudiantes, cursos); break;
            case 5: contar(solicitudes); break;
            case 6: cambiarModo(solicitudes); break;
            default: return;
        }
        Utils::pausar();
    }
}
