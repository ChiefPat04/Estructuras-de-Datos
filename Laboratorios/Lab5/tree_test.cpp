#include "LstdTree.h"
#include <iostream>
using namespace std;

int main(){
    cout << "Creamos un arbol con raiz 1 y dos hijos: 2 y 3" << endl;
    lstdTreeNode<int> root(1);
    root.addChild(2);
    root.addChild(3);
    root.getChildren().at(0).addChild(4);
    root.getChildren().at(0).addChild(5);

    cout << "Imprimimos el arbol inicial" << endl;
    root.print();

    cout << "Buscamos el valor 4 en el arbol" << endl;
    cout << "Resultado: " << (root.searchChild(4) ? "true" : "false") << endl;

    cout << "Calculamos la profundidad del valor 4" << endl;
    cout << "Resultado: " << root.getDepthChild(4) << endl;

    cout << "Buscamos el valor 99, que no existe" << endl;
    cout << "Resultado: " << (root.searchChild(99) ? "true" : "false") << endl;

    cout << "Borramos el valor 4 del arbol" << endl;
    root.removeChild(4);

    cout << "Volvemos a buscar 4 para verificar que ya no existe" << endl;
    cout << "Resultado: " << (root.searchChild(4) ? "true" : "false") << endl;

    return 0;
}
