// Estudiante.h - datos de un estudiante

#ifndef ESTUDIANTE_H
#define ESTUDIANTE_H

#include <string>
#include "HistorialAcademico.h"

class Estudiante {
private:
    long long carne_;
    std::string nombre_;
    std::string correo_;
    HistorialAcademico historial_;

public:
    Estudiante();
    Estudiante(long long carne, const std::string& nombre,
               const std::string& correo);

    long long getCarne() const;
    std::string getNombre() const;
    std::string getCorreo() const;

    void setCarne(long long carne);
    void setNombre(const std::string& nombre);
    void setCorreo(const std::string& correo);

    HistorialAcademico& getHistorial() { return historial_; }
    const HistorialAcademico& getHistorial() const { return historial_; }

    void mostrar() const;
};

#endif
