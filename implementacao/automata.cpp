#include <string.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>
using namespace std;

#define token second

typedef string::iterator in;
enum state {Q0, Q1, Q2, Q3, Q4, Q5, Q6, Q7, Q8, TRAP};
bool valid(string list, char sym){ return list.find(sym) != string::npos; }

ifstream input;
ofstream output;

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

state q0(in &sym, string &buffer)
{
    if(valid(blank_sym, *sym))
    {
        sym++;
        return Q0; // inicial
    }
    
    if(valid(alph_sym, *sym))
    {
        buffer += *(sym++);
        return Q1; // primeira letra
    }

    if(valid(num_sym, *sym))
    {
        buffer += *(sym++);
        return Q4; // primeiro numero
    }

    if(valid(op_sym, *sym))
    {
        buffer += *(sym++);
        return Q6; //primeiro operador
    }

    if(valid(delim_sym, *sym))
    {
        buffer += *(sym++);
        return Q8; // delimitador
    }

    return TRAP;
}

state q1(in &sym, string &buffer) //li uma letra
{
    if(valid(blank_sym, *sym)||valid(op_sym, *sym)||valid(delim_sym, *sym))
    {
        if(reserved_words.find(buffer) != reserved_words.end())
            return Q2; //aceita reservado
        return Q3; // aceita id
    }
    
    if(valid(alph_sym, *sym)||valid(num_sym, *sym))
    {
        buffer += *(sym++);
        return Q1; // outras letras
    }

    return TRAP;
}

state q2(in &sym, string &buffer) // reconhece reservado
{
    output << reserved_words[buffer] << endl;
    buffer.clear();
    return Q0;
}

state q3(in &sym, string &buffer) // reconhece id
{
    output << "ID" << endl;
    buffer.clear();
    return Q0;
}

state q4(in &sym, string &buffer) //li um numero
{
    if(valid(blank_sym, *sym)||valid(op_sym, *sym)||valid(delim_sym, *sym))
        return Q5; // aceita numero
    
    if(valid(alph_sym, *sym))
    {
        buffer += *(sym++);
        return TRAP; // erro ao ler letra depois de numero
    }

    if(valid(num_sym, *sym))
    {
        buffer += *(sym++);
        return Q4; // + numero
    }

    return TRAP;
}

state q5(in &sym, string &buffer) // reconhece numero
{
    output << "NUMBER" << endl;
    buffer.clear();
    return Q0;
}

state q6(in &sym, string &buffer) //li um operador
{
    if(valid(blank_sym, *sym)||valid(alph_sym, *sym)||valid(num_sym, *sym))
        return Q7; // aceita operador

    if(valid(op_sym, *sym))
    {
        buffer += *(sym++);
        if(buffer.length() > 2) return TRAP; //nao tem operadores com mais de 2 caracteres
        return Q6; //operador formado por + de um caracteres
    }

    if(valid(delim_sym, *sym))
        return TRAP; // nao da pra ler delimitador depois de operador

    return TRAP;
}

state q7(in &sym, string &buffer) // reconhece operador
{
    output << operators[buffer] << endl;
    buffer.clear();
    return Q0;
}

state q8(in &sym, string &buffer)
{
    output << delimiters[buffer] << endl;
    buffer.clear();
    return Q0;
}

bool trapped = false;
state tp(in &sym, string &buffer)
{
    buffer += *(sym++);

    if(!trapped) 
    {
        output << "ERROR: TRAP REACHED" << endl;
        trapped = true;
    }

    return TRAP; // estado de erro
}

typedef state(*next_state)(in &, string &);

int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        cout << "Use: <input_file> <output_file>\n";
        exit(0);
    }

    input.open(argv[1]);
    output.open(strcat(argv[2], ".txt"));
   
    if(!input.good())
    {
        cout << "Error opening input file.\n";
        remove(strcat(argv[2], ".txt"));
        exit(0);
    }

    if(!output.good())
    {
        cout << "Error handling output file.\n";
        exit(0);
    }
   
    stringstream codestream;
    codestream << input.rdbuf();
    string code(codestream.str());
    in sym = code.begin();

    state cur_state = Q0, end_state = TRAP;
    string buffer;

    next_state delta[] = {&q0, &q1, &q2, &q3, &q4, &q5, &q6, &q7, &q8, &tp}; 

    bool ended = false;
    while(!ended)
    {
        if(sym == code.end())
        {
            ended = true;
            end_state = cur_state;
        }

        cur_state = (*delta[cur_state])(sym, buffer);
    }

    if(end_state != Q0) cout << "INPUT REJECTED\n";
    else                cout << "INPUT ACCEPTED\n";
}