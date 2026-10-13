// Menus.cpp - submenús de estudiantes y cursos
// Las funciones "static" solo se usan dentro de este archivo.

#include "Menus.h"

#include <iostream>
#include <iterator>
#include <list>
#include <string>
#include <vector>
#include "Utils.h"

// Carné máximo: 10 dígitos
static const long long CARNE_MAXIMO = 9999999999LL;

static long long pedirCarne(const std::string& mensaje) {
    return Utils::leerEntero(mensaje, 1, CARNE_MAXIMO);
}

// Imprime una lista de estudiantes, o avisa si no hay ninguno.
static void imprimirEstudiantes(const std::vector<const Estudiante*>& estudiantes) {
    if (estudiantes.empty()) {
        std::cout << "No se encontró ningún estudiante.\n";
        return;
    }
    for (std::size_t i = 0; i < estudiantes.size(); i++) {
        std::cout << "  ";
        estudiantes[i]->mostrar();
    }
}

// 1. Registrar
static void registrarEstudiante(GestorEstudiantes& gestor) {
    std::cout << "\n--- Registrar estudiante ---\n";
    long long carne = pedirCarne("Carné: ");
    std::string nombre = Utils::leerTexto("Nombre completo: ");
    std::string correo = Utils::leerCorreo("Correo institucional: ");

    std::string error;
    if (gestor.registrar(carne, nombre, correo, error)) {
        std::cout << "Estudiante registrado correctamente.\n";
    } else {
        std::cout << "No se pudo registrar: " << error << "\n";
    }
}

// 2. Modificar
static void modificarEstudiante(GestorEstudiantes& gestor) {
    std::cout << "\n--- Modificar estudiante ---\n";
    long long carne = pedirCarne("Carné del estudiante a modificar: ");

    const Estudiante* actual = gestor.buscarPorCarne(carne);
    if (actual == nullptr) {
        std::cout << "No existe ningún estudiante con ese carné.\n";
        return;
    }
    std::cout << "Estudiante encontrado:\n  ";
    actual->mostrar();

    std::cout << "\n1. Cambiar nombre\n2. Cambiar correo\n3. Cambiar carné\n0. Cancelar\n";
    long long opcion = Utils::leerEntero("Opción: ", 0, 3);

    std::string error;
    bool exito = false;
    if (opcion == 1) {
        exito = gestor.cambiarNombre(carne, Utils::leerTexto("Nuevo nombre: "), error);
    } else if (opcion == 2) {
        exito = gestor.cambiarCorreo(carne, Utils::leerCorreo("Nuevo correo: "), error);
    } else if (opcion == 3) {
        exito = gestor.cambiarCarne(carne, pedirCarne("Nuevo carné: "), error);
    } else {
        std::cout << "Operación cancelada.\n";
        return;
    }

    if (exito) {
        std::cout << "Cambio realizado correctamente.\n";
    } else {
        std::cout << "No se pudo modificar: " << error << "\n";
    }
}

// 3. Eliminar
static void eliminarEstudiante(GestorEstudiantes& gestor) {
    std::cout << "\n--- Eliminar estudiante ---\n";
    long long carne = pedirCarne("Carné del estudiante a eliminar: ");

    const Estudiante* e = gestor.buscarPorCarne(carne);
    if (e == nullptr) {
        std::cout << "No existe ningún estudiante con ese carné.\n";
        return;
    }
    std::cout << "Se eliminará:\n  ";
    e->mostrar();

    if (Utils::confirmar("¿Seguro que desea eliminarlo?")) {
        gestor.eliminar(carne);
        std::cout << "Estudiante eliminado.\n";
    } else {
        std::cout << "Operación cancelada.\n";
    }
}

// 4. Mostrar todos (recorre la lista)
static void mostrarTodos(const GestorEstudiantes& gestor) {
    std::cout << "\n--- Todos los estudiantes (orden de registro, desde la lista) ---\n";
    if (gestor.vacio()) {
        std::cout << "No hay estudiantes registrados.\n";
        return;
    }
    int numero = 1;
    for (const Estudiante& e : gestor.todos()) {
        std::cout << numero << ". ";
        e.mostrar();
        numero++;
    }
    std::cout << "Total: " << gestor.cantidad() << "\n";
}

// 5. Mostrar ordenados por carné (inorden del ABB)
static void mostrarOrdenadosPorCarne(const GestorEstudiantes& gestor) {
    std::cout << "\n--- Estudiantes ordenados por carné (inorden del ABB) ---\n";
    imprimirEstudiantes(gestor.inorden());
}

// 6, 7 y 8. Búsquedas
static void buscarPorCarne(const GestorEstudiantes& gestor) {
    std::cout << "\n--- Buscar por carné (búsqueda en el ABB) ---\n";
    long long carne = pedirCarne("Carné: ");
    const Estudiante* e = gestor.buscarPorCarne(carne);
    if (e == nullptr) {
        std::cout << "No existe ningún estudiante con ese carné.\n";
    } else {
        std::cout << "Encontrado:\n  ";
        e->mostrar();
    }
}

static void buscarPorNombre(const GestorEstudiantes& gestor) {
    std::cout << "\n--- Buscar por nombre (recorre la lista) ---\n";
    std::string texto = Utils::leerTexto("Nombre o parte del nombre: ");
    imprimirEstudiantes(gestor.buscarPorNombre(texto));
}

static void buscarPorCorreo(const GestorEstudiantes& gestor) {
    std::cout << "\n--- Buscar por correo (recorre la lista) ---\n";
    std::string correo = Utils::leerTexto("Correo completo: ");
    const Estudiante* e = gestor.buscarPorCorreo(correo);
    if (e == nullptr) {
        std::cout << "No existe ningún estudiante con ese correo.\n";
    } else {
        std::cout << "Encontrado:\n  ";
        e->mostrar();
    }
}

void menuEstudiantes(GestorEstudiantes& gestor) {
    while (true) {
        std::cout << "\n===== MANEJO DE ESTUDIANTES =====\n"
                  << "1. Registrar estudiante\n"
                  << "2. Modificar estudiante\n"
                  << "3. Eliminar estudiante\n"
                  << "4. Mostrar todos (orden de registro)\n"
                  << "5. Mostrar ordenados por carné (ABB)\n"
                  << "6. Buscar por carné\n"
                  << "7. Buscar por nombre\n"
                  << "8. Buscar por correo\n"
                  << "0. Volver al menú principal\n";
        long long opcion = Utils::leerEntero("Opción: ", 0, 8);

        switch (opcion) {
            case 1: registrarEstudiante(gestor); break;
            case 2: modificarEstudiante(gestor); break;
            case 3: eliminarEstudiante(gestor); break;
            case 4: mostrarTodos(gestor); break;
            case 5: mostrarOrdenadosPorCarne(gestor); break;
            case 6: buscarPorCarne(gestor); break;
            case 7: buscarPorNombre(gestor); break;
            case 8: buscarPorCorreo(gestor); break;
            default: return;   // 0 = volver
        }
        Utils::pausar();
    }
}

// ----- Ayudas compartidas -----

// Pide un carné y devuelve el estudiante, o nullptr si no existe.
Estudiante* pedirEstudiante(GestorEstudiantes& estudiantes) {
    if (estudiantes.vacio()) {
        std::cout << "No hay estudiantes registrados.\n";
        return nullptr;
    }
    Estudiante* e = estudiantes.buscarPorCarne(pedirCarne("Carné del estudiante: "));
    if (e == nullptr) {
        std::cout << "No existe ningún estudiante con ese carné.\n";
    }
    return e;
}

// ----- Cursos y contenidos (árbol genérico) -----

// Deja elegir un curso. Devuelve nullptr si no hay cursos o se cancela.
ArbolGenerico* elegirCurso(const GestorCursos& gestor) {
    const std::list<ArbolGenerico*>& cursos = gestor.cursos();
    if (cursos.empty()) {
        std::cout << "No hay cursos registrados. Cree uno con la opción 1 "
                     "o cargue el de ejemplo con la opción 10.\n";
        return nullptr;
    }
    if (cursos.size() == 1) {
        ArbolGenerico* unico = cursos.front();
        std::cout << "Curso: " << unico->getRaiz()->getNombre() << "\n";
        return unico;
    }
    std::cout << "Cursos disponibles:\n";
    int numero = 1;
    for (ArbolGenerico* curso : cursos) {
        std::cout << "  " << numero << ". " << curso->getRaiz()->getNombre() << "\n";
        numero++;
    }
    long long opcion = Utils::leerEntero("Número de curso (0 para cancelar): ", 0,
                                         static_cast<long long>(cursos.size()));
    if (opcion == 0) return nullptr;

    std::list<ArbolGenerico*>::const_iterator it = cursos.begin();
    std::advance(it, opcion - 1);
    return *it;
}

// Pide el número de un nodo ("1.2.1"); "c" es el curso y "0" cancela.
// Devuelve nullptr si cancela o no existe.
NodoCurso* pedirNodo(const ArbolGenerico* arbol, const std::string& mensaje) {
    std::string texto = Utils::leerTexto(mensaje + " (ej. 1.2.1; c = el curso; 0 = cancelar): ");
    if (texto == "0") return nullptr;
    if (Utils::aMinusculas(texto) == "c") return arbol->getRaiz();
    NodoCurso* nodo = arbol->buscarPorNumeracion(texto);
    if (nodo == nullptr) {
        std::cout << "No existe el nodo \"" << texto << "\" en este curso.\n";
    }
    return nodo;
}

// Elige un curso, lo muestra y pide un nodo de ese curso.
NodoCurso* elegirNodo(const GestorCursos& cursos, const std::string& mensaje) {
    ArbolGenerico* arbol = elegirCurso(cursos);
    if (arbol == nullptr) return nullptr;
    std::cout << "\n";
    arbol->mostrar(std::cout, cursos.posicionCurso(arbol));
    std::cout << "\n";
    return pedirNodo(arbol, mensaje);
}

// 1. Crear curso
static void crearCurso(GestorCursos& gestor) {
    std::cout << "\n--- Crear curso ---\n";
    std::string nombre = Utils::leerTexto("Nombre del curso: ");
    std::string error;
    ArbolGenerico* curso = gestor.crearCurso(nombre, error);
    if (curso != nullptr) {
        std::cout << "Curso creado.\n";
    } else {
        std::cout << "No se pudo crear: " << error << "\n";
    }
}

// 2. Agregar nodo hijo a cualquier nodo
static void agregarNodo(GestorCursos& gestor) {
    std::cout << "\n--- Agregar nodo hijo ---\n";
    ArbolGenerico* arbol = elegirCurso(gestor);
    if (arbol == nullptr) return;

    std::cout << "\n";
    arbol->mostrar(std::cout, gestor.posicionCurso(arbol));
    std::cout << "\n";

    NodoCurso* padre = arbol->getRaiz();
    if (arbol->cantidadNodos() == 1) {
        std::cout << "El curso está vacío: el nodo se agregará directamente en el curso.\n";
    } else {
        padre = pedirNodo(arbol, "Número del nodo al que se agregará el hijo");
        if (padre == nullptr) return;
    }

    std::cout << "Tipo del nuevo nodo:\n"
              << "  1. Unidad\n  2. Tema\n  3. Recurso\n  4. Actividad\n";
    long long tipoOpcion = Utils::leerEntero("Tipo: ", 1, 4);
    TipoNodo tipo = TipoNodo::Unidad;
    if (tipoOpcion == 2) tipo = TipoNodo::Tema;
    if (tipoOpcion == 3) tipo = TipoNodo::Recurso;
    if (tipoOpcion == 4) tipo = TipoNodo::Actividad;

    std::string nombre = Utils::leerTexto("Nombre: ");
    std::string error;
    NodoCurso* nuevo = gestor.agregarHijo(padre->getId(), tipo, nombre, error);
    if (nuevo != nullptr) {
        std::cout << "Nodo agregado como " << arbol->numeracion(nuevo) << " dentro de \""
                  << padre->getNombre() << "\".\n";
    } else {
        std::cout << "No se pudo agregar: " << error << "\n";
    }
}

// 3. Eliminar nodo hoja
static void eliminarNodo(GestorCursos& gestor) {
    std::cout << "\n--- Eliminar nodo hoja ---\n";
    ArbolGenerico* arbol = elegirCurso(gestor);
    if (arbol == nullptr) return;

    std::cout << "\n";
    arbol->mostrar(std::cout, gestor.posicionCurso(arbol));
    std::cout << "\n";
    NodoCurso* nodo = pedirNodo(arbol, "Número del nodo a eliminar");
    if (nodo == nullptr) return;

    if (!nodo->esHoja()) {
        std::cout << "\"" << nodo->getNombre() << "\" tiene "
                  << arbol->contarDescendientes(nodo)
                  << " descendientes. Solo se pueden eliminar nodos hoja.\n";
        return;
    }
    bool esCurso = (nodo == arbol->getRaiz());
    std::string pregunta = esCurso
        ? "¿Eliminar el curso \"" + nodo->getNombre() + "\"?"
        : "¿Eliminar \"" + nodo->getNombre() + "\"?";
    if (!Utils::confirmar(pregunta)) {
        std::cout << "Operación cancelada.\n";
        return;
    }
    std::string error;
    int id = nodo->getId();   // se guarda porque el nodo se destruye
    if (gestor.eliminarNodo(id, error)) {
        std::cout << "Eliminado.\n";
    } else {
        std::cout << "No se pudo eliminar: " << error << "\n";
    }
}

// 4. Mostrar el árbol de un curso (con indentación)
static void mostrarArbol(GestorCursos& gestor) {
    std::cout << "\n--- Árbol de un curso ---\n";
    ArbolGenerico* arbol = elegirCurso(gestor);
    if (arbol == nullptr) return;
    std::cout << "\n";
    arbol->mostrar(std::cout, gestor.posicionCurso(arbol));
}

// 5. Mostrar todos los cursos
static void mostrarTodosLosCursos(const GestorCursos& gestor) {
    std::cout << "\n--- Todos los cursos ---\n";
    if (gestor.vacio()) {
        std::cout << "No hay cursos registrados.\n";
        return;
    }
    for (const ArbolGenerico* curso : gestor.cursos()) {
        std::cout << "\n";
        curso->mostrar(std::cout, gestor.posicionCurso(curso));
    }
    std::cout << "\nCursos: " << gestor.cantidadCursos()
              << " | Nodos en total: " << gestor.totalNodos() << "\n";
}

// La raíz no tiene numeración, se muestra como "curso".
static std::string numeroDe(const ArbolGenerico* arbol, const NodoCurso* nodo) {
    return (nodo == arbol->getRaiz()) ? "curso" : arbol->numeracion(nodo);
}

// 6. Buscar nodos por nombre (en todos los cursos), mostrando su ruta
static void buscarNodo(GestorCursos& gestor) {
    std::cout << "\n--- Buscar nodo por nombre ---\n";
    std::string texto = Utils::leerTexto("Nombre o parte del nombre: ");
    std::vector<NodoCurso*> encontrados = gestor.buscarPorNombre(texto);
    if (encontrados.empty()) {
        std::cout << "No se encontró ningún nodo.\n";
        return;
    }
    std::cout << encontrados.size() << " resultado(s):\n";
    for (const NodoCurso* nodo : encontrados) {
        const ArbolGenerico* arbol = gestor.cursoDeNodo(nodo->getId());
        std::cout << "  [" << nombreTipo(nodo->getTipo()) << " " << numeroDe(arbol, nodo) << "] "
                  << nodo->getNombre() << "\n"
                  << "      nivel " << arbol->nivel(nodo) << " | ruta: "
                  << arbol->rutaComoTexto(nodo) << "\n";
    }
}

// 7. Información de un nodo: nivel, descendientes, ruta
static void informacionNodo(GestorCursos& gestor) {
    std::cout << "\n--- Información de un nodo ---\n";
    ArbolGenerico* arbol = elegirCurso(gestor);
    if (arbol == nullptr) return;

    std::cout << "\n";
    arbol->mostrar(std::cout, gestor.posicionCurso(arbol));
    std::cout << "\n";
    NodoCurso* nodo = pedirNodo(arbol, "Número del nodo");
    if (nodo == nullptr) return;

    std::cout << "\nNodo: " << nodo->getNombre() << "\n"
              << "  Tipo:            " << nombreTipo(nodo->getTipo()) << "\n"
              << "  Número:          " << numeroDe(arbol, nodo) << "\n"
              << "  Nivel:           " << arbol->nivel(nodo) << "  (la raíz es el nivel 1)\n"
              << "  Hijos directos:  " << nodo->getHijos().size() << "\n"
              << "  Descendientes:   " << arbol->contarDescendientes(nodo) << "\n"
              << "  ¿Es hoja?:       " << (nodo->esHoja() ? "sí" : "no") << "\n"
              << "  Ruta desde la raíz:\n      " << arbol->rutaComoTexto(nodo) << "\n";
}

// 8. Estadísticas del árbol de un curso
static void estadisticasCurso(GestorCursos& gestor) {
    std::cout << "\n--- Altura y tamaño de un curso ---\n";
    ArbolGenerico* arbol = elegirCurso(gestor);
    if (arbol == nullptr) return;

    std::cout << "\nCurso: " << arbol->getRaiz()->getNombre() << "\n"
              << "  Cantidad de nodos: " << arbol->cantidadNodos() << "\n"
              << "  Altura del árbol:  " << arbol->altura() << " niveles\n"
              << "  Nodos hoja:        " << arbol->hojas().size() << "\n";
}

// 9. Mostrar los nodos hoja de un curso
static void mostrarHojas(GestorCursos& gestor) {
    std::cout << "\n--- Nodos hoja de un curso ---\n";
    ArbolGenerico* arbol = elegirCurso(gestor);
    if (arbol == nullptr) return;

    std::vector<NodoCurso*> hojas = arbol->hojas();
    std::cout << "\n" << hojas.size() << " nodo(s) hoja:\n";
    for (const NodoCurso* hoja : hojas) {
        std::cout << "  [" << nombreTipo(hoja->getTipo()) << " " << arbol->numeracion(hoja) << "] "
                  << hoja->getNombre() << "\n"
                  << "      ruta: " << arbol->rutaComoTexto(hoja) << "\n";
    }
}

// 11. Ruta entre dos nodos del mismo curso
static void rutaEntreNodos(GestorCursos& gestor) {
    std::cout << "\n--- Ruta entre dos nodos ---\n";
    ArbolGenerico* arbol = elegirCurso(gestor);
    if (arbol == nullptr) return;
    std::cout << "\n";
    arbol->mostrar(std::cout, gestor.posicionCurso(arbol));
    std::cout << "\n";
    NodoCurso* origen = pedirNodo(arbol, "Número del nodo de origen");
    if (origen == nullptr) return;
    NodoCurso* destino = pedirNodo(arbol, "Número del nodo de destino");
    if (destino == nullptr) return;

    std::vector<const NodoCurso*> camino = arbol->rutaEntre(origen, destino);
    std::cout << "\nRuta de \"" << origen->getNombre() << "\" a \""
              << destino->getNombre() << "\":\n";
    for (std::size_t i = 0; i < camino.size(); i++) {
        std::cout << "  " << (i + 1) << ". [" << nombreTipo(camino[i]->getTipo());
        if (camino[i] != arbol->getRaiz()) {
            std::cout << " " << arbol->numeracion(camino[i]);
        }
        std::cout << "] " << camino[i]->getNombre() << "\n";
    }
    std::cout << "Pasos: " << camino.size() - 1 << "\n";
}

// 10. Curso de ejemplo para probar sin escribir todo
static void cargarCursoEjemplo(GestorCursos& gestor) {
    std::cout << "\n--- Cargar curso de ejemplo ---\n";
    std::string error;
    ArbolGenerico* curso = gestor.crearCurso("Estructuras de Datos", error);
    if (curso == nullptr) {
        std::cout << "No se pudo cargar el ejemplo: " << error << "\n";
        return;
    }
    int raiz = curso->getRaiz()->getId();
    NodoCurso* u1 = gestor.agregarHijo(raiz, TipoNodo::Unidad, "Fundamentos", error);
    NodoCurso* t11 = gestor.agregarHijo(u1->getId(), TipoNodo::Tema, "Tipos abstractos", error);
    gestor.agregarHijo(t11->getId(), TipoNodo::Recurso, "PDF de teoría", error);
    gestor.agregarHijo(t11->getId(), TipoNodo::Recurso, "Video introductorio", error);
    NodoCurso* t12 = gestor.agregarHijo(u1->getId(), TipoNodo::Tema, "Complejidad", error);
    gestor.agregarHijo(t12->getId(), TipoNodo::Actividad, "Quiz", error);
    NodoCurso* u2 = gestor.agregarHijo(raiz, TipoNodo::Unidad, "Estructuras lineales", error);
    gestor.agregarHijo(u2->getId(), TipoNodo::Tema, "Listas", error);
    gestor.agregarHijo(u2->getId(), TipoNodo::Tema, "Pilas", error);
    gestor.agregarHijo(u2->getId(), TipoNodo::Tema, "Colas", error);

    std::cout << "Curso de ejemplo cargado:\n\n";
    curso->mostrar(std::cout, gestor.posicionCurso(curso));
}

void menuCursos(GestorCursos& gestor) {
    while (true) {
        std::cout << "\n===== CURSOS Y CONTENIDOS =====\n"
                  << "1. Crear curso\n"
                  << "2. Agregar nodo hijo\n"
                  << "3. Eliminar nodo hoja\n"
                  << "4. Mostrar árbol de un curso\n"
                  << "5. Mostrar todos los cursos\n"
                  << "6. Buscar nodo por nombre\n"
                  << "7. Información de un nodo (nivel, descendientes, ruta)\n"
                  << "8. Altura y cantidad de nodos de un curso\n"
                  << "9. Mostrar nodos hoja de un curso\n"
                  << "10. Cargar curso de ejemplo\n"
                  << "11. Ruta entre dos nodos\n"
                  << "0. Volver al menú principal\n";
        long long opcion = Utils::leerEntero("Opción: ", 0, 11);

        switch (opcion) {
            case 1: crearCurso(gestor); break;
            case 2: agregarNodo(gestor); break;
            case 3: eliminarNodo(gestor); break;
            case 4: mostrarArbol(gestor); break;
            case 5: mostrarTodosLosCursos(gestor); break;
            case 6: buscarNodo(gestor); break;
            case 7: informacionNodo(gestor); break;
            case 8: estadisticasCurso(gestor); break;
            case 9: mostrarHojas(gestor); break;
            case 10: cargarCursoEjemplo(gestor); break;
            case 11: rutaEntreNodos(gestor); break;
            default: return;   // 0 = volver
        }
        Utils::pausar();
    }
}
