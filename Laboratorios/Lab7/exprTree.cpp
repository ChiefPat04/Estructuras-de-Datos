#include "exprTree.h"

// Definicion del mapa global declarado como extern en exprTree.h
std::map<char, double> variableValues;

//Helper functions for precedence and associativity
int precedence(char op){
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^' || op == '_') return 3; // potencia y raiz: mayor precedencia
    return 0;
}

bool isOperator(char c){
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '_';
}

bool isDigitOrDecimal(const std::string& str){
    //Check if a string is a valid number (including floating point)
    std::istringstream iss(str);
    double d;
    iss >> d;
    return !iss.fail();
}

//se chequea la expresion correcta antes de construir el arbol.
//Revisa parentesis balanceados
//que no hayan dos operandos seguidos ni un operador sin operando a la izq
//y que no queden caracteres invalidos.
bool validateExpression(const std::string& expression, std::string& errorMsg){
    if (expression.empty()){
        errorMsg = "La expresion esta vacia.";
        return false;
    }

    int balance = 0;
    bool prevWasOperand = false; //ultimo token visto fue numero, variable o 

    for (size_t i = 0; i < expression.size(); i++){
        char c = expression[i];

        if (std::isspace(c)) continue;

        if (c == '('){
            if (prevWasOperand) {errorMsg = "Falta un operador antes de '('."; return false;}
            balance++;
            prevWasOperand = false;
        }
        else if (c == ')') {
            balance--;
            if (balance < 0) { errorMsg = "Parentesis desbalanceados."; return false; }
            if (!prevWasOperand) { errorMsg = "Parentesis vacios o mal formados."; return false; }
            prevWasOperand = true;
        }
        else if (std::isdigit(c) || c == '.') {
            if (prevWasOperand) { errorMsg = "Falta un operador entre dos operandos."; return false; }
            while (i < expression.size() && (std::isdigit(expression[i]) || expression[i] == '.')) i++;
            i--; // el for hace i++ otra vez
            prevWasOperand = true;
        }
        else if (std::isupper(c)) {
            if (prevWasOperand) { errorMsg = "Falta un operador antes de la variable."; return false; }
            prevWasOperand = true;
        }
        else if (isOperator(c)) {
            bool isUnaryMinus = (c == '-' && !prevWasOperand);
            if (!isUnaryMinus && !prevWasOperand) {
                errorMsg = "Operador sin operando a la izquierda.";
                return false;
            }
            prevWasOperand = false;
        }
        else {
            errorMsg = std::string("Caracter no valido: '") + c + "'.";
            return false;
        }
    }
 
    if (balance != 0) { errorMsg = "Parentesis desbalanceados."; return false; }
    if (!prevWasOperand) { errorMsg = "La expresion termina en un operador."; return false; }
 
    return true;
}
 
// Punto 3/4 del PDF: recorre la hilera y devuelve, sin repetir, las
// letras mayusculas (variables) que aparecen, en el orden en que salen.
std::string collectVariableNames(const std::string& expression) {
    std::string names;
    for (char c : expression) {
        if (std::isupper(c) && names.find(c) == std::string::npos) {
            names += c;
        }
    }
    return names;
}
 
