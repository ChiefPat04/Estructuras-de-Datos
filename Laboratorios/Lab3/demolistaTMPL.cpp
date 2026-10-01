#include <iostream>
#include "listasimple.h"

template <typename T> 
void ListaSimple<T>::insertarAlInicio(T _dato){ // Inserta un nuevo nodo al inicio de la lista
	if (inicio == NULL)
		inicio = new Nodo<T>(_dato); // Si la lista está vacía, se crea un nuevo nodo y se asigna a inicio
	else {
		Nodo<T> * nuevo = new Nodo<T>(_dato); // Se crea un nuevo nodo con el dato proporcionado
		nuevo->siguiente = inicio;
		inicio = nuevo;
	}   
}

template <typename T>
void ListaSimple<T>::insertarAlFinal(T _dato){ // Inserta un nuevo nodo al final de la lista
	if (inicio == NULL)
		inicio = new Nodo<T>(_dato); 
	else {
		Nodo<T> * tmp = inicio; // Se crea un puntero temporal que apunta al inicio de la lista
		while (tmp->siguiente != NULL){
			tmp = tmp->siguiente;
		}
		tmp->siguiente = new Nodo<T>(_dato);// Se crea un nuevo nodo al final de la lista
	}
}

template <typename T>
Nodo<T> * ListaSimple<T>::eliminar(T _dato){ // Elimina un nodo de la lista que contiene el dato proporcionado
	if (inicio == NULL)
		return NULL;

	if (_dato == inicio->dato) { // Si el dato a eliminar es el del primer nodo, se elimina el primer nodo
		Nodo<T> * eliminado = inicio;
		inicio = inicio->siguiente;
		eliminado->siguiente = NULL;// Se desconecta el nodo eliminado de la lista
		return eliminado;
	}
	else {
		Nodo<T> * nodo_anterior = inicio; // Se crea un puntero que apunta al nodo anterior al nodo que se desea eliminar
		Nodo<T> * tmp = inicio;// Se crea un puntero temporal que apunta al inicio de la lista
		while (tmp != NULL){
			if (tmp->dato == _dato){ 
				nodo_anterior->siguiente = tmp->siguiente;
				tmp->siguiente = NULL;
				return tmp;
			}
			nodo_anterior = tmp;
			tmp = tmp->siguiente;
		}
		return NULL;
	}
}

template <typename T>
Nodo<T> * ListaSimple<T>::buscar(T _dato){ // Busca un nodo en la lista que contiene el dato proporcionado
	Nodo<T> * tmp = inicio;
	while (tmp != NULL){// Se recorre la lista hasta encontrar el nodo que contiene el dato proporcionado
		if (tmp->dato == _dato)
			return tmp;
		tmp = tmp->siguiente;
	}
	return NULL;
}

template <typename T>
bool ListaSimple<T>::estaVacia(){// Verifica si la lista está vacía 
	return inicio == NULL;
}

template <typename T>
void ListaSimple<T>::imprimir(){// Imprime los datos de todos los nodos de la lista
	if (!estaVacia()) {// Si la lista no está vacía, se recorre la lista y se imprimen los datos de cada nodo
		Nodo<T> * tmp = inicio;
		while (tmp != NULL){
			std::cout << tmp->dato << " ";
			tmp = tmp->siguiente;
		}
		std::cout << std::endl;// Se imprime un salto de línea al final de la lista
	}
	else
		std::cout << "No hay elementos" << std::endl;
}

int main(){
	ListaSimple<double> lista;// Se crea una lista simple de tipo double
	double valor;

	std::cout << "Ingrese 5 valores double para agregar a la lista:\n";
	for (int indice = 0; indice < 5; ++indice) {
		std::cin >> valor;
		if (indice % 2 == 0)
			lista.insertarAlInicio(valor);
		else
			lista.insertarAlFinal(valor);
	}

	std::cout << "Lista original: ";
	lista.imprimir();

	std::cout << "Ingrese 2 valores para eliminar:\n";
	for (int indice = 0; indice < 2; ++indice) {
		std::cin >> valor;
		Nodo<double> * eliminado = lista.eliminar(valor);
		if (eliminado != NULL) {// Si se eliminó un nodo, se imprime el valor eliminado y se libera la memoria del nodo eliminado
			std::cout << "Eliminado: " << eliminado->dato << std::endl;
			delete eliminado;// Se libera la memoria del nodo eliminado
		} else {
			std::cout << "El valor no esta en la lista: " << valor << std::endl; // Si no se encontró el nodo a eliminar, se imprime un mensaje indicando que el valor no está en la lista
		}
	}

	std::cout << "Lista despues de eliminar: ";
	lista.imprimir();

	std::cout << "Ingrese 2 valores para buscar:\n";
	for (int indice = 0; indice < 2; ++indice) {
		std::cin >> valor;
		std::cout << valor << (lista.buscar(valor) != NULL 
			? " esta en la lista."  //Usando operadores ternarios para imprimir si el valor se encuentra en la lista o no
			: " no esta en la lista.") << std::endl; 
	}

	return 0;
}