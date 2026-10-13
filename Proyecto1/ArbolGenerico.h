// ArbolGenerico.h - árbol genérico propio (cada nodo puede tener varios hijos)
// Jerarquía de un curso: Curso -> Unidad -> Tema -> Recurso/Actividad.
// Cada nodo guarda una std::list con sus hijos y un puntero a su padre.
// Los nombres pueden repetirse, por eso cada nodo tiene un id único.
// Niveles: la raíz es el nivel 1 y la altura es la cantidad de niveles.

#ifndef ARBOL_GENERICO_H
#define ARBOL_GENERICO_H

#include <list>
#include <ostream>
#include <string>
#include <vector>

enum class TipoNodo { Curso, Unidad, Tema, Recurso, Actividad };

std::string nombreTipo(TipoNodo tipo);

// Devuelve false si el texto no es un tipo válido.
bool tipoDesdeTexto(const std::string& texto, TipoNodo& tipo);

// Solo ArbolGenerico (friend) puede crear nodos.
class NodoCurso {
    friend class ArbolGenerico;

private:
    int id_;
    TipoNodo tipo_;
    std::string nombre_;
    NodoCurso* padre_;              // nullptr si es la raíz
    std::list<NodoCurso*> hijos_;
    int visitas_;

    NodoCurso(int id, TipoNodo tipo, const std::string& nombre, NodoCurso* padre);

    NodoCurso(const NodoCurso&) = delete;
    NodoCurso& operator=(const NodoCurso&) = delete;

public:
    int getId() const { return id_; }
    TipoNodo getTipo() const { return tipo_; }
    const std::string& getNombre() const { return nombre_; }
    NodoCurso* getPadre() const { return padre_; }
    const std::list<NodoCurso*>& getHijos() const { return hijos_; }
    bool esHoja() const { return hijos_.empty(); }

    int getVisitas() const { return visitas_; }
    void registrarVisita() { visitas_++; }
    void setVisitas(int visitas) { visitas_ = visitas; }   // para cargar datos
    bool deshacerVisita();          // false si ya estaba en 0
};

// El árbol de un curso; la raíz es el curso mismo.
class ArbolGenerico {
private:
    NodoCurso* raiz_;
    int cantidad_;

    static void destruirRec(NodoCurso* nodo);
    static NodoCurso* buscarIdRec(NodoCurso* nodo, int id);
    static void preordenRec(NodoCurso* nodo, std::vector<NodoCurso*>& salida);
    static int alturaRec(const NodoCurso* nodo);
    static int contarSubarbol(const NodoCurso* nodo);
    static void mostrarRec(std::ostream& salida, const NodoCurso* nodo, int nivel,
                           const std::string& numero, int posicion);   // posicion 0 = raíz

    bool perteneceAlArbol(const NodoCurso* nodo) const;

public:
    ArbolGenerico(int idRaiz, const std::string& nombreCurso);
    ~ArbolGenerico();

    ArbolGenerico(const ArbolGenerico&) = delete;
    ArbolGenerico& operator=(const ArbolGenerico&) = delete;

    NodoCurso* getRaiz() const { return raiz_; }
    int cantidadNodos() const { return cantidad_; }

    // Devuelve el nodo nuevo, o nullptr si el padre no es de este árbol,
    // el nombre está vacío o el tipo es Curso.
    NodoCurso* agregarHijo(NodoCurso* padre, int id, TipoNodo tipo,
                           const std::string& nombre);

    // Solo elimina hojas que no sean la raíz.
    bool eliminarHoja(NodoCurso* nodo);

    // Imprime el árbol con 4 espacios de indentación por nivel y numeración
    // por posición: 1, 1.1, 1.1.1 ...
    // numeroCurso es la posición del curso en la lista de cursos (0 = no mostrarla).
    void mostrar(std::ostream& salida, int numeroCurso = 0) const;

    NodoCurso* buscarPorId(int id) const;
    std::vector<NodoCurso*> buscarPorNombre(const std::string& fragmento) const;

    // Posición entre sus hermanos (1 = primero). 0 para la raíz.
    int posicion(const NodoCurso* nodo) const;

    // Numeración por posición: "1", "1.2", "1.2.1". Vacío para la raíz.
    std::string numeracion(const NodoCurso* nodo) const;

    // Lo contrario: "1.2.1" -> nodo. "curso" devuelve la raíz. nullptr si no existe.
    NodoCurso* buscarPorNumeracion(const std::string& texto) const;

    int nivel(const NodoCurso* nodo) const;
    int contarDescendientes(const NodoCurso* nodo) const;   // sin contar el nodo
    int altura() const;
    std::vector<NodoCurso*> hojas() const;

    // Camino de la raíz al nodo, y como texto: "Curso > Unidad > Tema".
    std::vector<const NodoCurso*> ruta(const NodoCurso* nodo) const;
    std::string rutaComoTexto(const NodoCurso* nodo,
                              const std::string& separador = " > ") const;

    // Camino de un nodo a otro: sube hasta el ancestro común y luego baja.
    // Vacío si alguno de los nodos no es de este árbol.
    std::vector<const NodoCurso*> rutaEntre(const NodoCurso* origen,
                                            const NodoCurso* destino) const;

    // Padres antes que hijos; sirve para guardar en archivo.
    std::vector<NodoCurso*> preorden() const;
};

#endif
