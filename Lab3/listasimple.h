#include "nodo.h" 

template <typename T> // Clase ListaSimple
class ListaSimple { // Definición de la clase ListaSimple
	
private:
	// Atributos
	Nodo<T> * inicio; // Puntero al primer nodo de la lista

public:
	// Constructores
	ListaSimple(){
		inicio = NULL;
	}

	// Metodos
	void insertarAlInicio(T _dato); 
	void insertarAlFinal(T _dato); 
	Nodo<T> * buscar(T _dato); 
	Nodo<T> * eliminar(T _dato); 
	bool estaVacia();  
	void imprimir(); 
};
