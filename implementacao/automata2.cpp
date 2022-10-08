#include <iostream>
#include <string>
#include <map>
using namespace std;

map<string, string> tkn_op = {
        {"+",  "PLUS"},
        {"-",  "MINUS"},
        {"*",  "TIMES"},
        {"/",  "DIVIDE"},
        {"=",  "ATTRIBUTION"},
        {"==", "EQUALS"},
        {"!=", "DIFFERENT"},
        {">",  "GREATER"},
        {"<",  "LESS"},
        {">=", "GREATER_EQUAL"},
        {"<=", "LESS_EQUAL"}
    };

map<string, string> tkn_delim = {
        {"(", "LPAREN"},
        {")", "RPAREN"},
        {"[", "LBRACK"},
        {"]", "RBRACK"},
        {"{", "LBRACE"},
        {"}", "RBRACE"},
        {";", "SEMICOLON"},
        {",", "COMMA"}
    };

map<string, string> tkn_reserv = {
        {"if",     "IF"},
        {"else",   "ELSE"},
        {"while",  "WHILE"},
        {"for",    "FOR"},
        {"int",    "INT"},
        {"float",  "FLOAT"},
        {"return", "RETURN"},
        {"void",   "VOID"}
    };

string blank_sym = "\n\t ";
string op_sym =    "=+-*/<>!";
string delim_sym = "()[]{};,";
string num_sym =   "0123456789.";
string alph_sym =  "abcdefghijklmnopqrstuvwxyz"
                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ";


enum States {Q0, Q1, Q2, Q3, Q4, Q5, Q6, Q7, Q8, Q9, TRAP};

#define found !(string::npos)
#define token second

string code = "float fatorial(int n){ float fat; if ( n <= 1 ) return (1); else{ return n * fatorial(n - 1); } } int main(void){ int num = 3; int res = fatorial(num); return 0; }";

int deltaQ0(string in, int &i, string &buffer)
{
    char sym = in[i];

    if(blank_sym.find(sym) == found)
    {
        i++;
        return Q0; // inicial
    }
    
    if(alph_sym.find(sym) == found)
    {
        buffer += sym;
        i++;
        return Q1; // primeira letra
    }

    if(num_sym.find(sym) == found)
    {
        buffer += sym;
        i++;
        return Q4; // primeiro numero
    }

    if(op_sym.find(sym) == found)
    {
        buffer += sym;
        i++;
        return Q6; //primeiro operador
    }

    if(delim_sym.find(sym) == found)
    {
        buffer += sym;
        return Q8; // delimitador
    }
}

int deltaQ1(string in, int &i, string &buffer) //li uma letra
{
    char sym = in[i];

    if(blank_sym.find(sym) == found)
    {
        if(tkn_reserv.find(buffer) != tkn_reserv.end())
            return Q2; //aceita reservado
        return Q3; // aceita id
    }
    
    if(alph_sym.find(sym) == found)
    {
        buffer += sym;
        i++;
        return Q1; // outras letras
    }

    if(num_sym.find(sym) == found)
    {
        buffer += sym;
        i++;
        return Q1; // letra + numero
    }

    if(op_sym.find(sym) == found)
    {
        if(tkn_reserv.find(buffer) != tkn_reserv.end())
            return Q2; //aceita reservado
        return Q3; // aceita id
    }

    if(delim_sym.find(sym) == found)
    {
        if(tkn_reserv.find(buffer) != tkn_reserv.end())
            return Q2; //aceita reservado
        return Q3; // aceita id
    }
}

int deltaQ2(string in, int &i, string &buffer) // reconhece reservado
{
    cout << buffer <<  " ";
    cout << tkn_reserv.find(buffer)->token << endl;
    buffer.clear();
    return 0;
}

int deltaQ3(string in, int &i, string &buffer) // reconhece id
{
    cout << buffer <<  " ";
    cout << "ID" << endl;
    buffer.clear();
    return 0;
}

int deltaQ4(string in, int &i, string &buffer) //li um numero
{
    char sym = in[i];

    if(blank_sym.find(sym) == found)
    {
        return Q6; // aceita id
    }
    
    if(alph_sym.find(sym) == found)
    {
        buffer += sym;
        i++;
        return TRAP; // erro ao ler letra depois de numero
    }

    if(num_sym.find(sym) == found)
    {
        buffer += sym;
        i++;
        return Q1; // + numero
    }

    if(op_sym.find(sym) == found)
    {
        if(tkn_reserv.find(buffer) != tkn_reserv.end())
            return Q5; //aceita reservado
        return Q6; // aceita id
    }

    if(delim_sym.find(sym) == found)
    {
        if(tkn_reserv.find(buffer) != tkn_reserv.end())
            return Q5; //aceita reservado
        return Q6; // aceita id
    }
}

int deltaQ5(string in, int &i, string &buffer) // reconhece numero
{
    cout << buffer <<  " ";
    cout << "NUMBER" << endl;
    buffer.clear();
    return 0;
}

int deltaQ6(string in, int &i, string &buffer) //li um operador
{
    char sym = in[i];

    if(blank_sym.find(sym) == found)
    {
        return Q7; // aceita operador
    }
    
    if(alph_sym.find(sym) == found)
    {
        return Q7; 
    }

    if(num_sym.find(sym) == found)
    {
        return Q7;
    }

    if(op_sym.find(sym) == found)
    {
        buffer += sym;
        i++;
        if(buffer.length() > 2) return TRAP; //nao tem operadores com mais de 2 caracteres
        return Q6; //operador formado por + de um caracteres
    }

    if(delim_sym.find(sym) == found)
    {
        return TRAP; // nao da pra ler delimitador depois de operador
    }

    return TRAP;
}

int deltaQ7(string in, int &i, string &buffer) // reconhece operador
{
    cout << buffer <<  " ";
    cout << tkn_op.find(buffer)->token << endl;
    buffer.clear();
    return 0;
}

int deltaQTRAP(string in, int &i, string &buffer)
{

}