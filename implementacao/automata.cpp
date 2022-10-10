#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>
using namespace std;

#define token second

ifstream input;
ofstream output;

enum state {Q0, Q1, Q2, Q3, Q4, Q5, Q6, Q7, Q8, TRAP};

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

state q0(string in, int &i, string &buffer)
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

state q1(string in, int &i, string &buffer) //li uma letra
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

state q2(string in, int &i, string &buffer) // reconhece reservado
{
    output << reserved_words[buffer] << endl;
    buffer.clear();
    return Q0;
}

state q3(string in, int &i, string &buffer) // reconhece id
{
    output << "ID" << endl;
    buffer.clear();
    return Q0;
}

state q4(string in, int &i, string &buffer) //li um numero
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

state q5(string in, int &i, string &buffer) // reconhece numero
{
    output << "NUMBER" << endl;
    buffer.clear();
    return Q0;
}

state q6(string in, int &i, string &buffer) //li um operador
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

state q7(string in, int &i, string &buffer) // reconhece operador
{
    output << operators[buffer] << endl;
    buffer.clear();
    return Q0;
}

state q8(string in, int &i, string &buffer)
{
    output << delimiters[buffer] << endl;
    buffer.clear();
    return Q0;
}

bool trapped = false;
state tp(string in, int &i, string &buffer)
{
    buffer += in[i];
    i++;

    if(!trapped) 
    {
        output << "----- TRAP -----" << endl;
        trapped = true;
    }

    return TRAP; // estado de erro
}

typedef state(*next_state)(string, int &, string &);

int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        cout << "Uso: <arquivo_entrada> <arquivo_saida>\n";
        exit(0);
    }

    input.open(argv[1]);
    output.open(argv[2]);
   
    if(!input.good())
    {
        cout << "Arquivo de entrada invalido.\n";
        exit(0);
    }

    if(!output.good())
    {
        cout << "Arquivo de saida com problema.\n";
        exit(0);
    }
   
    stringstream code;
    code << input.rdbuf();

    next_state delta[] = {&q0, &q1, &q2, &q3, &q4, &q5, &q6, &q7, &q8, &tp}; 

    int index = 0;
    state cur_state = Q0;
    state end_state = TRAP;
    string buffer;

    bool ended = false;
    while(!ended)
    {
        if(index == code.str().length())
        {
            ended = true;
            end_state = cur_state;
        }

        cur_state = (*delta[cur_state])(code.str(), index, buffer);
    }

    if(end_state != Q0) cout << "INPUT REJECTED\n";
    else                cout << "INPUT ACCEPTED\n";
}