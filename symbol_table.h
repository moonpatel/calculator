#ifndef _SYMBOL_TABLE_H_
#define _SYMBOL_TABLE_H_

#include "utils.h"

// class that allows to declare and modiff all the variables and constants available
// for a particular instance of the program
class symbolTable {
private:
    std::map<std::string, double> variables;        // stores variables
    std::map<std::string, double> constants;        // stores constants

public:
    // constructor
    symbolTable();

    // getters and setters
    double get(std::string name);
    double getConst(std::string name);
    double set(std::string name, double value);

    // declare functions
    double declare(std::string name, double val, std::string str="");
    double declareConst(std::string name, double val);

    // checkers
    bool isDeclared(std::string name);
    bool isConstDeclared(std::string name);
};

#endif


