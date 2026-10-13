// Utils.cpp - implementación de Utils.h

#include "Utils.h"

#include <iostream>
#include <sstream>
#include <cstdlib>

namespace Utils {

std::string quitarEspacios(const std::string& texto) {
    const std::string invisibles = " \t\n\r";   // '\r' lo agrega Windows en archivos
    std::size_t inicio = texto.find_first_not_of(invisibles);
    if (inicio == std::string::npos) {
        return "";
    }
    std::size_t fin = texto.find_last_not_of(invisibles);
    return texto.substr(inicio, fin - inicio + 1);
}

// Se lee la línea completa con getline para que un "abc" no dañe el cin.
long long leerEntero(const std::string& mensaje,
                     long long minimo, long long maximo) {
    while (true) {
        std::cout << mensaje;
        std::string linea;
        if (!std::getline(std::cin, linea)) {
            std::cout << "\nEntrada cerrada. Saliendo del programa.\n";
            std::exit(0);
        }
        linea = quitarEspacios(linea);

        std::istringstream flujo(linea);
        long long valor;
        char sobrante;
        // Debe leerse un número y no sobrar nada (evita "12abc").
        if (flujo >> valor && !(flujo >> sobrante)) {
            if (valor >= minimo && valor <= maximo) {
                return valor;
            }
            std::cout << "  Error: el numero debe estar entre "
                      << minimo << " y " << maximo << ".\n";
        } else {
            std::cout << "  Error: debe escribir un numero entero.\n";
        }
    }
}

std::string leerTexto(const std::string& mensaje) {
    while (true) {
        std::cout << mensaje;
        std::string linea;
        if (!std::getline(std::cin, linea)) {
            std::cout << "\nEntrada cerrada. Saliendo del programa.\n";
            std::exit(0);
        }
        linea = quitarEspacios(linea);
        if (!linea.empty()) {
            return linea;
        }
        std::cout << "  Error: el texto no puede estar vacio.\n";
    }
}

// Formato: algo@estudiantec.cr (sin otro '@' ni espacios antes del dominio).
bool esCorreoValido(const std::string& correo) {
    const std::string dominio = "@estudiantec.cr";

    if (correo.size() <= dominio.size()) {
        return false;
    }
    std::size_t posicionDominio = correo.size() - dominio.size();
    if (correo.compare(posicionDominio, dominio.size(), dominio) != 0) {
        return false;
    }
    for (std::size_t i = 0; i < posicionDominio; i++) {
        char c = correo[i];
        if (c == '@' || c == ' ' || c == '\t') {
            return false;
        }
    }
    return true;
}

std::string leerCorreo(const std::string& mensaje) {
    while (true) {
        std::string correo = leerTexto(mensaje);
        if (esCorreoValido(correo)) {
            return correo;
        }
        std::cout << "  Error: el correo debe tener el formato "
                     "nombre@estudiantec.cr\n";
    }
}

// Solo convierte letras ASCII; las tildes y la ñ quedan igual.
std::string aMinusculas(const std::string& texto) {
    std::string resultado = texto;
    for (std::size_t i = 0; i < resultado.size(); i++) {
        char c = resultado[i];
        if (c >= 'A' && c <= 'Z') {
            resultado[i] = static_cast<char>(c - 'A' + 'a');
        }
    }
    return resultado;
}

bool contieneTexto(const std::string& texto, const std::string& fragmento) {
    return aMinusculas(texto).find(aMinusculas(fragmento)) != std::string::npos;
}

bool confirmar(const std::string& pregunta) {
    while (true) {
        std::string respuesta = aMinusculas(leerTexto(pregunta + " (s/n): "));
        if (respuesta == "s" || respuesta == "si") return true;
        if (respuesta == "n" || respuesta == "no") return false;
        std::cout << "  Error: responda s o n.\n";
    }
}

std::vector<std::string> dividir(const std::string& texto, char separador) {
    std::vector<std::string> pedazos;
    std::string actual;
    for (std::size_t i = 0; i < texto.size(); i++) {
        if (texto[i] == separador) {
            pedazos.push_back(actual);
            actual = "";
        } else {
            actual += texto[i];
        }
    }
    pedazos.push_back(actual);
    return pedazos;
}

bool aEntero(const std::string& texto, long long& valor) {
    std::istringstream flujo(quitarEspacios(texto));
    char sobrante;
    return (flujo >> valor) && !(flujo >> sobrante);
}

void pausar() {
    std::cout << "\nPresione ENTER para continuar...";
    std::string basura;
    std::getline(std::cin, basura);
}

}
