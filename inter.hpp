#ifndef __INTER_HPP__
#define __INTER_HPP__
#include <vector>
#include <iostream>
#include <cstdio>
#include <memory>
#include <stack>
#include <regex>
#include "dbms.hpp"
using namespace std;

enum lex_type_t {   IDN, STR, EQUAL, NUMBER, COMMA, ALL, OPEN, CLOSE,  END, ADD, SUB, DIV, MOD,
                    CREATE, INSERT, DELETE, DROP, SELECT, UPDATE, WHERE, SET, LE, L, B, BE, NE, 
                    FROM, INTO, LIKE, IN, TABLE, TEXT_T, LONG_T, AND, OR , NOT, EXIT, WHERE_ALL,
                    BOOL
                };
enum state_t {  _start, _comma, _open, _close, _idn, _str, _num, _all, _add, _sub, _mod, _and, 
                _or, _not, _div, _equal, _l, _le, _b, _be,  _ne, OK
            };
namespace lexer
{
    extern enum lex_type_t cur_lex_type;
    extern std::string cur_lex_text;
    extern string sentence;
    // current character
    extern int c;
    //current position in message
    extern int iter;
    // init lex_analyzer
    void init(string & Sentence);
    // read next lexem
    void next();
    // transfrom idn type to a special types 
    lex_type_t CheckType(string & s); 
}

namespace parser
{
    class SentenceException
    {
    public:
        string Message;
        virtual void PrintMessage ();
        SentenceException ();
        SentenceException ( const string& aMessage );
        SentenceException ( const SentenceException& exception );
        virtual ~ SentenceException ();
    };
    
    // str which server send client
    extern string server_answer;
    // sending answer flag
    extern bool answer_flag; 
    // exit flag
    extern bool end_flag;
    // parser::poliz for computing expression
    extern vector<string> poliz;
    // spec typpe parser::poliz 
    extern vector<lex_type_t> type_poliz;
    // init parser and lexer
    void init(string & Sentence);

    // choose clause
    void Start();
    // exit command analysis
    void ExitSen();
    // Select clause analysis.
    void SelectSen();
    // Update clause analysis.
    void UpdateSen();
    // Insert clause analysis.
    void InsertSen();
    // Drop clause analysis.
    void DropSen();
    // Delete clause analysis.
    void DeleteSen();
    // Create clause analysis.
    void CreateSen();

    // choose right where alternative
    int Where();

    // use poliz to store values and identificators, type_poliz stores type of poliz elems
    //  _logicaddition -> _logicmultiplication { OR _logicmultiplication }
    //  _logicmultiplication -> _logicfactor { AND _logicfactor }
    //  _logicfactor -> NOT _logicfactor | _compare
    //  _compare -> _addition [ = _addition | != _addition | > _addition | < _addition |
    //              >= _addition | <= _addition ]
    //  _addition -> multiplication {  ADD _multiplication | SUB  _multiplication } | TEXT_FIELD | STR 
    //  _multiplication -> _factor {  MULT _factor | DIV _factor | MOD _factor }
    //  _factor -> LONG_FIELD | NUMBER | ( _logicaddition )
    
    // Logic Expression analyzer. Read expression and compute it. Return bool mask. 
    vector<bool> LogicExpression(ITable * Table);
    void _logicaddition(ITable * Table);
    void _logicmultiplication(ITable * Table);
    void _logicfactor(ITable * Table);
    
    void _compare(ITable * Table);
    void _addition(ITable * Table);
    void _multiplication(ITable * Table);
    void _factor(ITable * Table);
    
    // Compute non-logic poliz. Return string vector.
    vector<string> ComputePoliz(ITable * Table);
    // Compute logic poliz. Return bool vector.
    vector<bool> ComputeLogicPoliz(ITable * Table);
    
    // For second where alternative. Check str in vector. 
    bool CheckIn(string & str, vector <string> & list);
}
#endif