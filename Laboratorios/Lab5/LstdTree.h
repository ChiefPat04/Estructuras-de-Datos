
#include <vector>
#include <iostream>
using namespace std;

//Cambios documentados en este encabezado:
// El borrado de un valor se vuelve recursivo para poder localizar el nodo dentro de ramas inferiores 
// y borrar la referencia correcta en el vector de hijos del nodo padre que corresponda.  

template<class T> 
class lstdTreeNode
{
private:
    T t;
    // Antes: vector<lstdTreeNode> children;
    // Se corrige a vector<lstdTreeNode<T>> children; porque la clase es
    // una plantilla y el vector de hijos debe almacenar el mismo tipo
    // instanciado con T, no el nodo genérico no parametrizado.
    vector<lstdTreeNode<T>> children;

public:
    lstdTreeNode()
    {
    }

    lstdTreeNode(const T &t)
    {
        this->t = t;
    }

    virtual ~lstdTreeNode()
    {
    }

    void addChild(const T &t)
    {
        this->children.push_back(lstdTreeNode(t));
    }

    // Antes:
    // remove a child by value, note: if the node has multiple children with the
    // same value, this will only delete the first child
    // void removeChild(const T &t)
    // {
    //     for (int i = 0; i < this->children.size(); ++i)
    //     {
    //         if (this->children.at(i).t == t)
    //         {
    //             this->children.erase(this->children.begin() + i);
    //             return;
    //         }
    //     }
    // }
    //
    // Cambio necesario: el borrado por valor no debe quedarse solo en el nivel
    // inmediato del nodo. Si se quiere cumplir el comportamiento esperado en un
    // árbol, hay que buscar también en niveles inferiores de forma recursiva.
    // Esto evita que un valor sólo se encuentre en subárboles y quede sin poder
    // eliminarse desde el nodo raíz o desde un nodo intermedio.
    void removeChild(const T &t)
    {
        for (int i = 0; i < this->children.size(); ++i)
        {
            if (this->children.at(i).t == t)
            {
                this->children.erase(this->children.begin() + i);
                return;
            }
        }

        for (int i = 0; i < this->children.size(); ++i)
        {
            this->children.at(i).removeChild(t);
        }
    }

    void removeChildByIndex(const int index)
    {
        this->children.erase(this->children.begin() + index);
    }

    void setValue(const T &t)
    {
        this->t = t;
    }

    T &getValue()
    {
        return this->t;
    }

    const T &getValue() const
    {
        return this->t;
    }

    vector<lstdTreeNode<T>> &getChildren()
    {
        return this->children;
    }

    const vector<lstdTreeNode<T>> &getChildren() const
    {
        return this->children;
    }

    // Nuevo metodo solicitado: devuelve true si el valor existe en cualquier
    // nodo del subarbol actual, incluido el propio nodo.
    bool searchChild(const T &t) const
    {
        if (this->t == t)
            return true;

        for (int i = 0; i < this->children.size(); ++i)
        {
            if (this->children.at(i).searchChild(t))
                return true;
        }

        return false;
    }

    // Nuevo metodo solicitado: devuelve la profundidad del valor en el subarbol
    // actual, contando la raiz como profundidad 0. Si no existe, devuelve -1.
    int getDepthChild(const T &t) const
    {
        return getDepthChildRecursive(t, 0);
    }

private:
    int getDepthChildRecursive(const T &t, const int depth) const
    {
        if (this->t == t)
            return depth;

        for (int i = 0; i < this->children.size(); ++i)
        {
            const int foundDepth = this->children.at(i).getDepthChildRecursive(t, depth + 1);
            if (foundDepth != -1)
                return foundDepth;
        }

        return -1;
    }

public:

    // the type has to have an overloaded std::ostream << operator for print to work
    void print(const int depth = 0) const
    {
        for (int i = 0; i < depth; ++i)
        {
            if (i != depth - 1)
                cout << "    ";
            else
                cout << "|-- ";
        }
        cout << this->t << endl;
        for (int i = 0; i < this->children.size(); ++i)
        {
            this->children.at(i).print(depth + 1);
        }
    }
};



//La salida de prueba es 1,2,0,0.
//Se interpreta como que el nodo raíz tiene valor 1, el primer hijo tiene valor 2, y los hijos de este primer hijo tienen valores 4 y 5. 
//La búsqueda del valor 4 devuelve true (1), la profundidad del valor 4 es 2, 
//la búsqueda del valor 99 devuelve false (0) ya que no existe en el árbol, y después de eliminar el valor 4, 
//la búsqueda de este valor nuevamente devuelve false (0).