// Utils.h - funciones auxiliares para leer y validar datos

#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

namespace Utils {

    // Quita espacios, tabuladores y saltos de línea al inicio y al final.
    // Ejemplo: "  hola  " -> "hola"
    std::string quitarEspacios(const std::string& texto);

    // Muestra un mensaje y pide un número entero entre minimo y maximo.
    // Repite la pregunta hasta que el usuario escriba algo válido.
    long long leerEntero(const std::string& mensaje,
                         long long minimo, long long maximo);

    // Muestra un mensaje y pide un texto que NO esté vacío.
    std::string leerTexto(const std::string& mensaje);

    // Dice si un correo cumple el formato  nombre@estudiantec.cr
    bool esCorreoValido(const std::string& correo);

    // Pide un correo hasta que sea válido.
    std::string leerCorreo(const std::string& mensaje);

    // Convierte las letras A-Z a minúsculas. Las tildes y la ñ no se tocan.
    // Ejemplo: "Ana PEREZ" -> "ana perez"
    std::string aMinusculas(const std::string& texto);

    // Dice si 'texto' contiene 'fragmento', sin distinguir mayúsculas.
    // Ejemplo: contieneTexto("Mariana Soto", "ANA") -> true
    bool contieneTexto(const std::string& texto, const std::string& fragmento);

    // Hace una pregunta de sí o no. Devuelve true si responde "s" o "si".
    bool confirmar(const std::string& pregunta);

    // Parte un texto en pedazos. Ejemplo: dividir("a|b|c", '|') -> {"a", "b", "c"}
    std::vector<std::string> dividir(const std::string& texto, char separador);

    // Convierte un texto a número. Devuelve false si no es un entero válido.
    bool aEntero(const std::string& texto, long long& valor);

    // Espera a que el usuario presione ENTER (para que lea la pantalla).
    void pausar();

}

#endif
