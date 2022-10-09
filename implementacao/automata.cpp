#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>
using namespace std;

#define token second

bool trapped = 0;

enum States {Q0, Q1, Q2, Q3, Q4, Q5, Q6, Q7, Q8, TRAP};

string blank_sym = "\n\t ";
string op_sym =    "=+-*/<>!";
string delim_sym = "()[]{};,";
string num_sym =   "0123456789.";
string alph_sym =  "abcdefghijklmnopqrstuvwxyz"
                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

map<string, string> operators = {
    { "+", "PLUS"         },
    { "-", "MINUS"        },
    { "*", "TIMES"        },
    { "/", "DIVIDE"       },
    { "=", "ATTRIBUTION"  },
    {"==", "EQUALS"       },
    {"!=", "DIFFERENT"    },
    { ">", "GREATER"      },
    { "<", "LESS"         },
    {">=", "GREATER_EQUAL"},
    {"<=", "LESS_EQUAL"   }
};

map<string, string> delimiters = {
    {";", "SEMICOLON"},
    {",", "COMMA"    },
    {"(", "LPAREN"   },
    {")", "RPAREN"   },
    {"[", "LBRACK"   },
    {"]", "RBRACK"   },
    {"{", "LBRACE"   },
    {"}", "RBRACE"   }
};

map<string, string> reserved_words = {
    {    "if", "IF"    },
    {  "else", "ELSE"  },
    { "while", "WHILE" },
    {   "for", "FOR"   },
    {   "int", "INT"   },
    { "float", "FLOAT" },
    {"return", "RETURN"},
    {  "void", "VOID"  }
};

bool valid(string list, char sym){ return list.find(sym) != string::npos; }

int q0(string in, int &i, string &buffer)
{
    char sym = in[i];

    if(valid(blank_sym, sym))
    {
        i++;
        return Q0; // inicial
    }
    
    if(valid(alph_sym, sym))
    {
        buffer += sym;
        i++;
        return Q1; // primeira letra
    }

    if(valid(num_sym, sym))
    {
        buffer += sym;
        i++;
        return Q4; // primeiro numero
    }

    if(valid(op_sym, sym))
    {
        buffer += sym;
        i++;
        return Q6; //primeiro operador
    }

    if(valid(delim_sym, sym))
    {
        buffer += sym;
        i++;
        return Q8; // delimitador
    }

    return TRAP;
}

int q1(string in, int &i, string &buffer) //li uma letra
{
    char sym = in[i];

    if(valid(blank_sym, sym)||valid(op_sym, sym)||valid(delim_sym, sym))
    {
        if(reserved_words.find(buffer) != reserved_words.end())
            return Q2; //aceita reservado
        return Q3; // aceita id
    }
    
    if(valid(alph_sym, sym)||valid(num_sym, sym))
    {
        buffer += sym;
        i++;
        return Q1; // outras letras
    }

    return TRAP;
}

int q2(string in, int &i, string &buffer) // reconhece reservado
{
    cout << reserved_words.find(buffer)->token << endl;
    buffer.clear();
    return Q0;
}

int q3(string in, int &i, string &buffer) // reconhece id
{
    cout << "ID" << endl;
    buffer.clear();
    return Q0;
}

int q4(string in, int &i, string &buffer) //li um numero
{
    char sym = in[i];

    if(valid(blank_sym, sym)||valid(op_sym, sym)||valid(delim_sym, sym))
        return Q5; // aceita numero
    
    if(valid(alph_sym, sym))
    {
        buffer += sym;
        i++;
        return TRAP; // erro ao ler letra depois de numero
    }

    if(valid(num_sym, sym))
    {
        buffer += sym;
        i++;
        return Q4; // + numero
    }

    return TRAP;
}

int q5(string in, int &i, string &buffer) // reconhece numero
{
    cout << "NUMBER" << endl;
    buffer.clear();
    return Q0;
}

int q6(string in, int &i, string &buffer) //li um operador
{
    char sym = in[i];

    if(valid(blank_sym, sym)||valid(alph_sym, sym)||valid(num_sym, sym))
        return Q7; // aceita operador

    if(valid(op_sym, sym))
    {
        buffer += sym;
        i++;
        if(buffer.length() > 2) return TRAP; //nao tem operadores com mais de 2 caracteres
        return Q6; //operador formado por + de um caracteres
    }

    if(valid(delim_sym, sym))
        return TRAP; // nao da pra ler delimitador depois de operador

    return TRAP;
}

int q7(string in, int &i, string &buffer) // reconhece operador
{
    cout << operators.find(buffer)->token << endl;
    buffer.clear();
    return Q0;
}

int q8(string in, int &i, string &buffer)
{
    cout << delimiters.find(buffer)->token << endl;
    buffer.clear();
    return Q0;
}

int tp(string in, int &i, string &buffer)
{
    buffer += in[i];
    i++;

    if(!trapped) 
    {
        cout << "----- TRAP -----" << endl;
        trapped = true;
    }

    return TRAP; // estado de erro
}

typedef int(*next_state)(string, int &, string &);

int main()
{
    next_state delta[] = {&q0, &q1, &q2, &q3, &q4, &q5, &q6, &q7, &q8, &tp}; 

    string file_name;
    cin >> file_name;
    ifstream file(file_name);
   
    if(!file.good())
    {
        cout << "erro\n";
        return 0;
    }
    
    stringstream code_buffer;
    code_buffer << file.rdbuf();
    string code = code_buffer.str();

    int i = 0;
    int cur_state = Q0;
    string buffer;

    bool ended = false;

    while(!ended)
    {
        if(i == code.length()) ended = true;
        
        cur_state = (*delta[cur_state])(code, i, buffer);
    }
}