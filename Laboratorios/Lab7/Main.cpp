#include <iostream>
#include "exprTree.h"

int main() {
    std::string expression;

    // Punto 5 del PDF: repetir hasta que el usuario decida salir.
    while (true) {
        std::cout << "Enter a mathematical expression (e.g., (3+4)*5), o 'salir' para terminar: ";
        std::getline(std::cin, expression);

        if (expression == "salir") break;

        // Punto 2 del PDF: validar antes de construir el arbol.
        std::string errorMsg;
        if (!validateExpression(expression, errorMsg)) {
            std::cout << "Invalid expression: " << errorMsg << std::endl;
            continue;
        }

        auto root = parseExpression<double>(expression);
        if (root) {
            ExpressionTree<double> exprTree(root);

            std::cout << "Infix Expression: " << exprTree.toInfix() << std::endl;
            std::cout << "Prefix Expression: " << exprTree.toPrefix() << std::endl;
            std::cout << "Postfix Expression: " << exprTree.toPostfix() << std::endl;

            // Puntos 3/4 del PDF: si hay variables, pedir sus valores antes de evaluar.
            std::string varNames = collectVariableNames(expression);
            for (char v : varNames) {
                std::cout << "Ingrese el valor de " << v << ": ";
                double val;
                std::cin >> val;
                std::cin.ignore();
                variableValues[v] = val;
            }

            try {
                std::cout << "Result: " << exprTree.evaluate() << std::endl;
            } catch (const std::exception& e) {
                std::cout << "Error al evaluar: " << e.what() << std::endl;
            }
        }
        else
            std::cout << "Invalid expression" << std::endl;
    }
    return 0;
}