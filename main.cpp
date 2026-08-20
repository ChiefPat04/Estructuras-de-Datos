#include <iostream>

int sumar(int primerNumero, int segundoNumero) {
	return primerNumero + segundoNumero;
}

int main() {
	int resultado = sumar(2, 3);
	std::cout << "2 + 3 = " << resultado << '\n';

	return resultado == 5 ? 0 : 1;
}

