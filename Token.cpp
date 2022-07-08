/*
==================GRAMMAR FOR EXPRESSION CALCULATOR===============
Expression:
    Term
    Term + Expression
    Term - Expression

Term:
    Primary
    Primary * Term
    Primary / Term

Primary:
    -Number
    Number
    constants like pi, e and so on
    '('Expression')'
    -'('Expression')'

Number:
    Floating point literal
    Integer
    results of functions like log(a,b), rt(a)

Input comes from cin through the Token_stream called ts
*/

#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>

#include "Token.h"
#include "utils.h"

//=====================================================================================
// Token class member functions
//=====================================================================================

// Token::Token(char kind_val, double value_val, std::string name_val = std::string{})
//     : kind{kind_val}, value{value_val}, name{name_val} {}
Token::Token(char kind_val, double value_val)
    : kind{kind_val}, value{value_val}, name{} {}
Token::Token(char kind_val, std::string name_val = std::string{})
    : kind{kind_val}, value{0}, name{name_val} {}

Token::Token()
    : Token{0, 0} {}

// =====================================================================================
// Token_stream class member functions
// =====================================================================================

// Default constructor
Token_stream::Token_stream()
    : buffer{}, isFull{false} {}

// Get a token from the input stream
// Actually behind the scenes what happens is get()
// checks the buffer Token defined in Token_stream class and returns
// the buffer Token if it is not empty else it reads a Token from the standard input
Token Token_stream::get()
{
    if (isFull)
    {
        Token temp = buffer.front();
        buffer.pop();             // clear the buffer queue
        isFull = !buffer.empty(); // set isFull to false indicating buffer queue is now empty
        return temp;              // return a copy of element removec from queue
    }

    // if the buffer is empty
    char ch = 0;
    std::cin >> ch;

    switch (ch)
    {
    case '+':
    case '-':
    case '*':
    case '/':
    case ';':
    case 'q':
    case '(':
    case ')':
    case '=':
        return Token{ch, 0}; // return a Token(kind,value)

    // the token is a number
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    {
        std::cin.unget(); // the character was not an operator
        double d;
        std::cin >> d; // read a double
        return Token{'~', d};
    }

    default:
        if (isalpha(ch) || ch == '_')
        {
            std::string str;
            str += ch;
            while (std::cin.get(ch) && (isalpha(ch) || isdigit(ch)))
                str += ch;
            std::cin.putback(ch);
            if (str == declaration_keyword)
                return Token{LET};
            else if (is_defined(str, constants))
                return Token{CONSTANT, str};
            return Token{NAME, str};
        }
        throw std::runtime_error("Error in Token_stream::get()"); // some bad token obtained in input stream
    }
}

// putback a character in the input stream
// The character is stored in the buffer member of the Token_stream class
void Token_stream::unget(Token token)
{
    Token t;
    t.kind = token.kind;
    t.value = token.value;
    t.name = token.name;
    buffer.push(t); // store the token in buffer queue
    isFull = !buffer.empty();
    return;
}

// clear buffer
void Token_stream::flush()
{
    isFull = false;
    while (!buffer.empty())
        buffer.pop();
}

// handle the expression calculation part
void calculate()
{
    std::cout << "Expression calculator program" << std::endl;
    std::cout << "Please enter the expression you want to calculate below" << std::endl;
    std::cout << "(NOTE: Enter q to QUIT and ; after the end of an expression)" << std::endl;
    printline();

    while (true)
    {
        try
        {
            std::cout << PROMPT;
            Token token = ts.get();

            // check whether input is expression or QUIT command
            if (token.kind == QUIT)
                return; // exit
            else if (token.kind == TERMINATOR)
                continue; // read another expression
            else
                ts.unget(token); // input is expression, unget and read expression()

            // calculate the expression
            double ans = statement();
            ts.flush(); // flush out remaining TERMINATOR(';')

            // do not print trailing zeroes after decimal point if number is an integer
            long long int ans_int = ans;
            if (ans_int - ans == 0)
                std::cout << RESULT << ans_int << std::endl; // print the RESULT
            else
                std::cout << RESULT << ans << std::endl; // print the RESULT
        }
        catch (std::runtime_error &e)
        {
            printline();
            std::cout << "Error! ";
            std::cout << e.what() << std::endl;
            printline();

            std::cin.ignore(100, '\n'); // ignore until ; is reached
            ts.flush();                 // flush buffer

            // bool exit = ask();     // to continue or exit program?
            // if(exit)
            //     break;
            // }
        }
    }
}

//=====================================================================================
// Parsing functions for our calculator
//=====================================================================================

// READ A STATEMENT
// Statement:
//      declaration
//      assignment
//      expression
double statement()
{
    Token token = ts.get();
    switch (token.kind)
    {
    case LET:
        return declaration();
    case NAME:
    case CONSTANT:
    {
        Token token2 = ts.get();
        if (token2.kind == '=')
            return assign(token.name);
        else
        {
            ts.unget(token);
            ts.unget(token2);
            return expression();
        }
    }
    default:
        ts.unget(token);
        return expression();
    }
}

// READ A DECLARATION
// Declaration:
//      "let" Name "=" Expression
double declaration()
{
    try
    {
        Token token = ts.get();
        if (token.kind != NAME)
            throw std::runtime_error("Name expected in declaration => Error in declaration()");
        std::string var = token.name;
        if ((token = ts.get()).kind != '=')
            throw std::runtime_error("= missing in the declaration of the variable" + token.name + " => Error in declaration()");

        double val = expression();

        return define_name(var, val);
    }
    catch (std::runtime_error &e)
    {
        std::cout << "Oops! Error occured in Token.cpp => declaration()";
        std::cout << e.what() << std::endl;
        return 0;
    }
    catch (std::string &str)
    {
        std::cout << "Variable " << str << " already declared.\n";
        return 0;
    }
}

// READ AN ASSIGNMENT
//      Name "=" Expression
double assign(std::string name)
{
    // check if variable is already defined or not
    if (!is_defined(name, variables))
        throw std::runtime_error("Error in reassign() => variable \'" + name + "\' not declared");
    // or 'name' is a constant
    else if(!is_defined(name, constants))
        throw std::runtime_error("Error in assign() => cannot assign value to constants");
    // otherwise add variable 'name'
    else
        variables[name] = expression();

    return variables[name];
}

// READ AN EXPRESSION
// Expression:
//     Term
//     Term + Expression
//     Term - Expression
double expression()
{
    double d = term(); // get a term first

    Token token = ts.get(); // get a token
    switch (token.kind)
    {
    case '+':
        return (d + expression());

    case '-':
        ts.unget(token);
        return d + expression();

    // no expression after term
    default:
        ts.unget(token);
        return d;
    }
}

// READ A TERM
//     Primary
//     Primary * Term
//     Primary / Term
double term()
{
    double d = primary(); // read a primary first

    Token token = ts.get(); // read next token
    switch (token.kind)
    {
    case '*':
        return d * term();

    case '/':
    {
        double den = term();
        if (den == 0)
            throw std::runtime_error("cannot divide by zero");
        return d / den;
    }

    default: // if there is no term after current primary
        ts.unget(token);
        return d;
    }
}

// READ A PRIMARY
//      (+or-)Number
//      (+or-)'('Expression')'
//      variables
//      constants
double primary()
{
    Token token = ts.get(); // receive a token from the input stream

    switch (token.kind)
    {
    // primary is '('Expression')'
    case '(':
    {
        double d = expression(); // read an expression
        Token t = ts.get();
        if (t.kind != ')')
            throw std::runtime_error("Bad format => Error in primary()");
        else
            return d;
    }

    // primary is -(some number)
    case SUBTRACT:
        return -primary();

    // primary is a number
    case NUM_KIND:
        return token.value;

    case NAME:
        return variables[token.name];

    // some error occured
    default:
        throw std::runtime_error("Bad format => Error in primary()");
    }
}

// check if a value exists for 'var' in map 'm'
bool is_defined(std::string var, const std::map<std::string, double> &m)
{
    auto itr = m.find(var);
    if (itr != m.end())
        return true;
    else
        return false;
}
double define_name(std::string var, double val)
{
    if (is_defined(var, variables))
        throw var;
    else if (var == "let")
        throw std::runtime_error("Error in define_name() => \'let\' is a keyword\n");
    else
    {
        variables[var] = val;
        return val;
    }
}