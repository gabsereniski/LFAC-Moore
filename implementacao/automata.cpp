#include <iostream>
#include <string>
#include <map>
using namespace std;

#define not_found string::npos
#define token second

enum States {Q0, Q1, Q2, Q3, Q4, Q5, Q6, Q7, Q8, Q9, TRAP};

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

int delta_q0(string in, int &i, string &buffer)
{
    char sym = in[i];

    if(blank_sym.find(sym) != not_found)
    {
        i++;
        return Q0; // inicial
    }
    
    if(alph_sym.find(sym) != not_found)
    {
        buffer += sym;
        i++;
        return Q1; // primeira letra
    }

    if(num_sym.find(sym) != not_found)
    {
        buffer += sym;
        i++;
        return Q4; // primeiro numero
    }

    if(op_sym.find(sym) != not_found)
    {
        buffer += sym;
        i++;
        return Q6; //primeiro operador
    }

    if(delim_sym.find(sym) != not_found)
    {
        buffer += sym;
        i++;
        return Q8; // delimitador
    }

    return TRAP;
}

int delta_q1(string in, int &i, string &buffer) //li uma letra
{
    char sym = in[i];

    if((blank_sym.find(sym) != not_found)
    || (op_sym.find(sym)    != not_found)
    || (delim_sym.find(sym) != not_found))
    {
        if(tkn_reserv.find(buffer) != tkn_reserv.end())
            return Q2; //aceita reservado
        return Q3; // aceita id
    }
    
    if((alph_sym.find(sym) != not_found)
    || (num_sym.find(sym)  != not_found))
    {
        buffer += sym;
        i++;
        return Q1; // outras letras
    }

    return TRAP;
}

int delta_q2(string in, int &i, string &buffer) // reconhece reservado
{
    cout << tkn_reserv.find(buffer)->token << endl;
    buffer.clear();
    return 0;
}

int delta_q3(string in, int &i, string &buffer) // reconhece id
{
    cout << "ID" << endl;
    buffer.clear();
    return 0;
}

int delta_q4(string in, int &i, string &buffer) //li um numero
{
    char sym = in[i];

    if((blank_sym.find(sym) != not_found)
    || (op_sym.find(sym)    != not_found)
    || (delim_sym.find(sym) != not_found))
        return Q5; // aceita numero
    
    if(alph_sym.find(sym) != not_found)
    {
        buffer += sym;
        i++;
        return TRAP; // erro ao ler letra depois de numero
    }

    if(num_sym.find(sym) != not_found)
    {
        buffer += sym;
        i++;
        return Q4; // + numero
    }

    return TRAP;
}

int delta_q5(string in, int &i, string &buffer) // reconhece numero
{
    cout << "NUMBER" << endl;
    buffer.clear();
    return 0;
}

int delta_q6(string in, int &i, string &buffer) //li um operador
{
    char sym = in[i];

    if((blank_sym.find(sym) != not_found)
    || (alph_sym.find(sym) !=  not_found)
    || (num_sym.find(sym) !=   not_found))
        return Q7; // aceita operador

    if(op_sym.find(sym) != not_found)
    {
        buffer += sym;
        i++;
        if(buffer.length() > 2) return TRAP; //nao tem operadores com mais de 2 caracteres
        return Q6; //operador formado por + de um caracteres
    }

    if(delim_sym.find(sym) != not_found)
        return TRAP; // nao da pra ler delimitador depois de operador

    return TRAP;
}

int delta_q7(string in, int &i, string &buffer) // reconhece operador
{
    cout << tkn_op.find(buffer)->token << endl;
    buffer.clear();
    return 0;
}

int delta_q8(string in, int &i, string &buffer)
{
    cout << tkn_delim.find(buffer)->token << endl;
    buffer.clear();
    return 0;
}

int delta_tp(string in, int &i, string &buffer)
{
    buffer += in[i];
    i++;
    return TRAP; // estado de erro
}


int main()
{
    string code = "float fatorial(int n){ float fat; if ( n <= 1 ) return (1); else{ return n * fatorial(n - 1); } } int main(void){ int num = 3; int res = fatorial(num); return 0; }";

    int i = 0;
    int cur_state = Q0;
    string buffer;

    bool ended = false;

    while(!ended)
    {
        if(i == code.length()) ended = true;
        
        switch(cur_state)
        {
            case Q0: cur_state = delta_q0(code, i, buffer); break;
            case Q1: cur_state = delta_q1(code, i, buffer); break;
            case Q2: cur_state = delta_q2(code, i, buffer); break;
            case Q3: cur_state = delta_q3(code, i, buffer); break;
            case Q4: cur_state = delta_q4(code, i, buffer); break;
            case Q5: cur_state = delta_q5(code, i, buffer); break;
            case Q6: cur_state = delta_q6(code, i, buffer); break;
            case Q7: cur_state = delta_q7(code, i, buffer); break;
            case Q8: cur_state = delta_q8(code, i, buffer); break;
            default: cur_state = delta_tp(code, i, buffer); break;
        }
    }
}