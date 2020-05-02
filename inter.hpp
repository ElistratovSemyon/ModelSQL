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

enum lex_type_t { STR, COMMA, ALL, OPEN, CLOSE,  END };
enum state_t {H, COM, O, C, S, A, OK};

namespace lexer {
    enum lex_type_t cur_lex_type;
    std::string cur_lex_text;
    BaseSocket * sock;
    int c;
    
    void init(BaseSocket * pConn)
    {
        sock = pConn;
        c = sock->GetChar();
    }

    void next()
    {
        
        cur_lex_text.clear();
        state_t state = H;
        while (state != OK) {
            switch (state) {
            case H:
                if (std::isspace(c)) {
                    // stay in H
                } else if (c == ','){
                    state = COM; 
                    cur_lex_text += c;
                } else if (c == '*') {
                    state = A;
                    cur_lex_text += c;
                } else if (c == '(') {
                    state = O;
                    cur_lex_text += c;
                } else if (c == ')') {
                    state = C;
                    cur_lex_text += c;
                } else if (c == '\n' || c == EOF) {
                    cur_lex_type = END;
                    state = OK;
                } else if (c != "'")
                    state = S;
                    cur_lex_text += c;
                } 
                else
                {
                    throw "BAD";    
                }
                
                break;
            
            case S:
                if ( ! std::isspace(c)) {
                    cur_lex_text += c;
                } else {
                    cur_lex_type = STR;
                    state = OK;
                }
                break;

            case COM:
                if (std::isspace(c))
                {
                    cur_lex_type = COMMA;
                    state = OK;
                }
                else
                {
                    state = STR;
                    cur_lex_text += c;
                }
                break;
            case A:
                if (std::isspace(c))
                {
                    cur_lex_type = ALL;
                    state = OK; 
                }
                else
                {
                    state = STR;
                    cur_lex_text += c;
                }

                break;

            case O:
                if (std::isspace(c))
                {
                    cur_lex_type = OPEN;
                    state = OK;
                }
                else
                {
                    state = STR;
                    cur_lex_text += c;
                }
                break;

            case C:
                if (std::isspace(c))
                {
                    cur_lex_type = CLOSE;
                    state = OK;
                }
                else
                {
                    state = STR;
                    cur_lex_text += c;
                }
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
                        c = sock->GetChar();
                    }   
                    
                }
                
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
    void init(BaseSocket * pConn)
    {
        lexer::init(pConn);
        lexer::next();
        end_flag = false;
    }
    //stack <string> st;
    bool end_flag = false;
    bool IsIdentificator(string & str);
    bool IsLong(string & str);

    bool IsIdentificator(string & str)
    {
        char c = str[0];
        if (!( ((c >= 'a') && (c <= 'z') ) || ( (c >= 'A') && (c <= 'Z') )))
        {
            return false;
        }
        for (int i = 1; i < str.size(); i++)
        {
            c = str[i];
            if (!( ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || (c == '_') || (isdigit(c))))
            {
                return false;
            }
        }
        return true;
    }
   
    void Start()
    {
        
        lexer::next();
        if (lexer::cur_lex_type != STR)
        {
            throw "No command";
        }
        switch (lexer::cur_lex_text)
        {
            case "SELECT":
                SelectSen();
                break;
            case "INSERT":
                InsertSen();
                break;
            case "UPDATE":
                UpdateSen();
                break;
            case "DELETE":
                DeleteSen();
                break;
            case "CREATE":
                CreateSen();
                break;
            case "DROP":
                DropSen();
                break;
            case "EXIT":
                end_flag = true;
                
                break;
            default:
                throw "No such command";
                break;        
                                
        }
        
    }
    void SelectSen()
    {
        lexer::next();
        if (lexer::cur_lex_type == ALL)
        {
            throw "No command";
        }

    }


    void InsertSen()
    {
        lexer::next();
        if (lexer::cur_lex_type != STR || lexer::cur_lex_text != "INTO")
        {
            throw "No command";
        }
        lexer::next();
        if (lexer::cur_lex_type != STR)
        {
            throw "No command";
        }
        if (!IsIdentificator(lexer::cur_lex_text))
        {
            throw "Bad name";
        }
        std::string name = lexer::cur_lex_text;
        ITable * Table = MyTable::Open(name);
        Table->LastRecord();
        lexer::next();
        if (lexer::cur_lex_type != OPEN)
        {
            throw "BAD"
        }
        lexer::next();
        if (lexer::cur_lex_type == CLOSE)
        {
            throw "BAD"
        }
        int i = 0;
        while(lexer::cur_lex_type != CLOSE)
        {
            if (lexer::cur_lex_type != STR)
            {
                throw "No command";
            }
            IField * Field = Table->GetField(i);
            if (Field->OfType() == TEXT)
            {
                Field->Text() = lexer::cur_lex_text;
            }
            else
            {
                Field->Long() = stol(lexer::cur_lex_text);
            }
            lexer::next();
            if (lexer::cur_lex_type != CLOSE && lexer::cur_lex_type != COMMA)
            {
                throw "BAD";
            }
            
        }
        lexer::next();
        if (cur_lex_type != END)
        {
            throw "trouble";
        }
        Table->Add();
        delete Table;
    }
    
    void DeleteSen()
    {
        lexer::next();
        if (lexer::cur_lex_type != STR || lexer::cur_lex_text != "FROM")
        {
            throw "No command";
        }
        lexer::next();
        if (lexer::cur_lex_type != STR)
        {
            throw "No command";
        }
        if (!IsIdentificator(lexer::cur_lex_text))
        {
            throw "Bad name";
        }
        ITable * Table = MyTable::Open(lexer::cur_lex_text);
        Where();

    }
    
    void CreateSen()
    {
        lexer::next();
        if (lexer::cur_lex_type != STR || lexer::cur_lex_text != "TABLE")
        {
            throw "No command";
        }
        lexer::next();
        if (lexer::cur_lex_type != STR)
        {
            throw "No command";
        }
        if (!IsIdentificator(lexer::cur_lex_text))
        {
            throw "Bad name";
        }
        ITableStruct * Struct = MyTable::CreateTableStruct ();
        Struct->SetName(lexer::cur_lex_text)
        lexer::next();
        if (lexer::cur_lex_type != OPEN)
        {
            throw "BAD"
        }
        lexer::next();
        if (lexer::cur_lex_type == CLOSE)
        {
            throw "BAD"
        }
        while(lexer::cur_lex_type != CLOSE)
        {
            
            if (lexer::cur_lex_type != STR)
            {
                throw "No command";
            }
            if (!IsIdentificator(lexer::cur_lex_text))
            {
                throw "Bad name";
            }
            std::string column = lexer::cur_lex_text;
            lexer::next();
            if (lexer::cur_lex_type != STR)
            {
                throw "No command";
            }
            if (lexer::cur_lex_text == TEXT)
            {
                lexer::next();
                if (lexer::cur_lex_type != OPEN)
                {
                    throw "BAD";
                }
                lexer::next();
                if (lexer::cur_lex_type != STR)
                {
                    throw "BAD";
                }
                long val = stol(lexer::cur_lex_text);
                lexer::next();
                if (lexer::cur_lex_type != CLOSE)
                {
                    throw "BAD";
                }
                Struct->AddText(column, val);
            }
            else if (lexer::cur_lex_text == LONG)
            {  
                Struct->AddLong(column); 
            }
            else
            {
                throw "BAD";
            }
            lexer::next();
            if (lexer::cur_lex_type != CLOSE && lexer::cur_lex_type != COMMA)
            {
                throw "BAD";
            }
        }
        lexer::next();
        if (cur_lex_type != END)
        {
            throw "trouble";
        }
        ITable * Table = MyTable::Create(Struct);
        delete Table;
    }
    
    void DropSen()
    {
        lexer::next();
        if (lexer::cur_lex_type != STR || lexer::cur_lex_text != "TABLE")
        {
            throw "No command";
        }
        lexer::next();
        if (lexer::cur_lex_type != STR)
        {
            throw "No command";
        }
        if (!IsIdentificator(lexer::cur_lex_text))
        {
            throw "Bad name";
        }
        lexer::next();
        if (cur_lex_type != END)
        {
            throw "trouble";
        }
        MyTable::Drop(lexer::cur_lex_text);
    }

}
#endif