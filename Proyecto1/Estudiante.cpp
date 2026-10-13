// ============================================================
// Estudiante.cpp  -  implementación de la clase Estudiante
// ============================================================

#include "Estudiante.h"
#include <iostream>

// "Estudiante::" significa "esta función pertenece a la clase Estudiante".

// Constructor por defecto. Lo que va después de ":" es la LISTA DE
// INICIALIZACIÓN: da el valor inicial a cada atributo.
Estudiante::Estudiante() : carne_(0), nombre_(""), correo_("") {}

Estudiante::Estudiante(long long carne, const std::string& nombre,
                       const std::string& correo)
    : carne_(carne), nombre_(nombre), correo_(correo) {}

long long Estudiante::getCarne() const { return carne_; }
std::string Estudiante::getNombre() const { return nombre_; }
std::string Estudiante::getCorreo() const { return correo_; }

void Estudiante::setCarne(long long carne) { carne_ = carne; }
void Estudiante::setNombre(const std::string& nombre) { nombre_ = nombre; }
void Estudiante::setCorreo(const std::string& correo) { correo_ = correo; }

void Estudiante::mostrar() const {
    std::cout << "Carne: " << carne_
              << " | Nombre: " << nombre_
              << " | Correo: " << correo_ << "\n";
}