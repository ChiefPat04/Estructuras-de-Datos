// ============================================================
// Page.cpp  -  implementacion de la clase Page
// ============================================================

#include "Page.h"

Page::Page()
    : paginaVirtual_(-1), paginaFisica_(-1), idProceso_(""),
      valida_(false), modificada_(false),
      contadorReferencias_(0), ultimaReferencia_(0) {}

Page::Page(int paginaVirtual, int paginaFisica, const std::string& idProceso)
    : paginaVirtual_(paginaVirtual), paginaFisica_(paginaFisica),
      idProceso_(idProceso), valida_(!idProceso.empty()),
      modificada_(false), contadorReferencias_(0), ultimaReferencia_(0) {}

void Page::registrarReferencia(long long reloj) {
    contadorReferencias_++;
    ultimaReferencia_ = reloj;
}
