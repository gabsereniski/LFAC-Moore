#include <string>
#include <map>
#include <iostream>
using namespace std;

map<string, string> token_op = {
    {"+", "PLUS"},
    {"-", "MINUS"},
    {"*", "TIMES"},
    {"/", "DIVIDE"},
    {"=", "ATTRIBUTION"},
    {"==", "EQUALS"},
    {"!=", "DIFFERENT"},
    {">", "GREATER"},
    {"<", "LESS"},
    {">=", "GREATER_EQUAL"},
    {"<=", "LESS_EQUAL"}
};

map<string, string> token_delim = {
    {"(", "LPAREN"},
    {")", "RPAREN"},
    {"[", "LBRACK"},
    {"]", "RBRACK"},
    {"{", "LBRACE"},
    {"}", "RBRACE"},
    {";", "SEMICOLON"},
    {",", "COMMA"}
};

map<string, string> token_reserv = {
    {"if",     "IF"},
    {"else",   "ELSE"},
    {"while",  "WHILE"},
    {"for", "FOR"},
    {"int",    "INT"},
    {"float",  "FLOAT"},
    {"return", "RETURN"},
    {"void",   "VOID"}
};

string lambda = "\n\t ";
string op = "=+-*/<>!";
string delim = "()[]{};,";

string code = "float fatorial(int n){ float fat; if ( n <= 1 ) return (1); else{ return n * fatorial(n - 1); } } int main(void){ int num = 3; int res = fatorial(num); return 0; }";


int cur_state;
string buff;
char c;

#define trap -1;

void return_ID()
{
    cout << "ID" << endl;
    buff.clear();
    cur_state = 0;
}

void return_RESERVED()
{
    cout << buff <<  " ";
    cout << token_reserv.find(buff)->second << endl;
    buff.clear();
    cur_state = 0;
}

void return_NUMBER()
{
    cout << buff <<  " ";
    printf("NUMBER\n");
    buff.clear();
    cur_state = 0;
}

void return_DELIM()
{
    cout << buff <<  " ";
    cout << token_delim.find(buff)->second << endl;
    buff.clear();
    cur_state = 0;
}

void return_OP()
{
    cout << buff <<  " ";
    cout << token_op.find(buff)->second << endl;
    buff.clear();
    cur_state = 0;
}

void t0()
{
    if(lambda.find(c) != string::npos)
    {
        cur_state = 0;
        return;
    }
    
    if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
    {
        cur_state = 1; //letras
        buff += c;
        return;
    }

    if((c >= '0' && c <= '9') || c == '.')
    {
        cur_state = 2; //numeros
        buff += c;
        return;
    }

    if(op.find(c) != string::npos)
    {
        cur_state = 3; //operadores
        buff += c;
        return;
    }

    if(delim.find(c) != string::npos)
    {
        buff += c;
        return_DELIM();
        return;
    }
    
}

void t1()
{
    if(lambda.find(c) != string::npos) //le vazio
    {
        if(token_reserv.find(buff) != token_reserv.end()) return_RESERVED();
        
        else return_ID();
        
        return;
    }

    if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) //letra
    {
        cur_state = 1; // id/reserv
        buff += c;
        return;
    }

    if((c >= '0' && c <= '9') || c == '.') //numero
    {
        cur_state = 1; // id/reserv
        buff += c;
        return;
    }

    if(op.find(c) != string::npos) //operador
    {
        if(token_reserv.find(buff) != token_reserv.end())
            return_RESERVED();

        else return_ID();

        buff += c;
        cur_state = 3;

        return;
    }

    if(delim.find(c) != string::npos)
    {
        if(token_reserv.find(buff) != token_reserv.end())
        {
            return_RESERVED();
            buff += c;
            return_DELIM();
            
        }

        else 
        {
            return_ID();
            buff += c;
            return_DELIM();
        }
    }

}

void t2() //numero
{
    if(lambda.find(c) != string::npos) //vazio
    {
        return_NUMBER();
        return;
    }

    if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) //letra
    {
        cur_state = trap; //numero seguido de letra
        buff += c;
        return;
    }

    if((c >= '0' && c <= '9') || c == '.') //numero
    {
        cur_state = 2; //continua lendo numero
        buff += c;
        return;
    }

    if(op.find(c) != string::npos) //operador
    {
        return_NUMBER();
        buff += c;
        cur_state = 3;
        return;
    }

    if(delim.find(c) != string::npos) //delimitador
    {
        return_NUMBER();
        buff += c;
        return_DELIM();
        return;
    }
}

void t3() //operadores
{
    if(lambda.find(c) != string::npos)
    {
        return_OP();
    }

    if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
    {
        return_OP();
        cur_state = 1; //numero seguido de letra
        buff += c;
        return;
    }

    if((c >= '0' && c <= '9') || c == '.')
    {
        return_OP();
        cur_state = 2; //continua lendo numero
        buff += c;
        return;
    }

    if(op.find(c) != string::npos)
    {
        cur_state = 3;
        buff += c;
        return;
    }

    if(delim.find(c) != string::npos)
    {
        return_OP();
        cur_state = trap;
        return;
    }
}


int main()
{
    cur_state = 0;
    for(int i = 0; i < code.length(); i++)
    {       
        c = code[i];
        switch(cur_state)
        {
            case 0: t0(); break;
            case 1: t1(); break;
            case 2: t2(); break;
            case 3: t3(); break;
        }     
    }
}