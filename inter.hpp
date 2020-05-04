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

enum lex_type_t {   IDN, STR, EQUAL, NUMBER, COMMA, ALL, OPEN, CLOSE,  END, ADD, SUB, DIV, MOD,
                    CREATE, INSERT, DELETE, DROP, SELECT, UPDATE, WHERE, SET, 
                    FROM, INTO, LIKE, IN, TABLE, TEXT_T, LONG_T, AND, OR , NOT, EXIT, WHERE_ALL
                };
enum state_t {H, COM, O, C, I, S, N, A, P, M, PER, D, E, OK};

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
                    else if (c == '+') {
                        state = P;
                        cur_lex_text += c;
                    } 
                    else if (c == '-') {
                        state = M;
                        cur_lex_text += c;
                    } 
                    else if (c == '/') {
                        state = D;
                        cur_lex_text += c;
                    } 
                    else if (c == '%') {
                        state = PER;
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
                    else if (c == '=') {
                        state = E;
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
                        
                        cur_lex_type = CheckType(cur_lex_text);
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
                        iter++;
                        if (iter < sentence.size())
                        {
                            c = sentence[iter];
                        }
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
                case E:
                    cur_lex_type = EQUAL;
                    cur_lex_text = "=";
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
        bool answer_flag = false;
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
    vector<string> & Expression(ITable * Table);
    vector<string> & TextExpression(ITable * Table);
    vector<string> & LongExpression(ITable * Table);
    

    void _factor(vector<string> & poliz, vector<lex_type_t> & type_poliz);
    void _multiplication(vector<string> & poliz, vector<lex_type_t> & type_poliz); 
     void _addition(vector<string> & poliz, vector<lex_type_t> & type_poliz);

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
                //UpdateSen();
                break;
            case DROP:
                DropSen();
                break;
            case DELETE:                   
                //DeleteSen();
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
        answer_flag = true;
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

            /// regex check!!!
        } else if (alternative == 2)
        {
            vector<string> mask = Expression(Table);
        }

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
        cout << server_answer << endl;
        delete Table;
        
    }

    int Where()
    {
        int where_start = lexer::iter;
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
                cout << Field->Text() << endl;
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
        string col = lexer::cur_lex_text;
        lexer::next();
        if (lexer::cur_lex_type != EQUAL)
        {
            throw SentenceException("Incorrectly constructed DELETE clause");
        }
        
        ITable * Table = Mytable::Open(name);
        LastRecord();
        if (Table->GetField(col)->OfType() == TEXT)
        {
            update_list = TextExpression(Table);
        }
        else
        {
            update_list = LongExpression(Table);
        }
        Where();


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
        //Where();

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
    vector<string> & Expression(ITable * Table)
    {
        int buf_offset = lexer::iter;
        Type exp_type = 0;
        lexer::next();
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
        lexer::iter = buf_offset;
        if (exp_type == TEXT)
        {
            return TextExpression(Table);
        }
        else if (exp_type == LONG)
        {
            return LongExpression(Table);
        }
        else
        {
            throw SentenceException("Incorrectly constructed WHERE clause");
        }
    }

    vector<string> & TextExpression(ITable * Table)
    {
        lexer::next();
        vector <string> res;
        if (lexer::cur_lex_type == STR)
        {
            vector <string> buf = {lexer::cur_lex_text};
            res = buf;
        }
        else if (lexer::cur_lex_type == IDN)
        {
            for (Table->ReadFirst(); !Table->IsEnd(); Table->ReadNext())
            {
                res.push_back(Table->GetField(lexer::cur_lex_text)->Text());
            }
        }
        else
        {
            throw SentenceException("Incorrectly constructed WHERE clause");
        }
        return res;
    }


    //  addition -> multiplication { [ ADD | SUB ] multiplication }
    //  multiplication -> factor { [ MULT | DIV | MOD ] factor }
    //  factor -> LONG_FIELD | NUMBER | OPEN addition CLOSE

    vector<string> & LongExpression(ITable * Table)
    {
        vector<string> poliz;
        vector<lex_type_t> type_poliz;
        _addition(poliz, type_poliz);
        for (Table->ReadFirst(); ! Table->IsEnd(); Table->ReadNext())
        {
            
        }
    }

    void _addition(vector<string> & poliz, vector<lex_type_t> & type_poliz)
    {
        enum lex_type_add {P, M};
        lex_type_add flag;

        _multiplication();
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
            _multiplication();
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
        _factor();
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
            _factor();
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
            _addition();
            if (lexer::cur_lex_type != CLOSE) {
                throw std::logic_error("Unexpected token; closing parenthesis "
                        "is expected");
            }
            lexer::next();
        } else {
            throw std::logic_error("Unexpected token; identifier or number or "
                    "open parenthesis are expected");
        }
    }
}
#endif