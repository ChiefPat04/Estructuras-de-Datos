// ============================================================
// OperationParser.cpp  -  lectura y ejecucion de operaciones
// ============================================================

#include "OperationParser.h"

#include <cctype>
#include <cstddef>
#include <fstream>
#include <sstream>

// Funciones de ayuda que solo se usan en este archivo.
namespace {

    // Quita espacios, tabuladores y saltos de linea al inicio y al final.
    // Tambien quita el '\r' que dejan los archivos guardados en Windows.
    std::string quitarEspacios(const std::string& texto) {
        const std::string espacios = " \t\r\n";
        std::size_t inicio = texto.find_first_not_of(espacios);
        if (inicio == std::string::npos) return "";
        std::size_t fin = texto.find_last_not_of(espacios);
        return texto.substr(inicio, fin - inicio + 1);
    }

    // Todo lo que esta despues de '#' es un comentario.
    std::string quitarComentario(const std::string& linea) {
        std::size_t posicion = linea.find('#');
        return (posicion == std::string::npos) ? linea : linea.substr(0, posicion);
    }

    // Algunos editores (como el Bloc de notas) ponen 3 bytes invisibles al
    // inicio del archivo (el "BOM" de UTF-8). Si estan, se quitan.
    void quitarBOM(std::string& linea) {
        if (linea.size() >= 3 && static_cast<unsigned char>(linea[0]) == 0xEF
            && static_cast<unsigned char>(linea[1]) == 0xBB
            && static_cast<unsigned char>(linea[2]) == 0xBF) {
            linea.erase(0, 3);
        }
    }

    std::string aMayusculas(const std::string& texto) {
        std::string resultado = texto;
        for (std::size_t i = 0; i < resultado.size(); i++) {
            resultado[i] = static_cast<char>(
                std::toupper(static_cast<unsigned char>(resultado[i])));
        }
        return resultado;
    }

    // Parte una linea en palabras separadas por espacios.
    // Ejemplo: "ACCESS  P1 2500" -> {"ACCESS", "P1", "2500"}
    std::vector<std::string> separarPalabras(const std::string& linea) {
        std::vector<std::string> palabras;
        std::istringstream lector(linea);
        std::string palabra;
        while (lector >> palabra) {
            palabras.push_back(palabra);
        }
        return palabras;
    }

    // Convierte un texto a entero. Solo acepta digitos, con un signo opcional
    // al inicio. "12abc", "3.5" o "" no son validos. Maximo 15 digitos para
    // que el numero no se desborde.
    bool aEntero(const std::string& texto, long long& valor) {
        std::size_t i = 0;
        bool negativo = false;
        if (!texto.empty() && (texto[0] == '-' || texto[0] == '+')) {
            negativo = (texto[0] == '-');
            i = 1;
        }
        if (i >= texto.size() || texto.size() - i > 15) return false;

        long long resultado = 0;
        for (; i < texto.size(); i++) {
            if (!std::isdigit(static_cast<unsigned char>(texto[i]))) return false;
            resultado = resultado * 10 + (texto[i] - '0');
        }
        valor = negativo ? -resultado : resultado;
        return true;
    }

    bool esConfiguracion(const std::string& comando) {
        return comando == "MEMORIA_TOTAL" || comando == "TAMANO_PAGINA"
            || comando == "TAMANO_CACHE_GLOBAL" || comando == "PAGINAS_SWAP";
    }

    // Formato de cada operacion y cuantos datos lleva despues del nombre.
    // Devuelve false si la operacion no existe.
    bool formatoDe(const std::string& comando, std::string& formato, std::size_t& datos) {
        if (comando == "NEW_PROCESS") { formato = "NEW_PROCESS idProceso memoria"; datos = 2; return true; }
        if (comando == "END_PROCESS") { formato = "END_PROCESS idProceso"; datos = 1; return true; }
        if (comando == "ACCESS")      { formato = "ACCESS idProceso direccion"; datos = 2; return true; }
        if (comando == "ALLOCATE")    { formato = "ALLOCATE idProceso bytes"; datos = 2; return true; }
        if (comando == "FREE")        { formato = "FREE idProceso direccionInicial bytes"; datos = 3; return true; }
        return false;
    }

    const char* const SEPARADOR =
        "------------------------------------------------------------";
}

// ------------------------------------------------------------
// Construccion y ayudas
// ------------------------------------------------------------

OperationParser::OperationParser(std::ostream& salida)
    : salida_(salida), memoria_(),
      memoriaTotal_(0), tamanoPagina_(0), tamanoCacheGlobal_(0),
      hayMemoriaTotal_(false), hayTamanoPagina_(false), hayTamanoCache_(false),
      paginasSwap_(0), haySwap_(false), huboOperacion_(false),
      operaciones_(0), errores_(0), errorFatal_(false) {}

void OperationParser::reportarError(const std::string& mensaje) {
    errores_++;
    salida_ << "  ERROR: " << mensaje << "\n";
}

std::string OperationParser::configuracionFaltante() const {
    std::string faltan;
    if (!hayMemoriaTotal_) faltan += " MEMORIA_TOTAL";
    if (!hayTamanoPagina_) faltan += " TAMANO_PAGINA";
    if (!hayTamanoCache_) faltan += " TAMANO_CACHE_GLOBAL";
    return faltan;
}

void OperationParser::mostrarAyuda(std::ostream& salida) {
    salida << "Operaciones disponibles:\n"
           << "  NEW_PROCESS idProceso memoria        crea un proceso (memoria en bytes)\n"
           << "  END_PROCESS idProceso                termina un proceso y libera todo\n"
           << "  ACCESS idProceso direccion           referencia una direccion de memoria\n"
           << "  ALLOCATE idProceso bytes             pide mas memoria para el proceso\n"
           << "  FREE idProceso direccionInicial bytes  libera un rango de memoria\n"
           << "  ESTADO                               muestra el estado de la memoria\n"
           << "  METRICAS                             muestra las metricas\n"
           << "  AYUDA                                muestra esta lista\n"
           << "  ARBOL AVL                            dibuja el AVL de paginas libres\n"
           << "  ARBOL SPLAY idProceso                dibuja la tabla de paginas de un proceso\n"
           << "  EXPORTAR archivo.csv                 guarda las metricas en un archivo CSV\n"
           << "  PAGINAS_SWAP n                       activa el reemplazo de paginas (antes de operar)\n"
           << "  SALIR                                termina la simulacion\n";
}

// ------------------------------------------------------------
// Configuracion
// ------------------------------------------------------------

bool OperationParser::procesarConfiguracion(const std::string& nombre,
                                            const std::vector<std::string>& palabras) {
    if (nombre == "PAGINAS_SWAP") {
        procesarSwap(palabras);   // es opcional: sus errores no detienen la simulacion
        return true;
    }
    if (memoria_ != nullptr) {
        // Cambiar la memoria a mitad de la simulacion no tiene sentido: se ignora.
        reportarError("Archivo de entrada incorrecto: la configuracion ya esta definida; "
                      "esta linea se ignora");
        return true;
    }

    long long valor = 0;
    if (palabras.size() != 2 || !aEntero(palabras[1], valor)) {
        reportarError("Archivo de entrada incorrecto: se esperaba '" + nombre + " numero'");
        errorFatal_ = true;
        return false;
    }

    bool* yaDefinido = &hayMemoriaTotal_;
    long long* destino = &memoriaTotal_;
    if (nombre == "TAMANO_PAGINA") {
        yaDefinido = &hayTamanoPagina_;
        destino = &tamanoPagina_;
    } else if (nombre == "TAMANO_CACHE_GLOBAL") {
        yaDefinido = &hayTamanoCache_;
        destino = &tamanoCacheGlobal_;
    }

    if (*yaDefinido) {
        reportarError("Archivo de entrada incorrecto: " + nombre + " aparece dos veces");
        errorFatal_ = true;
        return false;
    }
    *destino = valor;
    *yaDefinido = true;
    salida_ << "  " << nombre << " = " << valor << "\n";

    // Cuando ya estan los tres valores, se crea la memoria.
    if (hayMemoriaTotal_ && hayTamanoPagina_ && hayTamanoCache_) {
        return crearMemoria();
    }
    return true;
}

bool OperationParser::crearMemoria() {
    std::string error;
    if (!MemoryManager::configValida(memoriaTotal_, tamanoPagina_, tamanoCacheGlobal_, error)) {
        reportarError(error);
        errorFatal_ = true;
        return false;
    }

    memoria_.reset(new MemoryManager(memoriaTotal_, static_cast<int>(tamanoPagina_),
                                     static_cast<int>(tamanoCacheGlobal_)));
    if (haySwap_) {
        memoria_->configurarSwap(paginasSwap_, error);   // el valor ya se reviso al leerlo
    }
    salida_ << "  Memoria configurada: " << memoriaTotal_ << " / " << tamanoPagina_
            << " = " << memoria_->paginasTotales() << " paginas fisicas"
            << " | cache global de " << tamanoCacheGlobal_ << " entradas\n";
    if (memoria_->reemplazoActivo()) {
        salida_ << "  Reemplazo de paginas activado: swap de " << memoria_->getCapacidadSwap()
                << " paginas, victima elegida por LRU\n";
    }
    salida_ << "  Estado inicial:\n";
    memoria_->mostrarEstado(salida_);
    return true;
}

// ------------------------------------------------------------
// Procesar una linea (lo usan el archivo y la consola)
// ------------------------------------------------------------

bool OperationParser::procesarLinea(const std::string& lineaOriginal, int numeroLinea) {
    std::string linea = quitarEspacios(quitarComentario(lineaOriginal));
    if (linea.empty()) {
        return true;   // linea vacia o solo comentario
    }

    std::vector<std::string> palabras = separarPalabras(linea);
    std::string comando = aMayusculas(palabras[0]);

    salida_ << "\n" << SEPARADOR << "\n";
    salida_ << "[Linea " << numeroLinea << "] " << linea << "\n";

    if (esConfiguracion(comando)) {
        return procesarConfiguracion(comando, palabras);
    }
    if (comando == "SALIR") {
        salida_ << "  Fin de la lectura de operaciones.\n";
        return false;
    }
    if (comando == "AYUDA") {
        mostrarAyuda(salida_);
        return true;
    }
    if (comando == "ARBOL" || comando == "EXPORTAR") {
        if (memoria_ == nullptr) {
            reportarError("Archivo de entrada incorrecto: antes de " + comando
                          + " falta definir" + configuracionFaltante());
        } else if (comando == "ARBOL") {
            mostrarArbol(palabras);
        } else {
            exportarMetricas(palabras);
        }
        return true;
    }

    std::string formato;
    std::size_t datos = 0;
    bool esOperacion = formatoDe(comando, formato, datos);
    if (!esOperacion && comando != "ESTADO" && comando != "METRICAS") {
        reportarError("Operacion desconocida: '" + palabras[0] + "' (escriba AYUDA para ver la lista)");
        return true;
    }

    if (memoria_ == nullptr) {
        reportarError("Archivo de entrada incorrecto: antes de " + comando
                      + " falta definir" + configuracionFaltante());
        return true;
    }

    if (comando == "ESTADO") {
        memoria_->mostrarEstado(salida_);
    } else if (comando == "METRICAS") {
        memoria_->mostrarMetricas(salida_);
    } else {
        ejecutarOperacion(comando, palabras);
    }
    return true;
}

// ------------------------------------------------------------
// Ejecutar una operacion de memoria
// ------------------------------------------------------------

void OperationParser::ejecutarOperacion(const std::string& comando,
                                        const std::vector<std::string>& palabras) {
    huboOperacion_ = true;
    std::string formato;
    std::size_t datos = 0;
    formatoDe(comando, formato, datos);

    // 1. Cantidad de datos correcta.
    if (palabras.size() - 1 != datos) {
        reportarError("Archivo de entrada incorrecto: el formato es '" + formato + "'");
        return;
    }

    // 2. Despues del id, todos los datos son numeros enteros.
    std::vector<long long> numeros;
    for (std::size_t i = 2; i < palabras.size(); i++) {
        long long valor = 0;
        if (!aEntero(palabras[i], valor)) {
            reportarError("Archivo de entrada incorrecto: '" + palabras[i]
                          + "' no es un numero entero valido");
            return;
        }
        numeros.push_back(valor);
    }

    // 3. Ejecutar.
    const std::string& id = palabras[1];
    std::string error;
    bool exito = false;

    if (comando == "NEW_PROCESS") {
        exito = memoria_->nuevoProceso(id, numeros[0], error);
        if (exito) {
            int paginas = memoria_->paginasDe(id);
            salida_ << "  Proceso " << id << " creado con " << paginas
                    << " paginas (virtuales 0-" << (paginas - 1) << ")"
                    << ", cache local de " << memoria_->cacheLocalDe(id)->capacidad() << "\n";
        }
    } else if (comando == "END_PROCESS") {
        int paginas = memoria_->paginasDe(id);
        int enSwap = memoria_->paginasEnSwapDe(id);
        exito = memoria_->finalizarProceso(id, error);
        if (exito && !memoria_->reemplazoActivo()) {
            salida_ << "  Proceso " << id << " finalizado: sus " << paginas
                    << " paginas fisicas vuelven al AVL de libres\n";
        } else if (exito) {
            salida_ << "  Proceso " << id << " finalizado: " << (paginas - enSwap)
                    << " paginas fisicas vuelven al AVL de libres y " << enSwap
                    << " paginas salen de swap\n";
        }
    } else if (comando == "ACCESS") {
        ResultadoAcceso resultado;
        exito = memoria_->acceder(id, numeros[0], resultado, error);
        if (exito) {
            salida_ << "  Pagina virtual: " << resultado.paginaVirtual << "\n"
                    << "  Desplazamiento: " << resultado.desplazamiento << "\n"
                    << "  Resultado: " << resultado.detalle << "\n"
                    << "  Pagina fisica: " << resultado.paginaFisica << "\n";
        }
    } else if (comando == "ALLOCATE") {
        int antes = memoria_->paginasDe(id);
        exito = memoria_->asignarMemoria(id, numeros[0], error);
        if (exito) {
            int despues = memoria_->paginasDe(id);
            salida_ << "  " << id << " recibio " << (despues - antes)
                    << " paginas mas (ahora tiene " << despues << ")"
                    << ", cache local de " << memoria_->cacheLocalDe(id)->capacidad() << "\n";
        }
    } else if (comando == "FREE") {
        exito = memoria_->liberarMemoria(id, numeros[0], numeros[1], error);
        if (exito) {
            long long tamano = memoria_->getTamanoPagina();
            long long primera = numeros[0] / tamano;
            long long ultima = (numeros[0] + numeros[1] - 1) / tamano;
            salida_ << "  " << id << " libero las paginas virtuales " << primera
                    << " a " << ultima << " (" << (ultima - primera + 1) << " paginas)"
                    << ", cache local de " << memoria_->cacheLocalDe(id)->capacidad() << "\n";
        }
    }

    if (!exito) {
        reportarError(error);
        salida_ << "  (La memoria no cambio.)\n";
        return;
    }

    operaciones_++;
    // Extra: fallos de pagina y reemplazos que provoco la operacion.
    const std::vector<std::string>& eventos = memoria_->getEventos();
    for (std::size_t i = 0; i < eventos.size(); i++) {
        salida_ << "  " << eventos[i] << "\n";
    }
    salida_ << "  Estado de la memoria:\n";
    memoria_->mostrarEstado(salida_);
}

// ------------------------------------------------------------
// Modo archivo
// ------------------------------------------------------------

bool OperationParser::procesarFlujo(std::istream& entrada) {
    std::string linea;
    int numeroLinea = 0;
    while (std::getline(entrada, linea)) {
        numeroLinea++;
        if (numeroLinea == 1) quitarBOM(linea);
        if (!procesarLinea(linea, numeroLinea)) break;
    }

    if (errorFatal_) {
        salida_ << "\n  La simulacion se detuvo: sin una configuracion valida no se puede continuar.\n";
        return false;
    }
    if (memoria_ == nullptr) {
        salida_ << "\n";
        reportarError("Archivo de entrada incorrecto: el archivo termino sin definir"
                      + configuracionFaltante());
        errorFatal_ = true;
        return false;
    }
    return true;
}

bool OperationParser::procesarArchivo(const std::string& ruta) {
    std::ifstream archivo(ruta.c_str());
    if (!archivo) {
        reportarError("Archivo de entrada incorrecto: no se pudo abrir '" + ruta + "'");
        errorFatal_ = true;
        return false;
    }
    salida_ << "Leyendo operaciones del archivo: " << ruta << "\n";
    return procesarFlujo(archivo);
}

// ------------------------------------------------------------
// Modo consola
// ------------------------------------------------------------

bool OperationParser::pedirNumero(std::istream& entrada, const std::string& mensaje,
                                  long long& valor) {
    std::string linea;
    while (true) {
        salida_ << mensaje;
        salida_.flush();
        if (!std::getline(entrada, linea)) {
            return false;   // se acabo la entrada (Ctrl+Z en Windows)
        }
        if (aEntero(quitarEspacios(linea), valor)) {
            return true;
        }
        salida_ << "  Entrada incorrecta: escriba solo un numero entero.\n";
    }
}

bool OperationParser::pedirConfiguracion(std::istream& entrada) {
    salida_ << "Configuracion inicial (escriba un numero entero y presione ENTER).\n";
    while (true) {
        long long memoria = 0, pagina = 0, cache = 0;
        if (!pedirNumero(entrada, "  MEMORIA_TOTAL en bytes (ejemplo 65536): ", memoria)) return false;
        if (!pedirNumero(entrada, "  TAMANO_PAGINA en bytes (ejemplo 1024): ", pagina)) return false;
        if (!pedirNumero(entrada, "  TAMANO_CACHE_GLOBAL en entradas (ejemplo 10): ", cache)) return false;

        std::string error;
        if (MemoryManager::configValida(memoria, pagina, cache, error)) {
            memoriaTotal_ = memoria;
            tamanoPagina_ = pagina;
            tamanoCacheGlobal_ = cache;
            hayMemoriaTotal_ = hayTamanoPagina_ = hayTamanoCache_ = true;
            return crearMemoria();
        }
        reportarError(error);
        salida_ << "  Intente de nuevo.\n";
    }
}

void OperationParser::procesarConsola(std::istream& entrada) {
    if (!pedirConfiguracion(entrada)) {
        salida_ << "\n  No se completo la configuracion.\n";
        return;
    }

    salida_ << "\n";
    mostrarAyuda(salida_);

    std::string linea;
    int numeroLinea = 0;
    while (true) {
        salida_ << "\n> ";
        salida_.flush();
        if (!std::getline(entrada, linea)) break;
        numeroLinea++;
        if (!procesarLinea(linea, numeroLinea)) break;
    }
}

// ------------------------------------------------------------
// Resumen final
// ------------------------------------------------------------

void OperationParser::mostrarResumen() const {
    salida_ << "\n============================================================\n";
    salida_ << "RESUMEN FINAL\n";
    salida_ << "============================================================\n";
    salida_ << "  Operaciones ejecutadas con exito: " << operaciones_ << "\n";
    salida_ << "  Errores detectados:               " << errores_ << "\n";

    if (memoria_ == nullptr) {
        salida_ << "  La memoria no se llego a configurar.\n";
        return;
    }
    salida_ << "\nMetricas:\n";
    memoria_->mostrarMetricas(salida_);
    salida_ << "\nEstado final de la memoria:\n";
    memoria_->mostrarEstado(salida_);
}

// ------------------------------------------------------------
// Extras: ARBOL y EXPORTAR
// ------------------------------------------------------------

void OperationParser::mostrarArbol(const std::vector<std::string>& palabras) {
    std::string tipo = (palabras.size() >= 2) ? aMayusculas(palabras[1]) : "";

    if (palabras.size() == 2 && tipo == "AVL") {
        salida_ << "  AVL de paginas fisicas libres (clave = numero de pagina).\n"
                << "  La raiz es la linea sin sangria; los hijos de la derecha van arriba\n"
                << "  y los de la izquierda abajo.\n";
        memoria_->mostrarArbolLibres(salida_);
        return;
    }
    if (palabras.size() == 3 && tipo == "SPLAY") {
        const std::string& id = palabras[2];
        if (!memoria_->existeProceso(id)) {
            reportarError("Proceso inexistente: " + id);
            return;
        }
        salida_ << "  Splay Tree de " << id << " (clave = pagina virtual).\n"
                << "  La raiz es la linea sin sangria; suele ser la pagina usada mas recientemente.\n";
        memoria_->mostrarArbolProceso(id, salida_);
        return;
    }
    reportarError("Archivo de entrada incorrecto: el formato es 'ARBOL AVL' o 'ARBOL SPLAY idProceso'");
}

void OperationParser::exportarMetricas(const std::vector<std::string>& palabras) {
    if (palabras.size() != 2) {
        reportarError("Archivo de entrada incorrecto: el formato es 'EXPORTAR nombreArchivo.csv'");
        return;
    }
    const std::string& ruta = palabras[1];
    std::ofstream archivo(ruta.c_str());
    if (!archivo) {
        reportarError("No se pudo crear el archivo '" + ruta + "'");
        return;
    }
    memoria_->escribirMetricasCsv(archivo);
    salida_ << "  Metricas exportadas a " << ruta << " (CSV: se puede abrir con Excel)\n";
}

// ------------------------------------------------------------
// Extra: PAGINAS_SWAP (reemplazo de paginas)
// ------------------------------------------------------------

void OperationParser::procesarSwap(const std::vector<std::string>& palabras) {
    long long valor = 0;
    if (palabras.size() != 2 || !aEntero(palabras[1], valor)) {
        reportarError("Archivo de entrada incorrecto: se esperaba 'PAGINAS_SWAP numero'; "
                      "esta linea se ignora");
        return;
    }
    if (haySwap_) {
        reportarError("Archivo de entrada incorrecto: PAGINAS_SWAP aparece dos veces; "
                      "esta linea se ignora");
        return;
    }
    if (huboOperacion_) {
        reportarError("Archivo de entrada incorrecto: PAGINAS_SWAP debe ir antes de la "
                      "primera operacion; esta linea se ignora");
        return;
    }
    if (valor < 0 || valor > MemoryManager::MAX_PAGINAS_SWAP) {
        reportarError("Archivo de entrada incorrecto: PAGINAS_SWAP debe estar entre 0 y "
                      + std::string("1048576; esta linea se ignora"));
        return;
    }

    paginasSwap_ = valor;
    haySwap_ = true;
    salida_ << "  PAGINAS_SWAP = " << valor << "\n";

    // Si la memoria ya existe (la configuracion obligatoria ya estaba), se
    // activa de una vez; si no, se activa al crear la memoria.
    if (memoria_ != nullptr) {
        std::string error;
        if (!memoria_->configurarSwap(valor, error)) {
            reportarError(error + "; esta linea se ignora");
            return;
        }
        if (memoria_->reemplazoActivo()) {
            salida_ << "  Reemplazo de paginas activado: swap de " << valor
                    << " paginas, victima elegida por LRU\n";
        } else {
            salida_ << "  Reemplazo de paginas desactivado (0 paginas de swap)\n";
        }
    }
}
