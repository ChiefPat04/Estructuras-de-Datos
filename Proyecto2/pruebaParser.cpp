// pruebaParser.cpp - pruebas del OperationParser (lectura de archivo y consola).
// En vez de un archivo real se usa un istringstream (un "archivo en memoria"),
// y la salida se guarda en un ostringstream para revisarla.
// Tiene su propio main, se compila aparte con probar_parser.bat

#include <cstdio>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include "OperationParser.h"

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

static bool contiene(const std::string& texto, const std::string& buscado) {
    return texto.find(buscado) != std::string::npos;
}

static const std::string CONFIG =
    "MEMORIA_TOTAL 65536\nTAMANO_PAGINA 1024\nTAMANO_CACHE_GLOBAL 8\n";

static void pruebaArchivoValido() {
    std::cout << "\n== Archivo valido (ejemplo del enunciado) ==\n";
    std::istringstream entrada(CONFIG +
        "NEW_PROCESS P1 4096\nNEW_PROCESS P2 8192\nACCESS P1 100\nACCESS P1 2500\n"
        "ACCESS P2 7000\nALLOCATE P1 2048\nFREE P2 2048 2048\nEND_PROCESS P1\nEND_PROCESS P2\n");
    std::ostringstream salida;
    OperationParser parser(salida);
    verificar(parser.procesarFlujo(entrada), "el archivo se procesa completo");
    verificar(parser.getOperaciones() == 9 && parser.getErrores() == 0, "9 operaciones y 0 errores");
    verificar(contiene(salida.str(), "Pagina virtual: 2") && contiene(salida.str(), "Desplazamiento: 452"),
              "ACCESS P1 2500 muestra pagina virtual 2 y desplazamiento 452");
    verificar(parser.getMemoria()->paginasLibres() == 64, "al final todas las paginas quedan libres");
}

static void pruebaComentariosYFormato() {
    std::cout << "\n== Comentarios, lineas vacias, minusculas, BOM y finales de linea de Windows ==\n";
    // "\xEF\xBB\xBF" es el BOM que agrega el Bloc de notas; "\r\n" es el salto de linea de Windows.
    std::istringstream entrada(
        "\xEF\xBB\xBFMEMORIA_TOTAL 65536\r\n"
        "TAMANO_PAGINA 1024   # comentario al final\r\n"
        "\r\n"
        "   # una linea que solo es comentario\r\n"
        "TAMANO_CACHE_GLOBAL 8\r\n"
        "new_process P1 4096\r\n"
        "  Access   P1   10  \r\n");
    std::ostringstream salida;
    OperationParser parser(salida);
    verificar(parser.procesarFlujo(entrada), "el archivo se procesa completo");
    verificar(parser.getErrores() == 0, "no hay errores");
    verificar(parser.getOperaciones() == 2, "se ejecutan las 2 operaciones");
}

static void pruebaErroresDeLinea() {
    std::cout << "\n== Errores en las lineas (no detienen la simulacion) ==\n";
    std::istringstream entrada(CONFIG +
        "MOVE P1 0\n"               // operacion desconocida
        "NEW_PROCESS P1\n"          // falta un dato
        "NEW_PROCESS P1 12abc\n"    // no es numero
        "ACCESS P9 0\n"             // proceso inexistente
        "NEW_PROCESS P1 4096\n"     // esta si funciona
        "NEW_PROCESS P1 4096\n"     // duplicado
        "TAMANO_PAGINA 2048\n");    // configuracion repetida despues de empezar
    std::ostringstream salida;
    OperationParser parser(salida);
    verificar(parser.procesarFlujo(entrada), "la simulacion llega hasta el final");
    verificar(parser.getErrores() == 6 && parser.getOperaciones() == 1, "6 errores y 1 operacion exitosa");
    std::string texto = salida.str();
    verificar(contiene(texto, "Operacion desconocida: 'MOVE'"), "detecta 'Operacion desconocida'");
    verificar(contiene(texto, "el formato es 'NEW_PROCESS idProceso memoria'"), "detecta un dato faltante");
    verificar(contiene(texto, "'12abc' no es un numero entero valido"), "detecta un numero mal escrito");
    verificar(contiene(texto, "Proceso inexistente: P9"), "detecta 'Proceso inexistente'");
    verificar(contiene(texto, "Proceso duplicado: P1"), "detecta 'Proceso duplicado'");
    verificar(contiene(texto, "la configuracion ya esta definida"), "no deja cambiar la configuracion a mitad");
}

static void pruebaConfiguracionInvalida() {
    std::cout << "\n== Configuracion invalida (error fatal) ==\n";
    std::istringstream entrada(
        "MEMORIA_TOTAL 65536\nTAMANO_PAGINA 1000\nTAMANO_CACHE_GLOBAL 8\nNEW_PROCESS P1 4096\n");
    std::ostringstream salida;
    OperationParser parser(salida);
    verificar(!parser.procesarFlujo(entrada), "procesarFlujo devuelve false");
    verificar(contiene(salida.str(), "Tamano de pagina invalido"), "detecta 'Tamano de pagina invalido'");
    verificar(!parser.configuracionLista() && parser.getOperaciones() == 0, "no se ejecuta ninguna operacion");

    std::istringstream sinNumero("MEMORIA_TOTAL mucha\n");
    std::ostringstream salida2;
    OperationParser parser2(salida2);
    verificar(!parser2.procesarFlujo(sinNumero) && contiene(salida2.str(), "Archivo de entrada incorrecto"),
              "un valor de configuracion que no es numero es 'Archivo de entrada incorrecto'");

    std::istringstream repetida("MEMORIA_TOTAL 65536\nMEMORIA_TOTAL 1024\n");
    std::ostringstream salida3;
    OperationParser parser3(salida3);
    verificar(!parser3.procesarFlujo(repetida) && contiene(salida3.str(), "aparece dos veces"),
              "un parametro repetido detiene la simulacion");
}

static void pruebaFaltaConfiguracion() {
    std::cout << "\n== Falta la configuracion ==\n";
    std::istringstream entrada("MEMORIA_TOTAL 65536\nNEW_PROCESS P1 4096\n");
    std::ostringstream salida;
    OperationParser parser(salida);
    verificar(!parser.procesarFlujo(entrada), "un archivo sin configuracion completa falla");
    verificar(contiene(salida.str(), "falta definir TAMANO_PAGINA TAMANO_CACHE_GLOBAL"),
              "dice que parametros faltan");

    std::istringstream vacio("");
    std::ostringstream salida2;
    OperationParser parser2(salida2);
    verificar(!parser2.procesarFlujo(vacio) && contiene(salida2.str(), "Archivo de entrada incorrecto"),
              "un archivo vacio es 'Archivo de entrada incorrecto'");
}

static void pruebaArchivoInexistente() {
    std::cout << "\n== Archivo que no existe ==\n";
    std::ostringstream salida;
    OperationParser parser(salida);
    verificar(!parser.procesarArchivo("este_archivo_no_existe.txt"), "procesarArchivo devuelve false");
    verificar(contiene(salida.str(), "no se pudo abrir"), "informa que no se pudo abrir");
}

static void pruebaSalir() {
    std::cout << "\n== SALIR detiene la lectura ==\n";
    std::istringstream entrada(CONFIG + "NEW_PROCESS P1 4096\nSALIR\nNEW_PROCESS P2 4096\n");
    std::ostringstream salida;
    OperationParser parser(salida);
    parser.procesarFlujo(entrada);
    verificar(parser.getMemoria()->existeProceso("P1") && !parser.getMemoria()->existeProceso("P2"),
              "lo que esta despues de SALIR no se ejecuta");
}

static void pruebaConsola() {
    std::cout << "\n== Modo consola ==\n";
    // Lo que el usuario "escribe": primero un error, luego una configuracion
    // invalida, luego una valida, y despues operaciones.
    std::istringstream teclado(
        "abc\n"                          // no es numero: se vuelve a pedir
        "65536\n1000\n8\n"               // invalida: se piden las tres otra vez
        "65536\n1024\n8\n"               // valida
        "NEW_PROCESS P1 4096\n"
        "ACCESS P1 2500\n"
        "ACCESS P1 2500\n"
        "ACCESS P1 abc\n"
        "SALIR\n");
    std::ostringstream salida;
    OperationParser parser(salida);
    parser.procesarConsola(teclado);
    std::string texto = salida.str();
    verificar(contiene(texto, "Entrada incorrecta: escriba solo un numero entero"), "rechaza letras en la configuracion");
    verificar(contiene(texto, "Tamano de pagina invalido"), "rechaza una configuracion invalida y la vuelve a pedir");
    verificar(parser.configuracionLista(), "al final la configuracion queda lista");
    verificar(parser.getOperaciones() == 3, "ejecuta las 3 operaciones validas");
    verificar(contiene(texto, "HIT en cache local"), "el segundo acceso es HIT local");
    verificar(contiene(texto, "'abc' no es un numero entero valido"), "da el mismo error de formato que el modo archivo");
}

static void pruebaArbolYExportar() {
    std::cout << "\n== Extras: ARBOL y EXPORTAR ==\n";
    std::istringstream entrada(CONFIG +
        "NEW_PROCESS P1 4096\nACCESS P1 2500\n"
        "ARBOL AVL\nARBOL SPLAY P1\nARBOL SPLAY P9\nARBOL\n"
        "EXPORTAR metricas_prueba.csv\nEXPORTAR\n");
    std::ostringstream salida;
    OperationParser parser(salida);
    verificar(parser.procesarFlujo(entrada), "la simulacion llega hasta el final");
    verificar(parser.getOperaciones() == 2 && parser.getErrores() == 3,
              "ARBOL y EXPORTAR no cuentan como operaciones; hay 3 errores");
    std::string texto = salida.str();
    verificar(contiene(texto, "AVL de paginas fisicas libres"), "ARBOL AVL dibuja el arbol de libres");
    verificar(contiene(texto, "Splay Tree de P1"), "ARBOL SPLAY P1 dibuja la tabla del proceso");
    verificar(contiene(texto, "Proceso inexistente: P9"), "ARBOL SPLAY con proceso inexistente da error");
    verificar(contiene(texto, "el formato es 'ARBOL AVL'"), "ARBOL sin datos da error de formato");
    verificar(contiene(texto, "Metricas exportadas a metricas_prueba.csv"), "EXPORTAR confirma el archivo");
    verificar(contiene(texto, "el formato es 'EXPORTAR nombreArchivo.csv'"), "EXPORTAR sin archivo da error de formato");

    std::ifstream csv("metricas_prueba.csv");
    std::string contenidoCsv((std::istreambuf_iterator<char>(csv)), std::istreambuf_iterator<char>());
    verificar(contiene(contenidoCsv, "metrica,valor\n") && contiene(contenidoCsv, "accesos_totales,1")
              && contiene(contenidoCsv, "P1,4096,4,"),
              "el CSV tiene las metricas y una fila por proceso");
    csv.close();
    std::remove("metricas_prueba.csv");

    std::istringstream sinConfig("ARBOL AVL\n");
    std::ostringstream salida2;
    OperationParser parser2(salida2);
    verificar(!parser2.procesarFlujo(sinConfig) && contiene(salida2.str(), "antes de ARBOL falta definir"),
              "ARBOL antes de la configuracion da error");
}

static void pruebaReemplazo() {
    std::cout << "\n== Extra: PAGINAS_SWAP (reemplazo de paginas) ==\n";
    std::istringstream entrada(
        "MEMORIA_TOTAL 4096\nTAMANO_PAGINA 1024\nTAMANO_CACHE_GLOBAL 2\nPAGINAS_SWAP 4\n"
        "NEW_PROCESS P1 2048\nNEW_PROCESS P2 2048\nACCESS P1 0\nNEW_PROCESS P3 2048\n"
        "ACCESS P1 1024\nEND_PROCESS P2\nPAGINAS_SWAP 8\n");
    std::ostringstream salida;
    OperationParser parser(salida);
    verificar(parser.procesarFlujo(entrada) && parser.getErrores() == 1,
              "la simulacion termina; solo el PAGINAS_SWAP tardio es error");
    std::string texto = salida.str();
    verificar(contiene(texto, "Reemplazo de paginas activado: swap de 4 paginas"), "informa que el reemplazo esta activo");
    verificar(contiene(texto, "Reemplazo de pagina: LRU pasa P1:1 (pagina fisica 1) a swap"),
              "informa cada pagina que pasa a swap");
    verificar(contiene(texto, "Fallo de pagina: P1:1 estaba en swap")
              && contiene(texto, "P1:1 se cargo en la pagina fisica 3"), "informa el fallo de pagina");
    verificar(contiene(texto, "0 paginas fisicas vuelven al AVL de libres y 2 paginas salen de swap"),
              "END_PROCESS distingue paginas fisicas y paginas en swap");
    verificar(contiene(texto, "PAGINAS_SWAP aparece dos veces"),
              "un segundo PAGINAS_SWAP se rechaza");
    verificar(parser.getMemoria()->getMetricas().fallosPagina == 1
              && parser.getMemoria()->getCapacidadSwap() == 4, "las metricas y el swap son correctos");

    std::istringstream primero("PAGINAS_SWAP 2\n" + CONFIG);
    std::ostringstream salida2;
    OperationParser parser2(salida2);
    verificar(parser2.procesarFlujo(primero) && parser2.getMemoria()->getCapacidadSwap() == 2,
              "PAGINAS_SWAP puede ir antes que los demas parametros");

    std::istringstream errores(CONFIG + "PAGINAS_SWAP abc\nPAGINAS_SWAP -5\nPAGINAS_SWAP 3\nPAGINAS_SWAP 3\n");
    std::ostringstream salida3;
    OperationParser parser3(salida3);
    verificar(parser3.procesarFlujo(errores) && parser3.getErrores() == 3
              && parser3.getMemoria()->getCapacidadSwap() == 3,
              "letras, negativos y repetidos dan error, pero no detienen la simulacion");

    std::istringstream tarde(CONFIG + "NEW_PROCESS P1 1024\nPAGINAS_SWAP 4\n");
    std::ostringstream salida6;
    OperationParser parser6(salida6);
    verificar(parser6.procesarFlujo(tarde) && !parser6.getMemoria()->reemplazoActivo()
              && contiene(salida6.str(), "PAGINAS_SWAP debe ir antes de la primera operacion"),
              "PAGINAS_SWAP despues de la primera operacion se rechaza");

    std::istringstream sinSwap(CONFIG);
    std::ostringstream salida4;
    OperationParser parser4(salida4);
    verificar(parser4.procesarFlujo(sinSwap) && !parser4.getMemoria()->reemplazoActivo(),
              "sin PAGINAS_SWAP el reemplazo queda apagado");

    std::istringstream teclado("4096\n1024\n2\nPAGINAS_SWAP 4\nNEW_PROCESS P1 4096\n"
                               "NEW_PROCESS P2 1024\nSALIR\n");
    std::ostringstream salida5;
    OperationParser parser5(salida5);
    parser5.procesarConsola(teclado);
    verificar(parser5.getMemoria()->reemplazoActivo()
              && contiene(salida5.str(), "Reemplazo de pagina: LRU pasa P1:0"),
              "en modo consola tambien se activa escribiendo PAGINAS_SWAP");
}

int main() {
    std::cout << "=== PRUEBAS DEL LECTOR DE OPERACIONES ===\n";

    pruebaArchivoValido();
    pruebaComentariosYFormato();
    pruebaErroresDeLinea();
    pruebaConfiguracionInvalida();
    pruebaFaltaConfiguracion();
    pruebaArchivoInexistente();
    pruebaSalir();
    pruebaConsola();
    pruebaArbolYExportar();
    pruebaReemplazo();

    std::cout << "\nResultado: " << pasadas << " aprobadas, "
              << falladas << " falladas.\n";
    return (falladas == 0) ? 0 : 1;
}
