#ifndef __INTER_HPP__
#define __INTER_HPP__
#include "sock_wrap.hpp"
//#include "lex.h"
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
    enum lex_type_t cur_lex_type;
    std::string cur_lex_text;
    string sentence;
    int c;
    int iter;
     
    lex_type_t CheckType(string & s); 
    void init(string & Sentence)
    {
        sentence = Sentence;
        iter = 0;
        c = sentence[iter];
        
    }

    void next()
    {
        
        
        cur_lex_text.clear();
        state_t state = _start;
        while (state != OK) 
        {
            
            switch (state) 
            {
                case _start:
                    
                    if (std::isspace(c)) {
                        // stay in _start
                    } 
                    else if (c == ','){
                        state = _comma; 
                        cur_lex_text += c;
                    } 
                    else if (c == '*') {
                        state = _all;
                        cur_lex_text += c;
                    }
                    else if (c == '+') {
                        state = _add;
                        cur_lex_text += c;
                    } 
                    else if (c == '-') {
                        state = _sub;
                        cur_lex_text += c;
                    } 
                    else if (c == '/') {
                        state = _div;
                        cur_lex_text += c;
                    } 
                    else if (c == '%') {
                        state = _mod;
                        cur_lex_text += c;
                    }  
                    else if (c == '(') {
                        state = _open;
                        cur_lex_text += c;
                    } 
                    else if (c == ')') {
                        state = _close;
                        cur_lex_text += c;
                    } 
                    else if (c == '\'') {
                        state = _str;
                    } 
                    else if (c == '=') {
                        state = _equal;
                        cur_lex_text += c;
                    }
                    else if (c == '<') {
                        state = _l;
                        cur_lex_text += c;
                    }  
                    else if (c == '>') {
                        state = _b;
                        cur_lex_text += c;
                    } 
                    else if (c == '!') {
                        state = _ne;
                        cur_lex_text += c;
                    } 
                    else if (std::isdigit(c))
                    {
                        state = _num;
                        cur_lex_text += c;
                    } else if ( ((c >= 'a') && (c <= 'z') ) || ( (c >= 'A') && (c <= 'Z') ))
                    {
                        state = _idn;
                        cur_lex_text += c;
                    } else if (c == '\n' || c == EOF || c == 0) {
                        cur_lex_type = END;
                        state = OK;
                    }  
                    else
                    {
                        throw ("Invalid lexem 1");   
                    }
                    
                    break;
                
                case _idn:
                    if ( ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || (c == '_') || (isdigit(c))) {
                        cur_lex_text += c;
                    } else {
                        
                        cur_lex_type = CheckType(cur_lex_text);
                        state = OK;
                    }
                    break;
                case _str:
                    if ( c != '\'') {
                        cur_lex_text += c;
                    } else {
                        cur_lex_type = STR;
                        state = OK;
                        iter++;
                        if (iter < sentence.size())
                        {
                            c = sentence[iter];
                        }
                    }
                    break;
                case _num:
                    if (std::isdigit(c)) {
                        cur_lex_text += c;
                    } else {
                        cur_lex_type = NUMBER;
                        state = OK;
                    }
                    break;

                case _comma:
                    
                    cur_lex_type = COMMA;
                    cur_lex_text = ",";
                    state = OK;
                    
                    break;
                case _all:
                    cur_lex_type = ALL;
                    cur_lex_text = "*";
                    state = OK;
                    break;

                case _open:
                    cur_lex_type = OPEN;
                    cur_lex_text = "(";
                    state = OK;
                    break;

                case _close:
                    cur_lex_type = CLOSE;
                    cur_lex_text = ")";
                    state = OK;
                    break;
                case _equal:
                    cur_lex_type = EQUAL;
                    cur_lex_text = "=";
                    state = OK;
                    break;
                
                case _add:
                    cur_lex_type = ADD;
                    cur_lex_text = "+";
                    state = OK;
                    break;
                case _sub:
                    cur_lex_type = SUB;
                    cur_lex_text = "-";
                    state = OK;
                    break;
                case _div:
                    cur_lex_type = DIV;
                    cur_lex_text = "/";
                    state = OK;
                    break;
                case _mod:
                    cur_lex_type = MOD;
                    cur_lex_text = "%";
                    state = OK;
                    break;
                case _or:
                    cur_lex_type = OR;
                    cur_lex_text = "||";
                    state = OK;
                    break;
                case _and:
                    cur_lex_type = AND;
                    cur_lex_text = "&&";
                    state = OK;
                    break;
                case _not:
                    cur_lex_type = NOT;
                    cur_lex_text = "!";
                    state = OK;
                    break;
                case _l:
                    if (c == '=') {
                        cur_lex_text += c;
                        cur_lex_type = LE;
                        state = OK;
                        iter++;
                        if (iter < sentence.size())
                        {
                            c = sentence[iter];
                        }
                        else
                        {
                            throw ("Invalid lexem");
                        } 
                    }
                    else
                    {
                        cur_lex_type = L;
                        state = OK;
                    }
                    break;
                case _b:
                    if (c == '=') {
                        cur_lex_text += c;
                        cur_lex_type = BE;
                        state = OK;
                        iter++;
                        if (iter < sentence.size())
                        {
                            c = sentence[iter];
                        }
                        else
                        {
                            throw ("Invalid lexem");
                        }
                    }
                    else
                    {
                        cur_lex_type = B;
                        state = OK;
                    }
                    break; 
                case _ne:
                    if (c == '=') {
                        cur_lex_text += c;
                        cur_lex_type = NE;
                        state = OK;
                        iter++;
                        if (iter < sentence.size())
                        {
                            c = sentence[iter];
                        }
                        else
                        {
                            throw ("Invalid lexem");
                        } 
                    }
                    else
                    {
                        throw ("Invalid lexem");
                    }
                    break;      
                case OK:
                    break;
                default:
                    throw ("Invalid lexem"); 
                    break;
            }
            
            if (state != OK)
            {
                if (c == '\n') {
                    cur_lex_type = END;
                    state = OK;
                } else {
                    iter++;
                    if (iter < sentence.size())
                    {
                        c = sentence[iter];
                    }
                    else
                    {
                        cur_lex_type = END;
                        state = OK;
                    }
                    
                }   
                
            }
        }
        //
        cout << cur_lex_text << endl;        
    }

    lex_type_t CheckType(string & s)
    {
        if (s == "CREATE")
        {
            return CREATE;
        }
        else if (s == "INSERT")
        {
            return INSERT;
        }
        else if (s == "DELETE")
        {
            return DELETE;
        }
        else if (s == "DROP")
        {
            return DROP;                              
        }
        else if (s == "SELECT")
        {
            return SELECT;
        }
        else if (s == "UPDATE")
        {
            return UPDATE;
        }
        else if (s == "WHERE")
        {
            return WHERE;
        } 
        else if (s == "ALL")
        {
            return WHERE_ALL;
        }
        else if (s == "SET")
        {
            return SET;
        }
        else if (s == "FROM")
        {
            return FROM;
        }
        else if (s == "INTO")
        {
            return INTO;
        }
        else if (s == "LIKE")
        {
            return LIKE;
        }
        else if (s == "IN")
        {
            return IN;
        }
        else if (s == "TABLE")
        {
            return TABLE;
        }
        else if (s == "TEXT")
        {
            return TEXT_T;
        }
        else if (s == "LONG")
        {
            return LONG_T;
        }
        else if (s == "AND")
        {
            return AND;
        }
        else if (s == "OR")
        {
            return OR;
        }
        else if (s == "NOT")
        {
            return NOT;
        } 
        else if (s == "EXIT")
        {
            return EXIT;
        } else 
        {
            return IDN; 
        }
    }
}

namespace parser
{
    /*****
    GRAMMAR:
    <SQL-предложение> -> SELECT <SELECT-предложение> | INSERT <INSERT-предложение> | UPDATE <UPDATE-предложение> | 
                            DELETE <DELETE-предложение> | CREATE <CREATE-предложение> | DROP <DROP-предложение>
    
    <SELECT-предложение> -> <список полей> FROM <имя таблицы> <WHERE-клауза>
    <список полей> -> <имя поля> { , <имя поля> } | *
    <имя таблицы> -> <имя>
    <имя поля> -> <имя>
    <имя> -> <идентификатор языка Си>

    <INSERT-предложение> -> INTO <имя таблицы> (<значение поля> { , <значение поля> })
    <значение поля> -> <строка> | <длинное целое>
    <строка> ::= '<символ> {<символ>}'
    <символ> ::= <любой, представляемый в компьютере символ, кроме апострофа '>
    *******/
    // i dont't use arity, it is easier without it
    // poliz owns its items objects
    class SentenceException
    {
    public:
        string Message;
        virtual void PrintMessage ()
        {
            cerr << Message << endl;
        }
        SentenceException () {}
        SentenceException ( const string& aMessage ) {
            Message = aMessage; 
        }
        SentenceException ( const SentenceException& exception ) { 
            Message = exception.Message; 
        }
        virtual ~ SentenceException () {}
    };
    

    string server_answer;
    bool answer_flag;
    bool end_flag;
    void init(string & Sentence)
    {
        
        end_flag = false;
        answer_flag = false;
        server_answer.clear();
        lexer::init(Sentence);
        lexer::next();
        
    }

    

    void SelectSen();
    void InsertSen();
    void UpdateSen();
    void DropSen();
    void DeleteSen();
    void CreateSen();
    void ExitSen();
    void Expression(vector<string> & poliz, vector<lex_type_t> & type_poliz, ITable * Table);
    void TextExpression(vector<string> & poliz, vector<lex_type_t> & type_poliz);
    void LongExpression(vector<string> & poliz, vector<lex_type_t> & type_poliz);

    vector<bool> LogicExpression(ITable * Table);


    

    void _factor(vector<string> & poliz, vector<lex_type_t> & type_poliz);
    void _multiplication(vector<string> & poliz, vector<lex_type_t> & type_poliz); 
    void _addition(vector<string> & poliz, vector<lex_type_t> & type_poliz);

    void _logicfactor(vector<string> & poliz, vector<lex_type_t> & type_poliz, ITable * Table);
    void _logicmultiplication(vector<string> & poliz, vector<lex_type_t> & type_poliz, ITable * Table); 
    void _logicaddition(vector<string> & poliz, vector<lex_type_t> & type_poliz, ITable * Table);

    vector<string> ComputePoliz(vector<string> & poliz, vector<lex_type_t> & type_poliz, ITable * Table);
    bool CheckIn(string & str, vector <string> & list);
    int Where();

    
    void Start()
    {
    
        
        switch (lexer::cur_lex_type)
        {
            case SELECT:
                SelectSen();
                break;
            case INSERT:
                InsertSen();
                break;
            case UPDATE:
                UpdateSen();
                break;
            case DROP:
                DropSen();
                break;
            case DELETE:                   
                DeleteSen();
                break;
            case CREATE:
                CreateSen();
                break;
            case EXIT:
                ExitSen();
                break;
            default:
                throw SentenceException("Wrong command");
                break;                               
        }
    }
    void ExitSen()
    {
        end_flag = true;
        lexer::next();
        if (lexer::cur_lex_type != END)
        {
            throw SentenceException("Incorrectly constructed EXIT clause");
        }
    }
    
    void SelectSen()
    {
        answer_flag = true;
        lexer::next();
        // * | columns
        vector<string> columns;
        columns.clear();
        bool all_flag = false;
        if (lexer::cur_lex_type == ALL)
        {
            // if *
            all_flag = true;
            lexer::next();
            if (lexer::cur_lex_type != FROM)
            {
                throw SentenceException("Incorrectly constructed SELECT clause");   
            }
            
        }
        else if (lexer::cur_lex_type == IDN)
        {
            // read column names until FROM
            while(true)
            {
                
                columns.push_back(lexer::cur_lex_text);
                lexer::next();
                if (lexer::cur_lex_type == FROM)
                {
                    break;  
                }
                else if (lexer::cur_lex_type == COMMA)
                {
                    lexer::next();
                    if (lexer::cur_lex_type != IDN)
                    {
                        throw SentenceException("Incorrectly constructed SELECT clause");
                    }
                }
                else
                {
                    throw SentenceException("Incorrectly constructed SELECT clause"); 
                } 
            }
        }
        else
        {
            throw SentenceException("Incorrectly constructed SELECT clause"); 
        }
        //read name
        lexer::next();
        if (lexer::cur_lex_type != IDN)
        {
            throw SentenceException("Incorrectly constructed SELECT clause");
        }
        string name = lexer::cur_lex_text;
        
        server_answer.clear();
        
        ITable * Table = MyTable::Open(name);
        
        lexer::next();
        if (lexer::cur_lex_type != WHERE)
        {
            throw SentenceException("Incorrectly constructed SELECT clause");
        }
        
        int alternative = Where();
        
        if (alternative == 0)
        {
            throw SentenceException("Incorrectly constructed WHERE clause");
        }
        else if (alternative == 1)
        {
            
            lexer::next();
            if (lexer::cur_lex_type != IDN)
            {
                throw SentenceException("Incorrectly constructed WHERE clause");
            }
            string where_column = lexer::cur_lex_text;
            Table->LastRecord();
            if (Table->GetField(where_column)->OfType() == LONG)
            {
                throw SentenceException("Incorrectly constructed WHERE clause, only TEXT field allowed");
            }
            bool not_flag = false;
            lexer::next();
            if (lexer::cur_lex_type == NOT)
            {
                not_flag = true;
                lexer::next();
            } 
            if (lexer::cur_lex_type != LIKE)
            {
                throw SentenceException("Incorrectly constructed WHERE clause");
            } 
            lexer::next();
            if (lexer::cur_lex_type != STR)
            {
                throw SentenceException("Incorrectly constructed WHERE clause");
            } 
            string sample = lexer::cur_lex_text;
            lexer::next();
            if (lexer::cur_lex_type != END)
            {
                throw SentenceException("Incorrectly constructed WHERE clause");   
            }

            
            for(int i = 0; i < sample.size(); i++)
            {
                if (sample[i] == '%')
                {
                    sample[i] = '*';
                    sample.insert(sample.begin() + i, '.');
                    i++;

                }
                else if (sample[i] == '_')
                {
                    sample[i] = '.';
                }
                else if ((sample[i] == '.') || (sample[i] == '*'))
                {
                    sample.insert(sample.begin() + i, '\\');
                }
            }
            std::regex sample_regex(sample);
            for (Table->ReadFirst(); !Table->IsEnd(); Table->ReadNext())
            {
                if (regex_match(Table->GetField(where_column)->Text(), sample_regex) ^ not_flag)
                {
                    if (all_flag == false)
                    {
                        for (int j = 0; j < columns.size(); j++)
                        {
                            IField * Field = Table->GetField(columns[j]);
                            if (Field->OfType() == TEXT)
                            {
                                server_answer += Field->Text() + "    ";
                            }
                            else
                            {
                                server_answer +=  to_string(Field->Long()) + "    ";
                            }
                        }
                    }
                    else
                    {
                        
                        for (int j = 0; j < Table->AmountCols(); j++)
                        {
                            
                            IField * Field = Table->GetField(j);
                            if (Field->OfType() == TEXT)
                            {
                                server_answer += Field->Text() + "    ";
                            }
                            else
                            {
                                server_answer +=  to_string(Field->Long()) + "    ";
                            }
                        }
                    }
                    server_answer += "\n";
                }
            }
        

        }
        else if (alternative == 2)
        {
            
            lexer::next();
            vector<string> poliz;
            vector<lex_type_t> type_poliz;
            vector<string> mask;
            bool is_long = true;
            bool not_flag = false;
            Expression(poliz, type_poliz, Table);
        
            if ((type_poliz.size() == 1) && ((type_poliz.back() == STR) || (Table->GetField(poliz.back())->OfType() == TEXT)))
            {
                if (type_poliz.back() == STR)
                {
                    
                    for(Table->ReadFirst(); ! Table->IsEnd(); Table->ReadNext())
                    {
                        mask.push_back(poliz[0]);
                    }
                }
                else
                {
                    for(Table->ReadFirst(); ! Table->IsEnd(); Table->ReadNext())
                    {
                        mask.push_back(Table->GetField(poliz[0])->Text());
                    }
                }
                
                poliz.clear();
                type_poliz.clear();
                
                is_long = false;
            }
            else
            {   
                mask = ComputePoliz(poliz, type_poliz, Table);
                poliz.clear();
                type_poliz.clear();
            }
            
            if (lexer::cur_lex_type == NOT)
            {
                not_flag = true;
                lexer::next();
            }
            if (lexer::cur_lex_type != IN)
            {
                throw SentenceException("Incorrectly constructed WHERE clause");   
            }
            lexer::next();
            if (lexer::cur_lex_type != OPEN)
            {
                throw SentenceException("Incorrectly constructed WHERE clause");   
            }
            lexer::next();
            
            if (lexer::cur_lex_type == CLOSE)
            {
                throw SentenceException("Incorrectly constructed WHERE clause close");
            }
            vector<string> const_list;
            while (lexer::cur_lex_type != CLOSE)
            {
                if (is_long)
                {
                    if (lexer::cur_lex_type != NUMBER)
                    {
                        throw SentenceException("Incorrectly constructed WHERE clause close");
                    }
                    const_list.push_back(to_string(stol(lexer::cur_lex_text)));
                    lexer::next();
                    if (lexer::cur_lex_type == COMMA)
                    {
                        lexer::next();
                    }
                    else if (lexer::cur_lex_type == CLOSE)
                    {
                        break;
                    }
                    else
                    {
                        throw SentenceException("Incorrectly constructed WHERE clause close");
                    }
                }
                else
                {
                    if (lexer::cur_lex_type != STR)
                    {
                        throw SentenceException("Incorrectly constructed WHERE clause close");
                    }
                    const_list.push_back(lexer::cur_lex_text);
                    lexer::next();
                    if (lexer::cur_lex_type == COMMA)
                    {
                        lexer::next();
                    }
                    else if (lexer::cur_lex_type == CLOSE)
                    {
                        break;
                    }
                    else
                    {
                        throw SentenceException("Incorrectly constructed WHERE clause close");
                    }                    
                }
            }
            

            int j = 0;
            for (Table->ReadFirst(), j = 0; !Table->IsEnd(); Table->ReadNext(), j++)
            {
                if (CheckIn(mask[j], const_list) ^ not_flag)
                {    
                    if (all_flag == false)
                    {
                        for (int j = 0; j < columns.size(); j++)
                        {
                            IField * Field = Table->GetField(columns[j]);
                            if (Field->OfType() == TEXT)
                            {
                                server_answer += Field->Text() + "    ";
                            }
                            else
                            {
                                server_answer +=  to_string(Field->Long()) + "    ";
                            }
                        }
                    }
                    else
                    {
                        
                        for (int j = 0; j < Table->AmountCols(); j++)
                        {
                            
                            IField * Field = Table->GetField(j);
                            if (Field->OfType() == TEXT)
                            {
                                server_answer += Field->Text() + "    ";
                            }
                            else
                            {
                                server_answer +=  to_string(Field->Long()) + "    ";
                            }
                        }
                    }
                    server_answer += "\n";
                }
            }
        }
        else if (alternative == 3)
        {   
            lexer::next();
            vector<bool> mask = LogicExpression(Table);
            
            int j = 0;
            for (Table->ReadFirst(); !Table->IsEnd(); Table->ReadNext(), j++)
            {
                if (mask[j])
                {
                    if (all_flag == false)
                    {
                        for (int j = 0; j < columns.size(); j++)
                        {
                            IField * Field = Table->GetField(columns[j]);
                            if (Field->OfType() == TEXT)
                            {
                                server_answer += Field->Text() + "    ";
                            }
                            else
                            {
                                server_answer +=  to_string(Field->Long()) + "    ";
                            }
                        }
                    }
                    else
                    {
                        
                        for (int j = 0; j < Table->AmountCols(); j++)
                        {
                            
                            IField * Field = Table->GetField(j);
                            if (Field->OfType() == TEXT)
                            {
                                server_answer += Field->Text() + "    ";
                            }
                            else
                            {
                                server_answer +=  to_string(Field->Long()) + "    ";
                            }
                        }
                    }
                    server_answer += "\n";
                }
            }
            
        }
        else
        {
            lexer::next();
            for (Table->ReadFirst(); !Table->IsEnd(); Table->ReadNext())
            {
                if (all_flag == false)
                {
                    for (int j = 0; j < columns.size(); j++)
                    {
                        IField * Field = Table->GetField(columns[j]);
                        if (Field->OfType() == TEXT)
                        {
                            server_answer += Field->Text() + "    ";
                        }
                        else
                        {
                            server_answer +=  to_string(Field->Long()) + "    ";
                        }
                    }
                }
                else
                {
                    
                    for (int j = 0; j < Table->AmountCols(); j++)
                    {
                        
                        IField * Field = Table->GetField(j);
                        if (Field->OfType() == TEXT)
                        {
                            server_answer += Field->Text() + "    ";
                        }
                        else
                        {
                            server_answer +=  to_string(Field->Long()) + "    ";
                        }
                    }
                }
                server_answer += "\n";
            }
        }
        lexer::next();
        if (lexer::cur_lex_type != END)
        {
            throw SentenceException("Incorrectly constructed UPDATE clause");
        }
        delete Table;
        
    }

    void UpdateSen()
    {
        vector<string> update_list;
        lexer::next();
        if (lexer::cur_lex_type != IDN)
        {
            throw SentenceException("Incorrectly constructed DELETE clause");
        }
        string name = lexer::cur_lex_text;
        lexer::next();
        if (lexer::cur_lex_type != SET)
        {
            throw SentenceException("Incorrectly constructed DELETE clause");
        }
        lexer::next();
        if (lexer::cur_lex_type != IDN)
        {
            throw SentenceException("Incorrectly constructed DELETE clause");
        }
        string update_col = lexer::cur_lex_text;
        lexer::next();
        if (lexer::cur_lex_type != EQUAL)
        {
            throw SentenceException("Incorrectly constructed DELETE clause");
        }
        
        ITable * Table = MyTable::Open(name);
        Table->LastRecord();
        lexer::next();
        bool is_long_exp = false;
        if (Table->GetField(update_col)->OfType() == TEXT)
        {
            vector<string> poliz;
            vector<lex_type_t> type_poliz;
            TextExpression(poliz, type_poliz);
            if (type_poliz.back() == STR)
            {
                
                for(Table->ReadFirst(); ! Table->IsEnd(); Table->ReadNext())
                {
                    update_list.push_back(poliz[0]);
                }
            }
            else
            {
                for(Table->ReadFirst(); ! Table->IsEnd(); Table->ReadNext())
                {
                    update_list.push_back(Table->GetField(poliz[0])->Text());
                }
            }
            is_long_exp = false;   
            poliz.clear();
            type_poliz.clear();
        }
        else
        {
            vector<string> poliz;
            vector<lex_type_t> type_poliz;
            LongExpression(poliz, type_poliz);
            update_list = ComputePoliz(poliz, type_poliz, Table);
            is_long_exp = true;
        }
        

        int alternative = Where();
        if (alternative == 0)
        {
            throw SentenceException("Incorrectly constructed WHERE clause");
        }
        else if (alternative == 1)
        {
            lexer::next();
            if (lexer::cur_lex_type != IDN)
            {
                throw SentenceException("Incorrectly constructed WHERE clause");
            }
            string where_column = lexer::cur_lex_text;
            Table->LastRecord();
            if (Table->GetField(where_column)->OfType() == LONG)
            {
                throw SentenceException("Incorrectly constructed WHERE clause, only TEXT field allowed");
            }
            bool not_flag = false;
            lexer::next();
            if (lexer::cur_lex_type == NOT)
            {
                not_flag = true;
                lexer::next();
            } 
            if (lexer::cur_lex_type != LIKE)
            {
                throw SentenceException("Incorrectly constructed WHERE clause");
            } 
            lexer::next();
            if (lexer::cur_lex_type != STR)
            {
                throw SentenceException("Incorrectly constructed WHERE clause");
            } 
            string sample = lexer::cur_lex_text;
            lexer::next();
            if (lexer::cur_lex_type != END)
            {
                throw SentenceException("Incorrectly constructed WHERE clause");   
            }

            for(int i = 0; i < sample.size(); i++)
            {
                if (sample[i] == '%')
                {
                    sample[i] = '*';
                    sample.insert(sample.begin() + i, '.');
                    i++;

                }
                else if (sample[i] == '_')
                {
                    sample[i] = '.';
                }
                else if ((sample[i] == '.') || (sample[i] == '*'))
                {
                    sample.insert(sample.begin() + i, '\\');
                }
            }
            std::regex sample_regex(sample);
            int i = 0;
            for (Table->ReadFirst(); !Table->IsEnd(); Table->ReadNext(), i++)
            {
                if (regex_match(Table->GetField(where_column)->Text(), sample_regex) ^ not_flag)
                {
                    if (!is_long_exp)
                    {
                        Table->GetField(update_col)->Text() = update_list[i];
                    }
                    else
                    {
                        Table->GetField(update_col)->Long() = stol(update_list[i]);
                    }
                    Table->Add();
                }
            }
        }
        else if (alternative == 2)
        {
            lexer::next();
            vector<string> poliz;
            vector<lex_type_t> type_poliz;
            vector<string> mask;
            bool is_long = true;
            bool not_flag = false;
            Expression(poliz, type_poliz, Table);
        
            if ((type_poliz.size() == 1) && ((type_poliz.back() == STR) || (Table->GetField(poliz.back())->OfType() == TEXT)))
            {
                if (type_poliz.back() == STR)
                {
                    
                    for(Table->ReadFirst(); ! Table->IsEnd(); Table->ReadNext())
                    {
                        mask.push_back(poliz[0]);
                    }
                }
                else
                {
                    for(Table->ReadFirst(); ! Table->IsEnd(); Table->ReadNext())
                    {
                        mask.push_back(Table->GetField(poliz[0])->Text());
                    }
                }
                
                poliz.clear();
                type_poliz.clear();
                
                is_long = false;
            }
            else
            {   
                mask = ComputePoliz(poliz, type_poliz, Table);
                poliz.clear();
                type_poliz.clear();
            }
            
            if (lexer::cur_lex_type == NOT)
            {
                not_flag = true;
                lexer::next();
            }
            if (lexer::cur_lex_type != IN)
            {
                throw SentenceException("Incorrectly constructed WHERE clause");   
            }
            lexer::next();
            if (lexer::cur_lex_type != OPEN)
            {
                throw SentenceException("Incorrectly constructed WHERE clause");   
            }
            lexer::next();
            
            if (lexer::cur_lex_type == CLOSE)
            {
                throw SentenceException("Incorrectly constructed WHERE clause close");
            }
            vector<string> const_list;
            while (lexer::cur_lex_type != CLOSE)
            {
                if (is_long)
                {
                    if (lexer::cur_lex_type != NUMBER)
                    {
                        throw SentenceException("Incorrectly constructed WHERE clause close");
                    }
                    const_list.push_back(to_string(stol(lexer::cur_lex_text)));
                    lexer::next();
                    if (lexer::cur_lex_type == COMMA)
                    {
                        lexer::next();
                    }
                    else if (lexer::cur_lex_type == CLOSE)
                    {
                        break;
                    }
                    else
                    {
                        throw SentenceException("Incorrectly constructed WHERE clause close");
                    }
                }
                else
                {
                    if (lexer::cur_lex_type != STR)
                    {
                        throw SentenceException("Incorrectly constructed WHERE clause close");
                    }
                    const_list.push_back(lexer::cur_lex_text);
                    lexer::next();
                    if (lexer::cur_lex_type == COMMA)
                    {
                        lexer::next();
                    }
                    else if (lexer::cur_lex_type == CLOSE)
                    {
                        break;
                    }
                    else
                    {
                        throw SentenceException("Incorrectly constructed WHERE clause close");
                    }                    
                }
            }
            

            int i = 0;
            for (Table->ReadFirst(); !Table->IsEnd(); Table->ReadNext(), i++)
            {
                if (CheckIn(mask[i], const_list) ^ not_flag)
                {    
                    if (!is_long_exp)
                    {
                        Table->GetField(update_col)->Text() = update_list[i];
                    }
                    else
                    {
                        Table->GetField(update_col)->Long() = stol(update_list[i]);
                    }
                }
                Table->Add();
            }
        }
        else if (alternative == 3)
        {
            lexer::next();
            vector<bool> mask = LogicExpression(Table);
            
            int i = 0;
            for (Table->ReadFirst(); !Table->IsEnd(); Table->ReadNext(), i++)
            {
                if (mask[i])
                {
                    if (!is_long_exp)
                    {
                        Table->GetField(update_col)->Text() = update_list[i];
                    }
                    else
                    {
                        Table->GetField(update_col)->Long() = stol(update_list[i]);
                    }
                }
                Table->Add();
            }
        }
        else
        {
            lexer::next();
            int i = 0;
            for (Table->ReadFirst(); !Table->IsEnd(); Table->ReadNext(), i++)
            {
                
                cout << "cycl" << endl;
                if (!is_long_exp)
                {
                    Table->GetField(update_col)->Text() = update_list[i];
                }
                else
                {
                    cout << "rewrite" << endl;
                    Table->GetField(update_col)->Long() = stol(update_list[i]);
                }
                Table->Add();
                
            }
        }
        lexer::next();
        if (lexer::cur_lex_type != END)
        {
            throw SentenceException("Incorrectly constructed UPDATE clause");
        }
        delete Table;

    }

    

    int Where()
    {
        int where_start = lexer::iter;
        int current_char = lexer::c;
        int alternative = 0;
        {
            lexer::next();
            if (lexer::cur_lex_type == WHERE_ALL)
            {
                alternative = 4;
            }
            while (lexer::cur_lex_type != END)
            {
                if (lexer::cur_lex_type == LIKE)
                {
                    alternative = 1;
                    break;
                }
                if (lexer::cur_lex_type == IN)
                {
                    alternative = 2;
                    break;
                }
                lexer::next();
            }
            if (alternative == 0)
            {
                alternative = 3;
            }
        }
        
        lexer::iter = where_start;
        lexer::c = current_char;
        return alternative;
    }

    void InsertSen()
    {
        lexer::next();
        if (lexer::cur_lex_type != INTO)
        {
            throw SentenceException("Incorrectly constructed INSERT clause");
        }
        lexer::next();
        if (lexer::cur_lex_type != IDN)
        {
            throw SentenceException("Incorrectly constructed INSERT clause");
        }
        std::string name = lexer::cur_lex_text;
        ITable * Table = MyTable::Open(name);
        
        lexer::next();
        if (lexer::cur_lex_type != OPEN)
        {
            throw SentenceException("Incorrectly constructed INSERT clause");
        }
        lexer::next();
        if (lexer::cur_lex_type == CLOSE)
        {
            throw SentenceException("Incorrectly constructed INSERT clause close");
        }
        int i = 0;
        Table->LastRecord();
        while(lexer::cur_lex_type != CLOSE)
        {
            
            
            IField * Field = Table->GetField(i);
            if (Field->OfType() == TEXT)
            {
                if (lexer::cur_lex_type != STR)
                {
                    throw SentenceException("Incorrectly constructed INSERT clause");
                }
                Field->Text() = lexer::cur_lex_text;
                
            }
            else
            {
                if (lexer::cur_lex_type != NUMBER)
                {
                    throw SentenceException("Incorrectly constructed INSERT clause");
                }
                Field->Long() = stol(lexer::cur_lex_text);
            }
            i++;
            lexer::next();
            if (lexer::cur_lex_type == COMMA)
            {
                lexer::next();
            }
            else if (lexer::cur_lex_type == CLOSE)
            {
                
                break;
            } 
            else
            {
                throw SentenceException("Incorrectly constructed INSERT clause");
            }
            
        }
        lexer::next();
        if (lexer::cur_lex_type != END)
        {
            throw SentenceException("Incorrectly constructed INSERT clause");
        }
        Table->Add();
        delete Table;
    }
    
    

    void DeleteSen()
    {
        lexer::next();
        if (lexer::cur_lex_type != FROM)
        {
            throw SentenceException("Incorrectly constructed DELETE clause");
        }
        lexer::next();
        if (lexer::cur_lex_type != IDN)
        {
            throw "No command";
        }
        ITable * Table = MyTable::Open(lexer::cur_lex_text);
        lexer::next();
        int alternative = Where();
        if (alternative == 0)
        {
            throw SentenceException("Incorrectly constructed WHERE clause");
        }
        else if (alternative == 1)
        {
            lexer::next();
            if (lexer::cur_lex_type != IDN)
            {
                throw SentenceException("Incorrectly constructed WHERE clause");
            }
            string where_column = lexer::cur_lex_text;
            Table->LastRecord();
            if (Table->GetField(where_column)->OfType() == LONG)
            {
                throw SentenceException("Incorrectly constructed WHERE clause, only TEXT field allowed");
            }
            bool not_flag = false;
            lexer::next();
            if (lexer::cur_lex_type == NOT)
            {
                not_flag = true;
                lexer::next();
            } 
            if (lexer::cur_lex_type != LIKE)
            {
                throw SentenceException("Incorrectly constructed WHERE clause");
            } 
            lexer::next();
            if (lexer::cur_lex_type != STR)
            {
                throw SentenceException("Incorrectly constructed WHERE clause");
            } 
            string sample = lexer::cur_lex_text;
            lexer::next();
            if (lexer::cur_lex_type != END)
            {
                throw SentenceException("Incorrectly constructed WHERE clause");   
            }

            for(int i = 0; i < sample.size(); i++)
            {
                if (sample[i] == '%')
                {
                    sample[i] = '*';
                    sample.insert(sample.begin() + i, '.');
                    i++;

                }
                else if (sample[i] == '_')
                {
                    sample[i] = '.';
                }
                else if ((sample[i] == '.') || (sample[i] == '*'))
                {
                    sample.insert(sample.begin() + i, '\\');
                }
            }
            std::regex sample_regex(sample);
            int i = 0;
            for (Table->ReadFirst(); !Table->IsEnd(); Table->ReadNext(), i++)
            {
                if (regex_match(Table->GetField(where_column)->Text(), sample_regex) ^ not_flag)
                {
                    Table->Delete();
                }
            }
        }
        else if (alternative == 2)
        {
            lexer::next();
            vector<string> poliz;
            vector<lex_type_t> type_poliz;
            vector<string> mask;
            bool is_long = true;
            bool not_flag = false;
            Expression(poliz, type_poliz, Table);
        
            if ((type_poliz.size() == 1) && ((type_poliz.back() == STR) || (Table->GetField(poliz.back())->OfType() == TEXT)))
            {
                if (type_poliz.back() == STR)
                {
                    
                    for(Table->ReadFirst(); ! Table->IsEnd(); Table->ReadNext())
                    {
                        mask.push_back(poliz[0]);
                    }
                }
                else
                {
                    for(Table->ReadFirst(); ! Table->IsEnd(); Table->ReadNext())
                    {
                        mask.push_back(Table->GetField(poliz[0])->Text());
                    }
                }
                
                poliz.clear();
                type_poliz.clear();
                
                is_long = false;
            }
            else
            {   
                mask = ComputePoliz(poliz, type_poliz, Table);
                poliz.clear();
                type_poliz.clear();
            }
            
            if (lexer::cur_lex_type == NOT)
            {
                not_flag = true;
                lexer::next();
            }
            if (lexer::cur_lex_type != IN)
            {
                throw SentenceException("Incorrectly constructed WHERE clause");   
            }
            lexer::next();
            if (lexer::cur_lex_type != OPEN)
            {
                throw SentenceException("Incorrectly constructed WHERE clause");   
            }
            lexer::next();
            
            if (lexer::cur_lex_type == CLOSE)
            {
                throw SentenceException("Incorrectly constructed WHERE clause close");
            }
            vector<string> const_list;
            while (lexer::cur_lex_type != CLOSE)
            {
                if (is_long)
                {
                    if (lexer::cur_lex_type != NUMBER)
                    {
                        throw SentenceException("Incorrectly constructed WHERE clause close");
                    }
                    const_list.push_back(to_string(stol(lexer::cur_lex_text)));
                    lexer::next();
                    if (lexer::cur_lex_type == COMMA)
                    {
                        lexer::next();
                    }
                    else if (lexer::cur_lex_type == CLOSE)
                    {
                        break;
                    }
                    else
                    {
                        throw SentenceException("Incorrectly constructed WHERE clause close");
                    }
                }
                else
                {
                    if (lexer::cur_lex_type != STR)
                    {
                        throw SentenceException("Incorrectly constructed WHERE clause close");
                    }
                    const_list.push_back(lexer::cur_lex_text);
                    lexer::next();
                    if (lexer::cur_lex_type == COMMA)
                    {
                        lexer::next();
                    }
                    else if (lexer::cur_lex_type == CLOSE)
                    {
                        break;
                    }
                    else
                    {
                        throw SentenceException("Incorrectly constructed WHERE clause close");
                    }                    
                }
            }
            

            int i = 0;
            for (Table->ReadFirst(); !Table->IsEnd(); Table->ReadNext(), i++)
            {
                if (CheckIn(mask[i], const_list) ^ not_flag)
                {    
                    Table->Delete();                }
            }
        }
        else if (alternative == 3)
        {
            lexer::next();
            vector<bool> mask = LogicExpression(Table);
            
            int i = 0;
            for (Table->ReadFirst(); !Table->IsEnd(); Table->ReadNext(), i++)
            {
                if (mask[i])
                {
                    Table->Delete();
                }
            }
        }
        else
        {
            lexer::next();
            int i = 0;
            for (Table->ReadFirst(); !Table->IsEnd(); Table->ReadNext(), i++)
            {
                Table->Delete();            
            }
        }
        lexer::next();
        if (lexer::cur_lex_type != END)
        {
            throw SentenceException("Incorrectly constructed UPDATE clause");
        }
        delete Table;


    }
    
    void CreateSen()
    {
        lexer::next();
        if (lexer::cur_lex_type != TABLE)
        {
            throw SentenceException("Incorrectly constructed CREATE clause");
        }
        lexer::next();
        if (lexer::cur_lex_type != IDN)
        {
            throw SentenceException("Incorrectly constructed CREATE clause");
        }
        ITableStruct * Struct = MyTable::CreateTableStruct ();
        Struct->SetName(lexer::cur_lex_text);
        lexer::next();
        if (lexer::cur_lex_type != OPEN)
        {
            throw SentenceException("Incorrectly constructed CREATE clause");
        }
        lexer::next();
        if (lexer::cur_lex_type == CLOSE)
        {
            throw SentenceException("Incorrectly constructed CREATE clause");
        }
        while(lexer::cur_lex_type != CLOSE)
        {
            
            if (lexer::cur_lex_type != IDN)
            {
                throw SentenceException("Incorrectly constructed CREATE clause");
            }
            std::string column = lexer::cur_lex_text;
            lexer::next();
            
            
            if (lexer::cur_lex_type == TEXT_T)
            {
                lexer::next();
                if (lexer::cur_lex_type != OPEN)
                {
                    throw SentenceException("Incorrectly constructed CREATE clause");
                }
                lexer::next();
                if (lexer::cur_lex_type != NUMBER)
                {
                    throw SentenceException("Incorrectly constructed CREATE clause");
                }
                long val = stol(lexer::cur_lex_text);
                lexer::next();
                if (lexer::cur_lex_type != CLOSE)
                {
                    throw SentenceException("Incorrectly constructed CREATE clause");
                }
                Struct->AddText(column, val);
            }
            else if (lexer::cur_lex_type == LONG_T)
            {  
                
                Struct->AddLong(column); 
            }
            else
            {
                throw SentenceException("Incorrectly constructed CREATE clause");
            }
            
            lexer::next();
            if (lexer::cur_lex_type == COMMA)
            {
                lexer::next();
            }
            else if (lexer::cur_lex_type == CLOSE)
            {
                
                break;
            } 
            else
            {
                throw SentenceException("Incorrectly constructed CREATE clause");
            }
            

        }
        lexer::next();
        if (lexer::cur_lex_type != END)
        {
            throw SentenceException("Incorrectly constructed CREATE clause");
        }
        
        ITable * Table = MyTable::Create(Struct);
        delete Table;
        
    }
    
    void DropSen()
    {
        lexer::next();
        if (lexer::cur_lex_type != TABLE)
        {
            throw SentenceException("Incorrectly constructed DROP clause");
        }
        lexer::next();
        if (lexer::cur_lex_type != IDN)
        {
            throw SentenceException("Incorrectly constructed DROP clause");
        }
        string name = lexer::cur_lex_text;
        lexer::next();
        if (lexer::cur_lex_type != END)
        {
            throw SentenceException("Incorrectly constructed DROP clause");
        }
        MyTable::Drop(name);
    }

    // 
    void Expression(vector<string> & poliz, vector<lex_type_t> & type_poliz, ITable * Table)
    {
        
        
        Type exp_type = LONG;
        
        if (lexer::cur_lex_type == STR)
        {
            exp_type = TEXT;
        }
        else if (lexer::cur_lex_type == IDN)
        {
            Table->LastRecord();
            if (Table->GetField(lexer::cur_lex_text)->OfType() == TEXT)
            {
                exp_type = TEXT;
            }
        }
        else
        {
            exp_type = LONG;
        }
        
        if (exp_type == TEXT)
        {
            TextExpression(poliz, type_poliz);
        }
        else if (exp_type == LONG)
        {
            LongExpression(poliz, type_poliz);
        }
        else
        {
            throw SentenceException("Incorrectly constructed WHERE clause");
        }
    }

    void TextExpression(vector<string> & poliz, vector<lex_type_t> & type_poliz)
    {
        
        vector <string> res;
        if (lexer::cur_lex_type == STR)
        {
            poliz.push_back(lexer::cur_lex_text);
            type_poliz.push_back(STR);
        }
        else if (lexer::cur_lex_type == IDN)
        {
            poliz.push_back(lexer::cur_lex_text);
            type_poliz.push_back(IDN);   
        }
        else
        {
            throw SentenceException("Incorrectly constructed WHERE clause");
        }
        lexer::next();
    }


    //  addition -> multiplication { [ ADD | SUB ] multiplication }
    //  multiplication -> factor { [ MULT | DIV | MOD ] factor }
    //  factor -> LONG_FIELD | NUMBER | OPEN addition CLOSE

    void LongExpression(vector<string> & poliz, vector<lex_type_t> & type_poliz)
    {
        _addition(poliz, type_poliz);
    }

    void _addition(vector<string> & poliz, vector<lex_type_t> & type_poliz)
    {
        enum lex_type_add {P, M};
        lex_type_add flag;
        _multiplication(poliz, type_poliz);
        while (lexer::cur_lex_type == ADD || lexer::cur_lex_type == SUB) {
            if (lexer::cur_lex_type == ADD)
            {
                flag = P;
            }
            else
            {
                flag = M;
            }
            lexer::next();
            _multiplication(poliz, type_poliz);
            if (flag == P)
            {
                poliz.push_back("+");
                type_poliz.push_back(ADD);
            }
            else
            {   
                poliz.push_back("-");
                type_poliz.push_back(SUB);
            }
            
        }
    }

    

    void _multiplication(vector<string> & poliz, vector<lex_type_t> & type_poliz)
    {
        enum lex_type_mult {MU, DI, MO};
        lex_type_mult flag;
        _factor(poliz, type_poliz);
        while (lexer::cur_lex_type == ALL | lexer::cur_lex_type == DIV | lexer::cur_lex_type == MOD) {
            if (lexer::cur_lex_type == ALL)
            {
                flag = MU;
            }
            else if (lexer::cur_lex_type == DIV)
            {
                flag = DI;
            }
            else
            {
                flag = MO;
            }
            lexer::next();
            _factor(poliz, type_poliz);
            if (flag == MU)
            {
                poliz.push_back("*");
                type_poliz.push_back(ALL);
            }
            else if (flag == DI)
            {   
                poliz.push_back("/");
                type_poliz.push_back(DIV);
            } 
            else
            {
                poliz.push_back("%");
                type_poliz.push_back(MOD);
            }
        }
    }

    void _factor(vector<string> & poliz, vector<lex_type_t> & type_poliz)
    {
        if (lexer::cur_lex_type == IDN) {
            poliz.push_back(lexer::cur_lex_text);
            type_poliz.push_back(lexer::cur_lex_type);
            lexer::next();
            
        } else if (lexer::cur_lex_type == NUMBER) {
            poliz.push_back(lexer::cur_lex_text);
            type_poliz.push_back(lexer::cur_lex_type);
            lexer::next();
            
        } else if (lexer::cur_lex_type == OPEN) {
            lexer::next();
            _addition(poliz, type_poliz);
            if (lexer::cur_lex_type != CLOSE) {
                throw SentenceException("");
            }
            lexer::next();
        } else {
            throw SentenceException("Incorrectly constructed WHERE clause");
        }
    }

    vector<string> ComputePoliz(vector<string> & poliz, vector<lex_type_t> & type_poliz, ITable * Table)
    {
        vector <int> st;
        vector<string> mask;
        for (Table->ReadFirst(); ! Table->IsEnd(); Table->ReadNext())
        {

            st.clear();
            for (int j = 0; j < poliz.size(); j++)
            {
                if (type_poliz[j] == IDN)
                {
                    st.push_back(Table->GetField(poliz[j])->Long());
                } 
                else if (type_poliz[j] == NUMBER)
                {
                    st.push_back(stol(poliz[j]));
                }
                else
                {
                    int r_val = st.back();
                    st.pop_back();
                    int l_val = st.back();
                    st.pop_back();
                    switch (type_poliz[j])
                    {
                        case ADD:
                            st.push_back(l_val + r_val);
                            break;
                        case SUB:
                            st.push_back(l_val - r_val);
                            break;
                        case ALL:
                            st.push_back(l_val * r_val);
                            break;
                        case MOD:
                            st.push_back(l_val % r_val);
                            break;
                        case DIV:
                            st.push_back(l_val / r_val);
                            break;
                        default:
                            throw SentenceException("Incorrectly constructed WHERE clause");
                            break;                   
                    }
                }
            }
            mask.push_back(to_string(st.back()));
        }
        return mask;
    }


    bool CheckIn(string & str, vector <string> & list)
    {
        for (int i = 0; i < list.size(); i++)
        {
            if (list[i] == str)
            {
                return true;
            }
        }
        return false;
    }


    vector<bool> LogicExpression(ITable * Table)
    {
        vector <string> poliz;
        vector<lex_type_t> type_poliz;
        _logicaddition(poliz, type_poliz, Table);
        vector <string> st;
        vector <lex_type_t> type_st;
        vector<bool> mask;
        
        for (Table->ReadFirst(); ! Table->IsEnd(); Table->ReadNext())
        {
            st.clear();
            type_st.clear();

            for (int j = 0; j < poliz.size(); j++)
            {
                
                if (type_poliz[j] == IDN)
                {
                    if (Table->GetField(poliz[j])->OfType() == TEXT)
                    {
                        st.push_back(Table->GetField(poliz[j])->Text());
                        type_st.push_back(STR);
                    }
                    else
                    {
                        st.push_back(to_string(Table->GetField(poliz[j])->Long()));
                        type_st.push_back(NUMBER);
                    }
                } 
                else if (type_poliz[j] == NUMBER)
                {
                    st.push_back(poliz[j]);
                    type_st.push_back(NUMBER);
                }
                else if (type_poliz[j] == STR)
                {
                    st.push_back(poliz[j]);
                    type_st.push_back(STR);
                }
                else
                {
                    string r_val_str = st.back();
                    st.pop_back();
                    
                    lex_type_t r_val_type = type_st.back();
                    type_st.pop_back();
                    if (type_poliz[j] == NOT)
                    {
                        if (r_val_type == BOOL)
                        {
                            if (r_val_str == "TRUE")
                            {
                                st.push_back("FALSE");
                                type_st.push_back(BOOL);
                            }
                            else
                            {
                                st.push_back("TRUE");
                                type_st.push_back(BOOL);
                            }    
                        }
                        else
                        {
                            throw SentenceException("Incorrectly constructed WHERE clause");
                        } 
                    }
                    else
                    {
                        /* code */
                    
                    
                        string l_val_str = st.back();
                        st.pop_back();
                        lex_type_t l_val_type = type_st.back();
                        type_st.pop_back();
                        if (l_val_type != r_val_type)
                        {
                            throw SentenceException("Incorrectly constructed WHERE clause");
                        }
                        if (l_val_type == STR)
                        {
                            switch(type_poliz[j])
                            {
                                case LE:
                                    if (l_val_str <= r_val_str)
                                    {
                                        type_st.push_back(BOOL);
                                        st.push_back("TRUE");
                                    }
                                    else
                                    {
                                        type_st.push_back(BOOL);
                                        st.push_back("FALSE");
                                    }
                                    break;
                                case L:
                                    if (l_val_str < r_val_str)
                                    {
                                        type_st.push_back(BOOL);
                                        st.push_back("TRUE");
                                    }
                                    else
                                    {
                                        type_st.push_back(BOOL);
                                        st.push_back("FALSE");
                                    }
                                    break;
                                case B:
                                    if (l_val_str > r_val_str)
                                    {
                                        type_st.push_back(BOOL);
                                        st.push_back("TRUE");
                                    }
                                    else
                                    {
                                        type_st.push_back(BOOL);
                                        st.push_back("FALSE");
                                    }
                                    break;
                                case BE:
                                    if (l_val_str >= r_val_str)
                                    {
                                        type_st.push_back(BOOL);
                                        st.push_back("TRUE");
                                    }
                                    else
                                    {
                                        type_st.push_back(BOOL);
                                        st.push_back("FALSE");
                                    }
                                    break;
                                case NE:
                                    if (l_val_str != r_val_str)
                                    {
                                        type_st.push_back(BOOL);
                                        st.push_back("TRUE");
                                    }
                                    else
                                    {
                                        type_st.push_back(BOOL);
                                        st.push_back("FALSE");
                                    }
                                    break; 
                                case EQUAL:
                                    if (l_val_str == r_val_str)
                                    {
                                        type_st.push_back(BOOL);
                                        st.push_back("TRUE");
                                    }
                                    else
                                    {
                                        type_st.push_back(BOOL);
                                        st.push_back("FALSE");
                                    }
                                    break; 
                                default:
                                    throw SentenceException("Incorrectly constructed WHERE clause");
                                    break;
                            }
                        }
                        else if (l_val_type == BOOL)
                        {
                            
                            switch(type_poliz[j])
                            {
                                case AND:
                                    if (l_val_str == "TRUE" && r_val_str == "TRUE")
                                    {
                                        type_st.push_back(BOOL);
                                        st.push_back("TRUE");
                                    }
                                    else
                                    {
                                        type_st.push_back(BOOL);
                                        st.push_back("FALSE");
                                    }
                                    break;
                                case OR:
                                    if (l_val_str == "TRUE" || r_val_str == "TRUE")
                                    {
                                        type_st.push_back(BOOL);
                                        st.push_back("TRUE");
                                    }
                                    else
                                    {
                                        type_st.push_back(BOOL);
                                        st.push_back("FALSE");
                                    }
                                    break;    
                                default:
                                    throw SentenceException("Incorrectly constructed WHERE clause");
                                    break;

                            }   
                        }
                        else
                        {
                            switch (type_poliz[j])
                            {
                                case ADD:
                                    st.push_back( to_string( stol(l_val_str) + stol(r_val_str) ) );
                                    type_st.push_back(NUMBER);
                                    break;
                                case SUB:
                                    st.push_back( to_string( stol(l_val_str) - stol(r_val_str) ) );
                                    type_st.push_back(NUMBER);
                                    break;
                                case ALL:
                                    st.push_back( to_string( stol(l_val_str) * stol(r_val_str) ) );
                                    type_st.push_back(NUMBER);
                                    break;
                                case MOD:
                                    st.push_back( to_string( stol(l_val_str) % stol(r_val_str) ) );
                                    type_st.push_back(NUMBER);
                                    break;
                                case DIV:
                                    st.push_back( to_string( stol(l_val_str) / stol(r_val_str) ) );
                                    type_st.push_back(NUMBER);
                                    break;
                                case LE:
                                    if (stol(l_val_str) <= stol(r_val_str))
                                    {
                                        type_st.push_back(BOOL);
                                        st.push_back("TRUE");
                                    }
                                    else
                                    {
                                        type_st.push_back(BOOL);
                                        st.push_back("FALSE");
                                    }
                                    break;
                                case L:
                                    if (stol(l_val_str) < stol(r_val_str))
                                    {
                                        type_st.push_back(BOOL);
                                        st.push_back("TRUE");
                                    }
                                    else
                                    {
                                        type_st.push_back(BOOL);
                                        st.push_back("FALSE");
                                    }
                                    break;
                                case B:
                                    if (stol(l_val_str) > stol(r_val_str))
                                    {
                                        type_st.push_back(BOOL);
                                        st.push_back("TRUE");
                                    }
                                    else
                                    {
                                        type_st.push_back(BOOL);
                                        st.push_back("FALSE");
                                    }
                                    break;
                                case BE:
                                    if (stol(l_val_str) >= stol(r_val_str))
                                    {
                                        type_st.push_back(BOOL);
                                        st.push_back("TRUE");
                                    }
                                    else
                                    {
                                        type_st.push_back(BOOL);
                                        st.push_back("FALSE");
                                    }
                                    break;
                                case NE:
                                    if (stol(l_val_str) != stol(r_val_str))
                                    {
                                        type_st.push_back(BOOL);
                                        st.push_back("TRUE");
                                    }
                                    else
                                    {
                                        type_st.push_back(BOOL);
                                        st.push_back("FALSE");
                                    }
                                    break; 
                                case EQUAL:
                                    if (stol(l_val_str) == stol(r_val_str))
                                    {
                                        type_st.push_back(BOOL);
                                        st.push_back("TRUE");
                                    }
                                    else
                                    {
                                        type_st.push_back(BOOL);
                                        st.push_back("FALSE");
                                    }
                                    break; 
                                default:
                                    throw SentenceException("Incorrectly constructed WHERE clause"); ///errr
                                    break;                   
                            }
                        }
                    }
                }    
            }
            if (type_st.back() != BOOL)
            {
                throw SentenceException("Incorrectly constructed WHERE clause");
            }
            else
            {
                if (st.back() == "TRUE")
                {
                    mask.push_back(true);
                }
                else
                {
                    mask.push_back(false);
                }
            }
        } 
        
        return mask; 
    }

    void _logicaddition(vector<string> & poliz, vector<lex_type_t> & type_poliz, ITable * Table)
    {
        _logicmultiplication(poliz, type_poliz, Table);
        while (lexer::cur_lex_type == OR) {
            lexer::next();
            _logicmultiplication(poliz, type_poliz, Table);
            poliz.push_back("||");
            type_poliz.push_back(OR);
        }
    }

    

    void _logicmultiplication(vector<string> & poliz, vector<lex_type_t> & type_poliz, ITable * Table)
    {
        _logicfactor(poliz, type_poliz, Table);
        while (lexer::cur_lex_type == AND) {
            lexer::next();
            _logicfactor(poliz, type_poliz, Table);
            poliz.push_back("&&");
            type_poliz.push_back(AND);
        }
    }

    void _logicfactor(vector<string> & poliz, vector<lex_type_t> & type_poliz, ITable * Table)
    {
        if (lexer::cur_lex_type == NOT) {
            lexer::next();
            _logicfactor(poliz, type_poliz, Table);
            poliz.push_back("!");
            type_poliz.push_back(NOT);    
        } else if (lexer::cur_lex_type == OPEN) {
            
            int buf_offset = lexer::iter;
            int current_char = lexer::c;
            bool is_exp_flag = true;
            int counter = 1;
            while(counter != 0)
            {
                lexer::next();
                switch(lexer::cur_lex_type)
                {
                    case AND:
                    case OR:
                    case NOT:
                        is_exp_flag = false;
                        counter = 0;
                        break;
                    case OPEN:
                        counter++;
                        break;
                    case CLOSE:
                        counter--;
                        break;   
                    case END:
                        is_exp_flag = true;
                        counter = 0;
                    default:
                        break;
                }
                
            }
            lexer::iter = buf_offset;
            lexer::c = current_char;
            lexer::next(); 
            if (is_exp_flag)
            {
                Expression(poliz, type_poliz, Table);
                string cmp;
                lex_type_t cmp_type;
                switch(lexer::cur_lex_type)
                {
                    case NE:
                        cmp = "!=";
                        cmp_type = NE;
                        break;
                    case EQUAL:
                        cmp = "==";
                        cmp_type = EQUAL;
                        break;
                    case LE:
                        cmp = "<=";
                        cmp_type = LE;
                        break;
                    case L:
                        cmp = "<";
                        cmp_type = L;
                        break;
                    case BE:
                        cmp = ">=";
                        cmp_type = BE;
                        break;
                    case B:
                        cmp = ">";
                        cmp_type = B;
                        break; 
                    default:
                        throw SentenceException("Incorrectly constructed WHERE clause");
                        break;
                }                  
                lexer::next();

                Expression(poliz, type_poliz, Table);
                poliz.push_back(cmp);
                type_poliz.push_back(cmp_type);
                
                
                
            }
            else
            {
                _logicaddition(poliz, type_poliz, Table);
            }
            if (lexer::cur_lex_type != CLOSE) {
                throw SentenceException("Incorrectly constructed WHERE clause try");
            }
            lexer::next();
        } else {
            throw SentenceException("Incorrectly constructed WHERE clause");
        }
    }    

}
#endif