#ifndef __INTER_HPP__
#define __INTER_HPP__
#include "sock_wrap.hpp"
//#include "lex.h"
#include <vector>
#include <iostream>
#include <cstdio>
#include <memory>
#include <stack>
#include "dbms.hpp"
using namespace std;

enum lex_type_t { IDN, STR, NUMBER, COMMA, ALL, OPEN, CLOSE,  END };
enum state_t {H, COM, O, C, I, S, N,  A, OK};

namespace lexer
{
    enum lex_type_t cur_lex_type;
    std::string cur_lex_text;
    string sentence;
    int c;
    int iter;
    
    void init(string & Sentence)
    {
        sentence = Sentence;
        iter = 0;
        c = sentence[iter];
        
    }

    void next()
    {
        
        
        cur_lex_text.clear();
        state_t state = H;
        while (state != OK) 
        {
            
            switch (state) 
            {
                case H:

                    if (std::isspace(c)) {
                        // stay in H
                    } 
                    else if (c == ','){
                        state = COM; 
                        cur_lex_text += c;
                    } 
                    else if (c == '*') {
                        state = A;
                        cur_lex_text += c;
                    } 
                    else if (c == '(') {
                        state = O;
                        cur_lex_text += c;
                    } 
                    else if (c == ')') {
                        state = C;
                        cur_lex_text += c;
                    } 
                    else if (c == '\'') {
                        state = S;
                        cur_lex_text += c;
                    } 
                    else if (std::isdigit(c))
                    {
                        
                        state = N;
                        cur_lex_text += c;
                    } else if ( ((c >= 'a') && (c <= 'z') )|| ( (c >= 'A') && (c <= 'Z') ))
                    {
                        state = I;
                        cur_lex_text += c;
                    } else if (c == '\n' || c == EOF || c == 0) {
                        cur_lex_type = END;
                        state = OK;
                    }  
                    else
                    {
                        throw ("Invalid lexem");   
                    }
                    
                    break;
                
                case I:
                    if ( ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || (c == '_') || (isdigit(c))) {
                        cur_lex_text += c;
                    } else {
                        cur_lex_type = IDN;
                        state = OK;
                    }
                    break;
                case S:
                    if ( c != '\'') {
                        cur_lex_text += c;
                    } else {
                        cur_lex_text += c;
                        cur_lex_type = STR;
                        state = OK;
                    }
                    break;
                case N:
                    if (std::isdigit(c)) {
                        cur_lex_text += c;
                    } else {
                        cur_lex_type = NUMBER;
                        state = OK;
                    }
                    break;

                case COM:
                    
                    cur_lex_type = COMMA;
                    cur_lex_text = ",";
                    state = OK;
                    
                    break;
                case A:
                    cur_lex_type = ALL;
                    cur_lex_text = "*";
                    state = OK;
                    break;

                case O:
                    cur_lex_type = OPEN;
                    cur_lex_text = "(";
                    state = OK;
                    break;

                case C:
                    cur_lex_type = CLOSE;
                    cur_lex_text = ")";
                    state = OK;
                    break;

                case OK:
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
        cout << cur_lex_text << endl;        
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
    
    bool end_flag;
    void init(string & Sentence)
    {
        
        end_flag = false;
        lexer::init(Sentence);
        lexer::next();
        
    }

    

    
    void InsertSen();
    void DropSen();
    void CreateSen();
    void ExitSen();
   
    void Start()
    {
        
        if (lexer::cur_lex_type != IDN)
        {
            throw SentenceException("No such command " + lexer::cur_lex_text);
        }
        char c = lexer::cur_lex_text[0];
        
        switch (c)
        {
            case 'S':
                if (lexer::cur_lex_text == "SELECT")
                {
                    //SelectSen();
                }
                else
                {
                    throw SentenceException("Wrong command");
                }
                break;
            case 'I':
                if (lexer::cur_lex_text == "INSERT")
                {
                    InsertSen();
                }
                else
                {
                    throw SentenceException("Wrong command");
                }
                break;
            case 'U':
                if (lexer::cur_lex_text == "UPDATE")
                {
                    //UpdateSen();
                }
                else
                {
                    throw SentenceException("Wrong command");
                }
                break;
            case 'D':
                if (lexer::cur_lex_text == "DROP")
                {
                    DropSen();
                }
                else if (lexer::cur_lex_text == "DELETE")
                {
                    //DeleteSen();
                }
                else
                {
                    throw SentenceException("Wrong command");
                }
                break;

            case 'C':
                if (lexer::cur_lex_text == "CREATE")
                {
                    CreateSen();
                }
                else
                {
                    throw SentenceException("Wrong command");
                }
                break;

            case 'E':
                if (lexer::cur_lex_text == "EXIT")
                {
                    ExitSen();
                }
                else
                {
                    throw SentenceException("Wrong command");
                }
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
        lexer::next();
        if (lexer::cur_lex_type == ALL)
        {
            throw SentenceException("Incorrectly constructed SELECT clause");
        }

    }


    void InsertSen()
    {
        lexer::next();
        if (lexer::cur_lex_type != IDN || lexer::cur_lex_text != "INTO")
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
        Table->LastRecord();
        lexer::next();
        if (lexer::cur_lex_type != OPEN)
        {
            throw SentenceException("Incorrectly constructed INSERT clause");
        }
        lexer::next();
        if (lexer::cur_lex_type == CLOSE)
        {
            throw SentenceException("Incorrectly constructed INSERT clause");
        }
        int i = 0;
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
            lexer::next();
            if (lexer::cur_lex_type != CLOSE && lexer::cur_lex_type != COMMA)
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
        if (lexer::cur_lex_type != IDN || lexer::cur_lex_text != "FROM")
        {
            throw SentenceException("Incorrectly constructed DELETE clause");
        }
        lexer::next();
        if (lexer::cur_lex_type != IDN)
        {
            throw "No command";
        }
        ITable * Table = MyTable::Open(lexer::cur_lex_text);
        //Where();

    }
    
    void CreateSen()
    {
        lexer::next();
        if (lexer::cur_lex_type != IDN || lexer::cur_lex_text != "TABLE")
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
            if (lexer::cur_lex_type != IDN)
            {
                throw SentenceException("Incorrectly constructed CREATE clause");
            }
            if (lexer::cur_lex_text == "TEXT")
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
            else if (lexer::cur_lex_text == "LONG")
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
        if (lexer::cur_lex_type != IDN || lexer::cur_lex_text != "TABLE")
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

}
#endif