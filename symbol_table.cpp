#include "symbol_table.h"

symbolTable::symbolTable()
    : variables{}, constants{}
{
}

double symbolTable::get(std::string name)
{
    if (this->isDeclared(name))
        return variables[name];
    else
        throw std::runtime_error(name + "is not declared");
}

double symbolTable::getConst(std::string name)
{
    if (this->isConstDeclared(name))
        return constants[name];
    else
        throw std::runtime_error("No constant with name " + name + " exists");
}

double symbolTable::set(std::string name, double value)
{
    if (this->isDeclared(name))
        throw std::runtime_error("variable \'" + name + "\' already declared");
    else
        declare(name, value);
}

double symbolTable::declare(std::string name, double val, std::string str = "")
{
    double d;
    if (str == "undefined")
        variables[name] = d;
    else
        variables[name] = val;

    return d;
}
double declareConst(std::string name, double val);
