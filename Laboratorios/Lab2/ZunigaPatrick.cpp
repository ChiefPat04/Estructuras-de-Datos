#include <iostream>
using namespace std;

//Problema 1
int digitosAux(int num) {
    if (num < 10) { //caso base
        return num;
    }
    return (num % 10) + digitosAux(num / 10); //
} //recursiva que suma digitos

int sumadigitos(int num) {
    if (num < 0) { //num natural
        cout << "Error: el numero debe ser natural" << endl;
        return -1;
    }
    return digitosAux(num);
}

//Problema 2
int sumatoriaAux(int i, int m, int res) {
    res = res + i; //suma el valor de i a res
    if (i >= m) { //caso base
        return res;
    } else {
        return sumatoriaAux(i + 1, m, res); //recursiva
    }
}

int sumatoria(int n, int m) {
    if (n > m) { //
        cout << "Error: el limite inferior no puede ser mayor que el superior" << endl;
        return -1;
    }
    return sumatoriaAux(n, m, 0);
}

//Problema 3
int apaux(int digito, int numero, int res) {
    int digitoActual = numero % 10; //obtiene el ultimo digito del numero
    if (digitoActual == digito) { //compara 
        res = res + 1; //incrementa el contador
    }
    numero = numero / 10; //elimina el ultimo digito del numero
    if (numero == 0) { //caso base
        return res; 
    } else {
        return apaux(digito, numero, res); //recursiva
    }
}

int apariciones(int digito, int numero) {
    if (digito < 0 || digito > 9) { //verifica
        cout << "Error: el digito debe estar entre 0 y 9" << endl;
        return -1;
    }
    if (numero < 0) {
        numero = -numero; //convierte a positivo
    }
    return apaux(digito, numero, 0);
}

int main() {
    cout << "Resultado: " << sumadigitos(123)
        << ", con los digitos (1, 2, 3)" << endl;
    cout << "Resultado: " << sumatoria(0, 5)
        << ", con los parametros (0, 5)" << endl;
    cout << "Resultado: " << apariciones(4, 34214)
        << ", con los parametros (4, 34214)" << endl;
    return 0;
}

/*
*/