#ifndef NODO_H // Evita que se incluya más de una vez
#define NODO_H // Definición de la clase Nodo

template <typename T>
class Nodo {
public:
	// Atributos
	T dato; // Dato que almacena el nodo
	Nodo<T> * siguiente;

	// Constructores
	Nodo(){
		dato = T();
		siguiente = NULL;
	}

	Nodo(T _dato){
		dato = _dato;
		siguiente = NULL;
	}

	Nodo(T _dato, Nodo<T> * _siguiente){
		dato = _dato;
		siguiente = _siguiente;
	}
};

#endif