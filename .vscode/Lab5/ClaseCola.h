#include <iostream>
using namespace std;

template <class T, int nE1 = 100>
class Cola
{
    T datos[nE1];
    int nElementos;
    int rear, front;

public:
    // constructor
    Cola() : nElementos(0),rear(-1),front(-1){};
    // añade un elemento a la cola
    void enqueue(T elem);
    // obtener y borrar un elemento del rear de la cola
    T dequeue();
    // obtener el elemento del rear de la cola
    T peek();
    // obtener el numero de elementos
    int num_elementos();
    //¿está la cola vacía?
    bool Vacia();
    // ¿está la cola llena?
    bool Llena();
};




template <class T, int nE1>
void Cola<T, nE1>::enqueue(T elem)
{
    if (nElementos < nE1)
    {
        if (front <= rear) {
            rear = nElementos;
            datos[rear] = elem;
            if (front == -1)
                front++;
            nElementos++;
        }

    }
    else
        cout << " cola llena ";
}

template <class T, int nE1>
T Cola<T, nE1>::dequeue()
{
    if (nElementos > 0)
    {
        int front_ant = front;
        front++;
        nElementos--;
        if (nElementos == 0) {
            front = -1;
            rear = -1;
            
        }

        return datos[front_ant];
    }
    else 
        cout << " error esta vacia ";
     
    return T();
}

template <class T, int nE1>
T Cola<T, nE1>::peek()
{
    if (nElementos > 0)
    {
        return datos[front];
    }
    else
        cout << " error esta vacia ";
    return T();
}

template <class T, int nE1>
int Cola<T, nE1>::num_elementos()
{
    return nElementos;
}

template <class T, int nE1>
bool Cola<T, nE1>::Vacia()
{
    return (nElementos == 0);
}

template <class T, int nE1>
bool Cola<T, nE1>::Llena()
{
    return (nElementos == nE1);
}
