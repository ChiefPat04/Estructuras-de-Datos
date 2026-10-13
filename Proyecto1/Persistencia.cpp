// Persistencia.cpp

#include "Persistencia.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include "Utils.h"

static const std::string ARCHIVO_ESTUDIANTES = "datos_estudiantes.txt";
static const std::string ARCHIVO_CURSOS = "datos_cursos.txt";
static const std::string ARCHIVO_HISTORIALES = "datos_historiales.txt";
static const std::string ARCHIVO_SOLICITUDES = "datos_solicitudes.txt";

typedef std::vector<std::string> Campos;

// ----- Ayudas -----

// El '|' separa campos, así que no puede aparecer dentro de un texto.
static std::string limpiarTexto(const std::string& texto) {
    std::string resultado = texto;
    std::replace(resultado.begin(), resultado.end(), '|', '/');
    return resultado;
}

static std::string aTexto(long long numero) {
    std::ostringstream salida;
    salida << numero;
    return salida.str();
}

static std::string errorLinea(const std::string& archivo, std::size_t linea, const std::string& motivo) {
    return archivo + ", línea " + aTexto(static_cast<long long>(linea)) + ": " + motivo;
}

// Lee todas las líneas de un archivo (sin el '\r' de Windows).
static bool leerLineas(const std::string& archivo, std::vector<std::string>& lineas,
                       std::string& error) {
    std::ifstream entrada(archivo.c_str());
    if (!entrada) {
        error = "No se encontró el archivo " + archivo + ".";
        return false;
    }
    std::string linea;
    while (std::getline(entrada, linea)) {
        lineas.push_back(Utils::quitarEspacios(linea));
    }
    return true;
}

// ----- Guardar -----

static bool guardarEstudiantes(const GestorEstudiantes& estudiantes, std::string& error) {
    std::ofstream salida(ARCHIVO_ESTUDIANTES.c_str());
    if (!salida) {
        error = "No se pudo crear " + ARCHIVO_ESTUDIANTES + ".";
        return false;
    }
    for (const Estudiante& e : estudiantes.todos()) {
        salida << e.getCarne() << "|" << limpiarTexto(e.getNombre()) << "|"
               << limpiarTexto(e.getCorreo()) << "\n";
    }
    return true;
}

// Cada curso se guarda en preorden, así el padre siempre queda antes que sus hijos.
static bool guardarCursos(const GestorCursos& cursos, std::string& error) {
    std::ofstream salida(ARCHIVO_CURSOS.c_str());
    if (!salida) {
        error = "No se pudo crear " + ARCHIVO_CURSOS + ".";
        return false;
    }
    for (const ArbolGenerico* curso : cursos.cursos()) {
        std::vector<NodoCurso*> nodos = curso->preorden();
        for (const NodoCurso* nodo : nodos) {
            int idPadre = (nodo->getPadre() == nullptr) ? 0 : nodo->getPadre()->getId();
            salida << nodo->getId() << "|" << idPadre << "|" << nombreTipo(nodo->getTipo()) << "|"
                   << limpiarTexto(nodo->getNombre()) << "|" << nodo->getVisitas() << "\n";
        }
    }
    return true;
}

// Las pilas se guardan de la más antigua a la más reciente, para poder
// volver a apilar en el mismo orden al cargar.
static bool guardarHistoriales(const GestorEstudiantes& estudiantes, std::string& error) {
    std::ofstream salida(ARCHIVO_HISTORIALES.c_str());
    if (!salida) {
        error = "No se pudo crear " + ARCHIVO_HISTORIALES + ".";
        return false;
    }
    for (const Estudiante& e : estudiantes.todos()) {
        const HistorialAcademico& h = e.getHistorial();
        long long carne = e.getCarne();

        std::vector<int> paginas = h.paginas();
        for (std::size_t i = paginas.size(); i > 0; i--) {
            salida << "P|" << carne << "|" << paginas[i - 1] << "\n";
        }
        std::vector<Accion> acciones = h.acciones();
        for (std::size_t i = acciones.size(); i > 0; i--) {
            salida << "A|" << carne << "|" << static_cast<int>(acciones[i - 1].tipo)
                   << "|" << acciones[i - 1].idNodo << "\n";
        }
        for (int id : h.completados()) {
            salida << "C|" << carne << "|" << id << "\n";
        }
        for (int id : h.inscripciones()) {
            salida << "I|" << carne << "|" << id << "\n";
        }
    }
    return true;
}

static bool guardarSolicitudes(const GestorSolicitudes& solicitudes, std::string& error) {
    std::ofstream salida(ARCHIVO_SOLICITUDES.c_str());
    if (!salida) {
        error = "No se pudo crear " + ARCHIVO_SOLICITUDES + ".";
        return false;
    }
    salida << "config|" << (solicitudes.getModoPrioridad() ? 1 : 0) << "|"
           << solicitudes.getProximoNumero() << "|" << solicitudes.atendidas() << "\n";
    std::vector<Solicitud> pendientes = solicitudes.enOrdenDeLlegada();
    for (const Solicitud& s : pendientes) {
        salida << s.numero << "|" << s.carne << "|" << s.idNodo << "|"
               << static_cast<int>(s.tipo) << "|" << limpiarTexto(s.descripcion) << "\n";
    }
    return true;
}

// ----- Cargar -----

static bool cargarEstudiantes(GestorEstudiantes& estudiantes, std::string& error) {
    std::vector<std::string> lineas;
    if (!leerLineas(ARCHIVO_ESTUDIANTES, lineas, error)) return false;

    for (std::size_t i = 0; i < lineas.size(); i++) {
        if (lineas[i].empty()) continue;
        Campos c = Utils::dividir(lineas[i], '|');
        long long carne;
        if (c.size() != 3 || !Utils::aEntero(c[0], carne)) {
            error = errorLinea(ARCHIVO_ESTUDIANTES, i + 1, "formato incorrecto.");
            return false;
        }
        std::string motivo;
        if (!estudiantes.registrar(carne, c[1], c[2], motivo)) {
            error = errorLinea(ARCHIVO_ESTUDIANTES, i + 1, motivo);
            return false;
        }
    }
    return true;
}

static bool cargarCursos(GestorCursos& cursos, std::string& error) {
    std::vector<std::string> lineas;
    if (!leerLineas(ARCHIVO_CURSOS, lineas, error)) return false;

    for (std::size_t i = 0; i < lineas.size(); i++) {
        if (lineas[i].empty()) continue;
        Campos c = Utils::dividir(lineas[i], '|');
        long long id, idPadre, visitas;
        TipoNodo tipo;
        if (c.size() != 5 || !Utils::aEntero(c[0], id) || !Utils::aEntero(c[1], idPadre) ||
            !tipoDesdeTexto(c[2], tipo) || !Utils::aEntero(c[4], visitas) ||
            id <= 0 || id > 2147483647LL || idPadre < 0 || idPadre > 2147483647LL ||
            visitas < 0 || visitas > 2147483647LL) {
            error = errorLinea(ARCHIVO_CURSOS, i + 1, "formato incorrecto.");
            return false;
        }
        std::string motivo;
        NodoCurso* nodo = nullptr;
        if (tipo == TipoNodo::Curso) {
            ArbolGenerico* curso = cursos.crearCursoConId(static_cast<int>(id), c[3], motivo);
            if (curso != nullptr) nodo = curso->getRaiz();
        } else {
            nodo = cursos.agregarHijoConId(static_cast<int>(idPadre), static_cast<int>(id),
                                           tipo, c[3], motivo);
        }
        if (nodo == nullptr) {
            error = errorLinea(ARCHIVO_CURSOS, i + 1, motivo);
            return false;
        }
        nodo->setVisitas(static_cast<int>(visitas));
    }
    return true;
}

static bool cargarHistoriales(GestorEstudiantes& estudiantes, std::string& error) {
    std::vector<std::string> lineas;
    if (!leerLineas(ARCHIVO_HISTORIALES, lineas, error)) return false;

    for (std::size_t i = 0; i < lineas.size(); i++) {
        if (lineas[i].empty()) continue;
        Campos c = Utils::dividir(lineas[i], '|');
        std::string tipo = c[0];
        bool esAccion = (tipo == "A");
        std::size_t esperados = esAccion ? 4 : 3;

        long long carne, idNodo, tipoAccion = 0;
        bool formatoOk = (tipo == "P" || tipo == "C" || tipo == "I" || esAccion) &&
                         c.size() == esperados && Utils::aEntero(c[1], carne) &&
                         Utils::aEntero(c[esperados - 1], idNodo) && idNodo > 0 && idNodo <= 2147483647LL;
        if (formatoOk && esAccion) {
            formatoOk = Utils::aEntero(c[2], tipoAccion) && tipoAccion >= 0 && tipoAccion <= 2;
        }
        if (!formatoOk) {
            error = errorLinea(ARCHIVO_HISTORIALES, i + 1, "formato incorrecto.");
            return false;
        }
        Estudiante* e = estudiantes.buscarPorCarne(carne);
        if (e == nullptr) {
            error = errorLinea(ARCHIVO_HISTORIALES, i + 1, "el estudiante no existe.");
            return false;
        }
        HistorialAcademico& h = e->getHistorial();
        int id = static_cast<int>(idNodo);
        if (tipo == "P") h.agregarPagina(id);
        if (tipo == "C") h.marcarCompletado(id);
        if (tipo == "I") h.inscribir(id);
        if (esAccion) h.registrarAccion(static_cast<TipoAccion>(tipoAccion), id);
    }
    return true;
}

static bool cargarSolicitudes(GestorSolicitudes& solicitudes, std::string& error) {
    std::vector<std::string> lineas;
    if (!leerLineas(ARCHIVO_SOLICITUDES, lineas, error)) return false;

    bool hayConfig = false;
    long long proximo = 1, atendidas = 0, mayorNumero = 0;
    for (std::size_t i = 0; i < lineas.size(); i++) {
        if (lineas[i].empty()) continue;
        Campos c = Utils::dividir(lineas[i], '|');

        if (c[0] == "config") {
            long long modo;
            if (c.size() != 4 || !Utils::aEntero(c[1], modo) || !Utils::aEntero(c[2], proximo) ||
                !Utils::aEntero(c[3], atendidas) || proximo < 1 || atendidas < 0) {
                error = errorLinea(ARCHIVO_SOLICITUDES, i + 1, "formato incorrecto.");
                return false;
            }
            solicitudes.setModoPrioridad(modo == 1);
            hayConfig = true;
            continue;
        }

        long long numero, carne, idNodo, tipo;
        if (c.size() < 5 || !Utils::aEntero(c[0], numero) || !Utils::aEntero(c[1], carne) ||
            !Utils::aEntero(c[2], idNodo) || !Utils::aEntero(c[3], tipo) ||
            numero < 1 || idNodo < 1 || idNodo > 2147483647LL || tipo < 0 || tipo > 3) {
            error = errorLinea(ARCHIVO_SOLICITUDES, i + 1, "formato incorrecto.");
            return false;
        }
        Solicitud s;
        s.numero = static_cast<int>(numero);
        s.carne = carne;
        s.idNodo = static_cast<int>(idNodo);
        s.tipo = static_cast<TipoSolicitud>(tipo);
        s.descripcion = c[4];
        solicitudes.restaurar(s);
        if (numero > mayorNumero) mayorNumero = numero;
    }
    if (!hayConfig) {
        error = ARCHIVO_SOLICITUDES + ": falta la línea de configuración.";
        return false;
    }
    if (proximo <= mayorNumero) proximo = mayorNumero + 1;
    solicitudes.setContadores(static_cast<int>(proximo), static_cast<int>(atendidas));
    return true;
}

// Carga los cuatro archivos en gestores que ya están vacíos.
static bool cargarEn(GestorEstudiantes& estudiantes, GestorCursos& cursos,
                     GestorSolicitudes& solicitudes, std::string& error) {
    return cargarEstudiantes(estudiantes, error) &&
           cargarCursos(cursos, error) &&
           cargarHistoriales(estudiantes, error) &&
           cargarSolicitudes(solicitudes, error);
}

// ----- Funciones públicas -----

namespace Persistencia {

bool guardar(const GestorEstudiantes& estudiantes, const GestorCursos& cursos,
             const GestorSolicitudes& solicitudes, std::string& error) {
    return guardarEstudiantes(estudiantes, error) &&
           guardarCursos(cursos, error) &&
           guardarHistoriales(estudiantes, error) &&
           guardarSolicitudes(solicitudes, error);
}

// Primero se prueba la carga en gestores temporales. Solo si sale bien se
// borran los datos actuales y se carga de verdad.
bool cargar(GestorEstudiantes& estudiantes, GestorCursos& cursos,
            GestorSolicitudes& solicitudes, std::string& error) {
    {
        GestorEstudiantes pruebaEstudiantes;
        GestorCursos pruebaCursos;
        GestorSolicitudes pruebaSolicitudes(pruebaEstudiantes, pruebaCursos);
        if (!cargarEn(pruebaEstudiantes, pruebaCursos, pruebaSolicitudes, error)) {
            return false;
        }
    }
    solicitudes.limpiar();
    estudiantes.limpiar();
    cursos.limpiar();
    return cargarEn(estudiantes, cursos, solicitudes, error);
}

bool hayDatosGuardados() {
    const std::string archivos[4] = { ARCHIVO_ESTUDIANTES, ARCHIVO_CURSOS,
                                      ARCHIVO_HISTORIALES, ARCHIVO_SOLICITUDES };
    for (int i = 0; i < 4; i++) {
        std::ifstream entrada(archivos[i].c_str());
        if (!entrada) return false;
    }
    return true;
}

}
