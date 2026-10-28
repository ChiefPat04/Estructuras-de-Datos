// Page.h - una pagina de memoria.
// Guarda a que pagina virtual de un proceso corresponde (paginaVirtual),
// en que marco fisico esta (paginaFisica) y cuanto se ha usado.
// Las paginas libres tambien usan esta clase, con idProceso vacio.

#ifndef PAGE_H
#define PAGE_H

#include <string>

class Page {
private:
    int paginaVirtual_;
    int paginaFisica_;
    std::string idProceso_;
    bool valida_;                 // true si esta asignada a un proceso
    bool modificada_;
    int contadorReferencias_;     // cuantas veces se ha accedido
    long long ultimaReferencia_;  // "reloj" de la simulacion en el ultimo acceso

public:
    Page();
    Page(int paginaVirtual, int paginaFisica, const std::string& idProceso);

    int getPaginaVirtual() const { return paginaVirtual_; }
    int getPaginaFisica() const { return paginaFisica_; }
    const std::string& getIdProceso() const { return idProceso_; }
    bool esValida() const { return valida_; }
    bool estaModificada() const { return modificada_; }
    int getContadorReferencias() const { return contadorReferencias_; }
    long long getUltimaReferencia() const { return ultimaReferencia_; }

    // Se llama cada vez que la pagina es accedida.
    void registrarReferencia(long long reloj);
    void marcarModificada() { modificada_ = true; }
};

#endif
