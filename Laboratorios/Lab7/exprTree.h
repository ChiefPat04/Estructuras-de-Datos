
#ifndef EXPRETREE_H
#define EXPRETREE_H


#include <memory>
#include <string>
#include <sstream>
#include <cctype>
#include <stack>
#include <map>
#include <stdexcept>
#include <cmath>
// https://www.youtube.com/watch?v=FNBNaldmuv0
template<typename T>
class ExprNode {
public:
    virtual ~ExprNode() = default;
    virtual T evaluate() const = 0;
    virtual std::string toString() const = 0;
    virtual std::string toInfix() const = 0;
    virtual std::string toPrefix() const = 0;
    virtual std::string toPostfix() const = 0;
};

template<typename T>
class ValueNode : public ExprNode<T> {
    T value;
public:
    ValueNode(T val) : value(val) {}
    T evaluate() const override {
        return value;
    }
    std::string toString() const override {
        return std::to_string(value);
    }
    std::string toInfix() const override {
        return std::to_string(value);
    }
    std::string toPrefix() const override {
        return std::to_string(value);
    }
    std::string toPostfix() const override {
        return std::to_string(value);
    }
};

// Mapa global con los valores que el usuario asigna a las variables A-Z
// antes de evaluar. Se llena en main() y VariableNode simplemente lo consulta, 
//por eso evaluate() no necesita cambiar de firma en ninguna otra clase.

extern std::map<char, double> variableValues;

template<typename T>
class VariableNode : public ExprNode<T> {
    char name;
public:
    VariableNode(char n) : name(n) {}
    T evaluate() const override {
        auto it = variableValues.find(name);
        if (it == variableValues.end())
            throw std::runtime_error("Valor no asignado para la variable" + std::string(1, name));
            return static_cast<T>(it->second);
    }
    std::string toString() const override {
        return std::string(1,name);
    }
    std::string toInfix() const override {
        return std::string(1,name);
    }
    std::string toPrefix() const override {
        return std::string(1,name);
    }
    std::string toPostfix() const override{
        return std::string(1,name);
    }
};

template<typename T>
class OpNode : public ExprNode<T> {
    char op;
    std::shared_ptr<ExprNode<T>> left;
    std::shared_ptr<ExprNode<T>> right;
public:
    OpNode(char oper, std::shared_ptr<ExprNode<T>> leftNode, std::shared_ptr<ExprNode<T>> rightNode = nullptr)
        : op(oper), left(leftNode), right(rightNode) {}

    T evaluate() const override {
        if (op == '+') return left->evaluate() + right->evaluate();
        if (op == '-') return left->evaluate() - right->evaluate();
        if (op == '*') return left->evaluate() * right->evaluate();
        if (op == '/') return left->evaluate() / right->evaluate();
        if (op == '^') return static_cast<T>(std::pow(left->evaluate(), right->evaluate()));
        if (op == '_') {// izq _ der  ->  raiz izq-esima de der (ej. 3_4 = raiz cubica de 4)
            T index = left ->evaluate();
            if (index == 0) throw std::runtime_error("Indice de raiz invalido");
            return static_cast<T>(std::pow(right->evaluate(), 1.0 / index));
        }
            throw std::runtime_error("Unknown operator");
    }

    std::string toString() const override {
        if (op == '-' && !right) // Unary minus
            return "-" + left->toString();
        return "(" + left->toString() + (right ? (" " + std::string(1, op) + " " + right->toString()) : "") + ")";
    }

    std::string toInfix() const override {
        if (op == '-' && !right)
            return "-" + left->toInfix();
        return "(" + left->toInfix() + (right ? (" " + std::string(1,op) + " " + right->toInfix()) : "") + ")";
    }

    std::string toPrefix() const override {
        return (right ? std::string(1, op) + " " : "") + left->toPrefix() + (right ? " " + right->toPrefix() : "");
    }

    std::string toPostfix() const override {
        return left->toPostfix() + (right ? " " + right->toPostfix() + " " : "") + std::string(1, op);
    }
};

template<typename T>
class ExpressionTree {
    std::shared_ptr<ExprNode<T>> root;
public:
    ExpressionTree(std::shared_ptr<ExprNode<T>> rootNode) : root(rootNode) {}

    T evaluate() const {
        return root->evaluate();
    }

    std::string toString() const {
        return root->toString();
    }

    std::string toInfix() const {
        return root->toInfix();
    }

    std::string toPrefix() const {
        return root->toPrefix();
    }

    std::string toPostfix() const {
        return root->toPostfix();
    }
};

// Helper functions for precedence and associativity
int precedence(char op);
bool isOperator(char c);
bool isDigitOrDecimal(const std::string& str);

// valida la expresion antes de construir el arbol.
bool validateExpression(const std::string& expression, std::string& errorMsg);

//se juntan las variables (A-Z) presentes en la expresion.
std::string collectVariableNames(const std::string& expression);

// Function to parse the expression and create an expression tree
template<typename T>
std::shared_ptr<ExprNode<T>> parseExpression(const std::string &expression) {
    std::stack<std::shared_ptr<ExprNode<T>>> values;
    std::stack<char> ops;

    size_t i = 0;
    while (i < expression.size()) {
        char token = expression[i];

        // Handle unary minus (preceding a number or opening parenthesis)
        if (token == '-' && (i == 0 || expression[i - 1] == '(' || isOperator(expression[i -   1]))) {
            i++;
            // Parse the number following the unary minus
            size_t start = i;
            while (i < expression.size() && (std::isdigit(expression[i]) || expression[i] == '.')) {
                i++;
            }
            std::string number = expression.substr(start, i - start);
            values.push(std::make_shared<ValueNode<T>>(-std::stod(number)));
            continue;
        }

        // Handle numbers (including floating-point numbers)
        else if (std::isdigit(token) || (token == '.' && i + 1 < expression.size() && std::isdigit(expression[i + 1]))) {
            size_t start = i;
            while (i < expression.size() && (std::isdigit(expression[i]) || expression[i] == '.')) {
                i++;
            }
            std::string number = expression.substr(start, i - start);
            values.push(std::make_shared<ValueNode<T>>(std::stod(number)));
            continue;
        }
        // Handle variables (uppercase letters A-Z)
        else if (std::isupper(token)){
            values.push(std::make_shared<VariableNode<T>>(token));
        }
        else if (token == '(') {
            ops.push(token);
        }
        else if (token == ')') {
            // Process until '(' is encountered
            while (!ops.empty() && ops.top() != '(') {
                char op = ops.top();
                ops.pop();

                auto right = values.top();
                values.pop();
                auto left = values.top();
                values.pop();

                values.push(std::make_shared<OpNode<T>>(op, left, right));
            }
            ops.pop(); // Pop the '('
        }
        else if (isOperator(token)) {
            // Handle operators
            while (!ops.empty() && precedence(ops.top()) >= precedence(token)) {
                char op = ops.top();
                ops.pop();

                auto right = values.top();
                values.pop();
                auto left = values.top();
                values.pop();

                values.push(std::make_shared<OpNode<T>>(op, left, right));
            }
            ops.push(token);
        }
        i++;
    }

    // Process remaining operators
    while (!ops.empty() && !values.empty()) {
        char op = ops.top();
        ops.pop();

        auto right = values.top();
        values.pop();

        if (!values.empty()) {
            auto left = values.top();
            values.pop();

            values.push(std::make_shared<OpNode<T>>(op, left, right));
        }
    }
    if (!values.empty())
        return values.top();
    else
        return NULL;
}

#endif