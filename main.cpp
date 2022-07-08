/*
Calculator program
Includes the following facilities:
1. Expression calculator
2. Number system conversion
(More features coming soon)
*/

#include <iostream>
#include <iomanip>
#include <map>

#include "Token.h"
#include "numsystem.h"
#include "utils.h"
// #include "matrix.h"
#include "functions.h"


using namespace std;

Token_stream ts;
map<string,double> variables;
map<string,double> constants;

int main() {
    constants["pi"] = 3.14159265359;
    constants["e"] = 2.71828;
    cout << "WELCOME" << "\n";

    // calculate
    calculate();

    return 0;
}