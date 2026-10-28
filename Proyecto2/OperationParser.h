// OperationParser.h - lee operaciones (de un archivo o de la consola),
// revisa que esten bien escritas y se las pasa al MemoryManager.
// Despues de cada operacion muestra el resultado y el estado de la memoria.
//
// Los dos modos (archivo y consola) usan la misma funcion procesarLinea(),
// asi que aceptan los mismos comandos y dan los mismos mensajes.
//
// Formato de cada linea:
//   MEMORIA_TOTAL n | TAMANO_PAGINA n | TAMANO_CACHE_GLOBAL n   (configuracion)
//   NEW_PROCESS idProceso memoria
//   END_PROCESS idProceso
//   ACCESS idProceso direccion
//   ALLOCATE idProceso bytes
//   FREE idProceso direccionInicial bytes
//   ESTADO | METRICAS | AYUDA | SALIR                           (extras)
// - Las lineas vacias se ignoran.
// - Todo lo que esta despues de '#' es un comentario.
// - Los comandos aceptan mayusculas o minusculas; los id de proceso no.

#ifndef OPERATION_PARSER_H
#define OPERATION_PARSER_H

#include <istream>
#include <memory>
#include <ostream>
#include <string>
#include <vector>
#include "MemoryManager.h"

class OperationParser {
private:
    std::ostream& salida_;
    std::unique_ptr<MemoryManager> memoria_;   // nullptr hasta tener la configuracion

    // La configuracion se va leyendo linea por linea (en cualquier orden).
    long long memoriaTotal_;
    long long tamanoPagina_;
    long long tamanoCacheGlobal_;
    bool hayMemoriaTotal_;
    bool hayTamanoPagina_;
    bool hayTamanoCache_;

    int operaciones_;     // operaciones ejecutadas sin error
    int errores_;         // errores detectados
    bool errorFatal_;     // configuracion invalida: no se puede seguir

    void reportarError(const std::string& mensaje);
    std::string configuracionFaltante() const;

    // Devuelve false si el error impide seguir (configuracion invalida).
    bool procesarConfiguracion(const std::string& nombre,
                               const std::vector<std::string>& palabras);
    bool crearMemoria();

    void ejecutarOperacion(const std::string& comando,
                           const std::vector<std::string>& palabras);

    // Solo para la consola: pide un numero hasta que escriban uno valido.
    bool pedirNumero(std::istream& entrada, const std::string& mensaje, long long& valor);
    bool pedirConfiguracion(std::istream& entrada);

public:
    explicit OperationParser(std::ostream& salida);

    OperationParser(const OperationParser&) = delete;
    OperationParser& operator=(const OperationParser&) = delete;

    // Procesa una linea. Devuelve false si hay que dejar de leer
    // (por SALIR o por un error fatal de configuracion).
    bool procesarLinea(const std::string& linea, int numeroLinea);

    // Lee todas las lineas de un flujo (un archivo abierto, por ejemplo).
    // Devuelve false si hubo un error fatal.
    bool procesarFlujo(std::istream& entrada);

    // Abre el archivo y lo procesa. Devuelve false si no se pudo abrir
    // o si la configuracion no era valida.
    bool procesarArchivo(const std::string& ruta);

    // Modo interactivo: pide la configuracion y luego lee operaciones
    // hasta que escriban SALIR.
    void procesarConsola(std::istream& entrada);

    void mostrarResumen() const;
    static void mostrarAyuda(std::ostream& salida);

    bool configuracionLista() const { return memoria_ != nullptr; }
    bool huboErrorFatal() const { return errorFatal_; }
    int getOperaciones() const { return operaciones_; }
    int getErrores() const { return errores_; }
    const MemoryManager* getMemoria() const { return memoria_.get(); }
};

#endif
