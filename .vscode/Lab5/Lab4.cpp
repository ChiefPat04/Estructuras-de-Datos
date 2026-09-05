#include <iostream>
#include <cmath>
#include "ClaseCola.h"
using namespace std;

// Clase Termino: guarda el coeficiente y el exponente de un termino del polinomio
class Termino
{
public:
    double coeficiente;
    int exponente;

    Termino() : coeficiente(0.0), exponente(0) {} 
    Termino(double coef, int exp) : coeficiente(coef), exponente(exp) {} 
};

// Lee los coeficientes y potencias del polinomio y los guarda en una cola
Cola<Termino> leerPolinomio(int grado) 
{
    Cola<Termino> polinomio;
    double coef;

    for (int k = 0; k <= grado; k++) 
    {
        cout << "Ingrese el coeficiente a" << k << ": ";
        cin >> coef;
        // el exponente de a_k es k
        Termino t(coef, k); // crea un objeto Termino con el coeficiente y el exponente
        polinomio.enqueue(t); // añade el termino a la cola
    }
    return polinomio;
}

// Evalua el polinomio contenido en la cola para el valor x dado
double evalua(Cola<Termino> polinomio, double x)
{
    double resultado = 0.0; 

    while (!polinomio.Vacia()) 
    {
        Termino t = polinomio.dequeue(); // obtiene el siguiente termino de la cola
        resultado += t.coeficiente * pow(x, t.exponente); // calcula el valor del termino y lo suma al resultado
    }
    return resultado;
}

int main()
{
    int grado; 
    double x;

    cout << "=== Evaluador de Polinomios usando Cola ===" << endl; 
    cout << "Ingrese el grado del polinomio (-1 para salir): ";
    cin >> grado; 

    while (grado != -1) 
    {
        if (grado < -1)
        {
            cout << "El grado debe ser -1 o un entero no negativo." << endl;
            cout << "Ingrese el grado del polinomio (-1 para salir): ";
            cin >> grado;
            continue;
        }

        if (grado >= 100)
        {
            cout << "El grado maximo permitido es 99." << endl;
            cout << "Ingrese el grado del polinomio (-1 para salir): ";
            cin >> grado;
            continue;
        }

        Cola<Termino> polinomio = leerPolinomio(grado); // lee los coeficientes y potencias del polinomio y los guarda en una cola

        cout << "Ingrese el valor de x: "; 
        cin >> x;

        double resultado = evalua(polinomio, x); // evalua el polinomio para el valor x dado

        cout << "p(" << x << ") = " << resultado << endl; 
        cout << endl; 

        cout << "Ingrese el grado del polinomio (-1 para salir): ";
        cin >> grado;
    }

    cout << "Fin del programa." << endl;
    return 0;
}