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

state q0(in &cur_sym, string &buffer)
{
    if(valid(blank_sym, *cur_sym))
    {
        cur_sym++;
        return Q0; // inicial
    }
    
    if(valid(alph_sym, *cur_sym))
    {
        buffer += *(cur_sym++);
        return Q1; // primeira letra
    }

    if(valid(num_sym, *cur_sym))
    {
        buffer += *(cur_sym++);
        return Q4; // primeiro numero
    }

    if(valid(op_sym, *cur_sym))
    {
        buffer += *(cur_sym++);
        return Q6; // primeiro operador
    }

    if(valid(delim_sym, *cur_sym))
    {
        buffer += *(cur_sym++);
        return Q8; // delimitador
    }

    return TRAP;
}

state q1(in &cur_sym, string &buffer) // primeiro caracter eh letra
{
    if(valid(blank_sym, *cur_sym)||valid(op_sym, *cur_sym)||valid(delim_sym, *cur_sym))
    {
        if(reserved_words.find(buffer) != reserved_words.end())
            return Q2; // vai pra estado que reconhece reservado
        return Q3; // vai pra estado que reconhece id
    }
    
    if(valid(alph_sym, *cur_sym)||valid(num_sym, *cur_sym))
    {
        buffer += *(cur_sym++);
        return Q1; // outras letras ou numeros
    }

    return TRAP;
}

state q2(in &cur_sym, string &buffer) // RESERVADO
{
    output << reserved_words[buffer] << endl;
    buffer.clear();
    return Q0;
}

state q3(in &cur_sym, string &buffer) // ID
{
    output << "ID" << endl;
    buffer.clear();
    return Q0;
}

state q4(in &cur_sym, string &buffer) // primeiro caracter eh numero
{
    if(valid(blank_sym, *cur_sym)||valid(op_sym, *cur_sym)||valid(delim_sym, *cur_sym))
        return Q5; // vai pra estado que reconhece numero
    
    if(valid(alph_sym, *cur_sym))
    {
        buffer += *(cur_sym++);
        return TRAP; // vai pra estado de erro ao ler letra depois de numero
    }

    if(valid(num_sym, *cur_sym))
    {
        buffer += *(cur_sym++);
        return Q4; // continua lendo numeros
    }

    return TRAP;
}

state q5(in &cur_sym, string &buffer) // NUMERO
{
    output << "NUMBER" << endl;
    buffer.clear();
    return Q0;
}

state q6(in &cur_sym, string &buffer) // primeiro caracter eh operador
{
    if(valid(blank_sym, *cur_sym)||valid(alph_sym, *cur_sym)||valid(num_sym, *cur_sym))
        return Q7; // vai pra estado que reconhece operador

    if(valid(op_sym, *cur_sym))
    {
        buffer += *(cur_sym++);
        if(buffer.length() > 2) return TRAP; // erro ao ler + de 2 caracteres
        return Q6; // operador formado por + de 1 caracter
    }

    if(valid(delim_sym, *cur_sym))
        return TRAP; // erro ao ler delimitador depois de operador

    return TRAP;
}

state q7(in &cur_sym, string &buffer) // OPERADOR
{
    output << operators[buffer] << endl;
    buffer.clear();
    return Q0;
}

state q8(in &cur_sym, string &buffer) // DELIMITADOR
{
    output << delimiters[buffer] << endl;
    buffer.clear();
    return Q0;
}

state tp(in &cur_sym, string &buffer) // estado de erro
{
    buffer += *(cur_sym++); // consome fita de entrada
    return TRAP; // se mantem no erro ate fim da entrada
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
        remove(strcat(argv[2], ".txt"));
        exit(0);
    }
   
    stringstream codestream;
    codestream << input.rdbuf();
    string code(codestream.str());
    in cur_sym = code.begin();

    state cur_state = Q0, end_state = TRAP;
    string buffer;

    next_state delta[] = {&q0, &q1, &q2, &q3, &q4, &q5, &q6, &q7, &q8, &tp}; 

    bool ended = false;
    while(!ended)
    {
        if(cur_sym == code.end())
        {
            ended = true;
            end_state = cur_state;
        }

        cur_state = (*delta[cur_state])(cur_sym, buffer);
    }

    if(end_state != Q0) cout << "INPUT REJECTED\n";
    else                cout << "INPUT ACCEPTED\n";
}