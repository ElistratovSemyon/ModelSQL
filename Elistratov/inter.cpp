#include <vector>
#include <iostream>
#include <regex>
#include "dbms.hpp"
#include "inter.hpp"
using namespace std;



lex_type_t lexer::cur_lex_type;
std::string lexer::cur_lex_text;
std::string lexer::sentence;
// current character
int lexer::c;
//current position in message
int lexer::iter;

// str which server send client
string parser::server_answer;
// parser::poliz for computing expression
vector<string> parser::poliz;
// spec typpe parser::poliz 
vector<lex_type_t> parser::type_poliz;
// sending answer flag
bool parser::answer_flag; 
// exit flag
bool parser::end_flag;
// init lex_analyzer  
void lexer::init(string & Sentence)
{
    sentence = Sentence;
    iter = 0;
    c = sentence[iter];
}
// read next lexem
void lexer::next()
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
    // control lexem
    //cout << cur_lex_text << endl;        
}
// transfrom idn type to a special types 
lex_type_t lexer::CheckType(string & s)
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
    } 
    else 
    {
        return IDN; 
    }   
}

void parser::SentenceException::PrintMessage ()
{
    cerr << Message << endl;
}
parser::SentenceException::SentenceException () {}
parser::SentenceException::SentenceException ( const string& aMessage ) {
    Message = aMessage; 
}
parser::SentenceException::SentenceException ( const SentenceException& exception ) { 
    Message = exception.Message; 
}
parser::SentenceException::~SentenceException () {}
// init parser and lexer
void parser::init(string & Sentence)
{
    end_flag = false;
    answer_flag = false;
    parser::poliz.clear();
    parser::type_poliz.clear();
    server_answer.clear();
    lexer::init(Sentence);
    lexer::next();
}
// choose clause   
void parser::Start()
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

void parser::ExitSen()
{
    // spec flag to off server
    end_flag = true;
    lexer::next();
    if (lexer::cur_lex_type != END)
    {
        throw SentenceException("Incorrectly constructed EXIT clause");
    }
}

void parser::SelectSen()
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
    //read table name
    lexer::next();
    if (lexer::cur_lex_type != IDN)
    {
        throw SentenceException("Incorrectly constructed SELECT clause");
    }
    std::string name = lexer::cur_lex_text;
    
    server_answer.clear();
    
    ITable * Table = MyTable::Open(name);
    
    lexer::next();
    if (lexer::cur_lex_type != WHERE)
    {
        throw SentenceException("Incorrectly constructed SELECT clause");
    }
    bool not_flag = false;
    lexer::next();
    if (lexer::cur_lex_type == WHERE_ALL)
    {
        // where all
        // create message
        vector<struct Columns> cols = Table->GetColumns();
        vector<int> sizes;
        
        if (all_flag)
        {
            
            for (int i = 0; i < cols.size(); i++)
            {
                
                sizes.push_back(max<int>(cols[i].size, cols[i].name.size()));
                server_answer += cols[i].name;
                for (int j = 0; j < (sizes[i] - cols[i].name.size()); j++)
                {
                    server_answer += " ";
                }
                server_answer += "    ";
            }
        }
        else
        {
            for (int i = 0; i < columns.size(); i++)
            {
                for (int j = 0; j < cols.size(); j++)
                {
                    if (cols[j].name == columns[i])
                    {
                        sizes.push_back(max<int>(cols[j].size, cols[j].name.size()));
                        server_answer += cols[j].name;
                        for (int j = 0; j < (sizes[j] - cols[j].name.size()); j++)
                        {
                            server_answer += " ";
                        }
                        server_answer += "    ";
                    }
                }  
            }      
        }
        
        server_answer += "\n";
        for (Table->ReadFirst(); !Table->IsEnd(); Table->ReadNext())
        {
            if (all_flag == false)
            {
                for (int j = 0; j < columns.size(); j++)
                {
                    IField * Field = Table->GetField(columns[j]);
                    if (Field->OfType() == TEXT)
                    {
                        server_answer += Field->Text();
                        for (int k = 0; k < (sizes[j] - Field->Text().size()); k++)
                        {
                            server_answer += " ";
                            
                        }
                        server_answer += "    ";
                    }
                    else
                    {
                        string str = to_string(Field->Long());
                        server_answer +=  str;
                        for (int k = 0; k < (sizes[j] - str.size()); k++)
                        {
                            server_answer += " ";
                        }
                        server_answer += "    ";
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
                        server_answer += Field->Text();
                        for (int k = 0; k < (sizes[j] - Field->Text().size()); k++)
                        {
                            server_answer += " ";
                        }
                        server_answer += "    ";
                    }
                    else
                    {
                        string str = to_string(Field->Long());
                        server_answer +=  str;
                        for (int k = 0; k < (sizes[j] - str.size()); k++)
                        {
                            server_answer += " ";
                        }
                        server_answer += "    ";
                    }
                }
            }
            server_answer += "\n";
        }
    }
    else 
    {
        // read all expression
        _logicaddition(Table);
        if (lexer::cur_lex_type == NOT)
        {
            not_flag = true;
            lexer::next();
        }
        if (lexer::cur_lex_type == LIKE) // LIKE-alter
        {
            if (parser::poliz.size() != 1 || parser::type_poliz[0] != IDN)
            {
                throw SentenceException("Incorrectly constructed WHERE clause");
            }
            Table->LastRecord();
            string where_column;
            // check type of column
            if (Table->GetField(parser::poliz[0])->OfType() == TEXT )
            {
                where_column = parser::poliz[0];
            }
            else
            {
                throw SentenceException("Incorrectly constructed WHERE clause");
            }
            
            parser::poliz.clear();
            parser::type_poliz.clear();
            // read regex
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
            // transform regex to std::regex
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
            // create message
            vector<struct Columns> cols = Table->GetColumns();
            vector<int> sizes;
            if (all_flag)
            {
                
                for (int i = 0; i < cols.size(); i++)
                {
                    
                    sizes.push_back(max<int>(cols[i].size, cols[i].name.size()));
                    server_answer += cols[i].name;
                    for (int k = 0; k < (sizes[i] - cols[i].name.size()); k++)
                    {
                        server_answer += " ";
                    }
                    server_answer += "    ";
                }
            }
            else
            {
                for (int i = 0; i < columns.size(); i++)
                {
                    for (int j = 0; j < cols.size(); j++)
                    {
                        if (cols[j].name == columns[i])
                        {
                            sizes.push_back(max<int>(cols[j].size, cols[j].name.size()));
                            server_answer += cols[j].name;
                            for (int k = 0; k < (sizes[j] - cols[j].name.size()); k++)
                            {
                                server_answer += " ";
                            }
                            server_answer += "    ";
                        }
                    }  
                }      
            }
            
            server_answer += "\n";
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
                                server_answer += Field->Text();
                                for (int k = 0; k < (sizes[j] -Field->Text().size()); k++)
                                {
                                    server_answer += " ";
                                }
                                server_answer += "    ";
                            }
                            else
                            {
                                string str = to_string(Field->Long());
                                server_answer +=  str;
                                for (int k = 0; k < (sizes[j] - str.size()); k++)
                                {
                                    server_answer += " ";
                                }
                                server_answer += "    ";
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
                                server_answer += Field->Text();
                                for (int k = 0; k < (sizes[j] -Field->Text().size()); k++)
                                {
                                    server_answer += " ";
                                }
                                server_answer += "    ";
                            }
                            else
                            {
                                string str = to_string(Field->Long());
                                server_answer +=  str;
                                for (int k = 0; k < (sizes[j] - str.size()); k++)
                                {
                                    server_answer += " ";
                                }
                                server_answer += "    ";
                            }
                        }
                    }
                    server_answer += "\n";
                }
            }
        }
        else if (lexer::cur_lex_type == IN) // IN-clause
        {
            vector<string> mask;
            bool is_long = true;
            // determine type of expression
            if (parser::type_poliz.size() == 1 && parser::type_poliz.back() == STR)
            {
                // str
                is_long = false;   
                for(Table->ReadFirst(); ! Table->IsEnd(); Table->ReadNext())
                {
                    mask.push_back(parser::poliz[0]);
                }
            }
            else if (parser::type_poliz.size() == 1 && parser::type_poliz.back() == IDN && 
                        Table->GetField(parser::poliz.back())->OfType() == TEXT)
            {
                // text field
                is_long = false;
                for(Table->ReadFirst(); ! Table->IsEnd(); Table->ReadNext())
                {
                    mask.push_back(Table->GetField(parser::poliz[0])->Text());
                }

            }
            else
            {   
                // long expression
                is_long = true;
                mask = ComputePoliz(Table);
            }
            parser::poliz.clear();
            parser::type_poliz.clear();
            lexer::next();
            if (lexer::cur_lex_type != OPEN)
            {
                throw SentenceException("Incorrectly constructed WHERE clause");   
            }
            lexer::next();
            
            if (lexer::cur_lex_type == CLOSE)
            {
                throw SentenceException("Incorrectly constructed WHERE clause");
            }
            std::vector<std::string> const_list;
            // read IN constants
            while (lexer::cur_lex_type != CLOSE)
            {
                if (is_long) // long expression
                {
                    if (lexer::cur_lex_type != NUMBER)
                    {
                        throw SentenceException("Incorrectly constructed WHERE clause");
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
                        throw SentenceException("Incorrectly constructed WHERE clause");
                    }
                }
                else // text expression
                {
                    if (lexer::cur_lex_type != STR)
                    {
                        throw SentenceException("Incorrectly constructed WHERE clause");
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
                        throw SentenceException("Incorrectly constructed WHERE clause");
                    }     
                }
            }
            int j = 0;
            // create message
            vector<struct Columns> cols = Table->GetColumns();
            vector<int> sizes;
            if (all_flag)
            {
                
                for (int i = 0; i < cols.size(); i++)
                {
                    
                    sizes.push_back(max<int>(cols[i].size, cols[i].name.size()));
                    server_answer += cols[i].name;
                    for (int j = 0; j < (sizes[i] - cols[i].name.size()); j++)
                    {
                        server_answer += " ";
                    }
                    server_answer += "    ";
                }
            }
            else
            {
                for (int i = 0; i < columns.size(); i++)
                {
                    for (int j = 0; j < cols.size(); j++)
                    {
                        if (cols[j].name == columns[i])
                        {
                            sizes.push_back(max<int>(cols[j].size, cols[j].name.size()));
                            server_answer += cols[j].name;
                            for (int j = 0; j < (sizes[j] - cols[j].name.size()); j++)
                            {
                                server_answer += " ";
                            }
                            server_answer += "    ";
                        }
                    }  
                }      
            }
            
            server_answer += "\n";
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
                                server_answer += Field->Text();
                                for (int k = 0; k < (sizes[j] -Field->Text().size()); k++)
                                {
                                    server_answer += " ";
                                }
                                server_answer += "    ";
                            }
                            else
                            {
                                string str = to_string(Field->Long());
                                server_answer +=  str;
                                for (int k = 0; k < (sizes[j] -Field->Text().size()); k++)
                                {
                                    server_answer += " ";
                                }
                                server_answer += "    ";
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
                                server_answer += Field->Text();
                                for (int k = 0; k < (sizes[j] -Field->Text().size()); k++)
                                {
                                    server_answer += " ";
                                }
                                server_answer += "    ";
                            }
                            else
                            {
                                string str = to_string(Field->Long());
                                server_answer +=  str;
                                for (int k = 0; k < (sizes[j] -Field->Text().size()); k++)
                                {
                                    server_answer += " ";
                                }
                                server_answer += "    ";
                            }
                        }
                    }
                    server_answer += "\n";
                }           
            }
        } 
        else
        {
            if (not_flag)
            {
                throw SentenceException("Incorrectly constructed WHERE clause");
            }
            vector<bool> mask;
            mask = ComputeLogicPoliz(Table);
            int j = 0;
            // create message
            vector<struct Columns> cols = Table->GetColumns();
            vector<int> sizes;
            if (all_flag)
            {
                
                for (int i = 0; i < cols.size(); i++)
                {
                    
                    sizes.push_back(max<int>(cols[i].size, cols[i].name.size()));
                    server_answer += cols[i].name;
                    for (int j = 0; j < (sizes[i] - cols[i].name.size()); j++)
                    {
                        server_answer += " ";
                    }
                    server_answer += "    ";
                }
            }
            else
            {
                for (int i = 0; i < columns.size(); i++)
                {
                    for (int j = 0; j < cols.size(); j++)
                    {
                        if (cols[j].name == columns[i])
                        {
                            sizes.push_back(max<int>(cols[j].size, cols[j].name.size()));
                            server_answer += cols[j].name;
                            for (int j = 0; j < (sizes[j] - cols[j].name.size()); j++)
                            {
                                server_answer += " ";
                            }
                            server_answer += "    ";
                        }
                    }  
                }      
            }
            
            server_answer += "\n";
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
                                server_answer += Field->Text();
                                for (int k = 0; k < (sizes[j] -Field->Text().size()); k++)
                                {
                                    server_answer += " ";
                                }
                                server_answer += "    ";
                            }
                            else
                            {
                                string str = to_string(Field->Long());
                                server_answer +=  str;
                                for (int k = 0; k < (sizes[j] -Field->Text().size()); k++)
                                {
                                    server_answer += " ";
                                }
                                server_answer += "    ";
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
                                server_answer += Field->Text();
                                for (int k = 0; k < (sizes[j] -Field->Text().size()); k++)
                                {
                                    server_answer += " ";
                                }
                                server_answer += "    ";
                            }
                            else
                            {
                                string str = to_string(Field->Long());
                                server_answer +=  str;
                                for (int k = 0; k < (sizes[j] - str.size()); k++)
                                {
                                    server_answer += " ";
                                }
                                server_answer += "    ";
                            }
                        }
                    }
                    server_answer += "\n";
                }
            }

        }
    }
    // end clause control
    lexer::next();
    if (lexer::cur_lex_type != END)
    {
        throw SentenceException("Incorrectly constructed SELECT clause");
    }
    delete Table;
    
}

void parser::UpdateSen()
{
    std::vector<std::string> update_list;
    lexer::next();
    if (lexer::cur_lex_type != IDN)
    {
        throw SentenceException("Incorrectly constructed UPDATE clause");
    }
    std::string name = lexer::cur_lex_text;
    lexer::next();
    if (lexer::cur_lex_type != SET)
    {
        throw SentenceException("Incorrectly constructed UPDATE clause");
    }
    lexer::next();
    if (lexer::cur_lex_type != IDN)
    {
        throw SentenceException("Incorrectly constructed UPDATE clause");
    }
    string update_col = lexer::cur_lex_text;
    lexer::next();
    if (lexer::cur_lex_type != EQUAL)
    {
        throw SentenceException("Incorrectly constructed UPDATE clause");
    }
    
    ITable * Table = MyTable::Open(name);
    Table->LastRecord();
    lexer::next();
    bool is_long_exp = false;
    
    if (Table->GetField(update_col)->OfType() == TEXT)
    {
        if (lexer::cur_lex_type == STR)
        {
            for(Table->ReadFirst(); ! Table->IsEnd(); Table->ReadNext())
            {
                update_list.push_back(lexer::cur_lex_text);
            }
        }
        else
        {
            for(Table->ReadFirst(); ! Table->IsEnd(); Table->ReadNext())
            {
                update_list.push_back(Table->GetField(lexer::cur_lex_text)->Text());
            }
        }
        is_long_exp = false;   
        parser::poliz.clear();
        parser::type_poliz.clear();
        lexer::next();
    }
    else
    {
        _addition(Table);
        update_list = ComputePoliz(Table);
        is_long_exp = true;
    }
    

    
    if (lexer::cur_lex_type != WHERE)
    {
        throw SentenceException("Incorrectly constructed UPDATE clause");
    }
    bool not_flag = false;
    lexer::next();
    if (lexer::cur_lex_type == WHERE_ALL)
    {
        int i = 0;
        for (Table->ReadFirst(); !Table->IsEnd(); Table->ReadNext(), i++)
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
    else 
    {
        _logicaddition(Table);
        if (lexer::cur_lex_type == NOT)
        {
            not_flag = true;
            lexer::next();
        }
        if (lexer::cur_lex_type == LIKE) // LIKE-alter
        {
            if (parser::poliz.size() != 1 || parser::type_poliz[0] != IDN)
            {
                throw SentenceException("Incorrectly constructed WHERE clause");
            }
            Table->LastRecord();
            string where_column;

            if (Table->GetField(parser::poliz[0])->OfType() == TEXT )
            {
                where_column = parser::poliz[0];
            }
            else
            {
                throw SentenceException("Incorrectly constructed WHERE clause");
            }
            
            parser::poliz.clear();
            parser::type_poliz.clear();
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
        else if (lexer::cur_lex_type == IN)
        {
            vector<string> mask;
            bool is_long = true;
            if (parser::type_poliz.size() == 1)
            {
                if (parser::type_poliz.back() == STR)
                {
                    
                    for(Table->ReadFirst(); ! Table->IsEnd(); Table->ReadNext())
                    {
                        mask.push_back(parser::poliz[0]);
                    }
                }
                else if (Table->GetField(parser::poliz.back())->OfType() == TEXT)
                {
                    for(Table->ReadFirst(); ! Table->IsEnd(); Table->ReadNext())
                    {
                        mask.push_back(Table->GetField(parser::poliz[0])->Text());
                    }
                }
                else
                {
                    throw SentenceException("Incorrectly constructed WHERE clause");
                }
                
                parser::poliz.clear();
                parser::type_poliz.clear();
                is_long = false;
            }
            else
            {   
                is_long = true;
                mask = ComputePoliz(Table);
                parser::poliz.clear();
                parser::type_poliz.clear();
            }
            lexer::next();
            if (lexer::cur_lex_type != OPEN)
            {
                throw SentenceException("Incorrectly constructed WHERE clause");   
            }
            lexer::next();
            
            if (lexer::cur_lex_type == CLOSE)
            {
                throw SentenceException("Incorrectly constructed WHERE clause");
            }
            std::vector<std::string> const_list;
            while (lexer::cur_lex_type != CLOSE)
            {
                if (is_long)
                {
                    if (lexer::cur_lex_type != NUMBER)
                    {
                        throw SentenceException("Incorrectly constructed WHERE clause");
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
                        throw SentenceException("Incorrectly constructed WHERE clause");
                    }
                }
                else
                {
                    if (lexer::cur_lex_type != STR)
                    {
                        throw SentenceException("Incorrectly constructed WHERE clause");
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
                        throw SentenceException("Incorrectly constructed WHERE clause");
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
                    Table->Add();
                }
                
            }
            
        } 
        else
        {
            if (not_flag)
            {
                throw SentenceException("Incorrectly constructed WHERE clause");
            }
            vector<bool> mask;
            mask = ComputeLogicPoliz(Table);
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
                    Table->Add();
                }
                
            }
            
        }
    }
    
    lexer::next();
    if (lexer::cur_lex_type != END)
    {
        throw SentenceException("Incorrectly constructed UPDATE clause");
    }
    delete Table;
    

}

void parser::InsertSen()
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
        throw SentenceException("Incorrectly constructed INSERT clause");
    }
    int i = 0;
    Table->LastRecord();
    // read record
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
    // end clause control
    lexer::next();
    if (lexer::cur_lex_type != END)
    {
        throw SentenceException("Incorrectly constructed INSERT clause");
    }
    Table->Add();
    delete Table;
}



void parser::DeleteSen()
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
    if (lexer::cur_lex_type != WHERE)
    {
        throw SentenceException("Incorrectly constructed DELETE clause");
    }
    bool not_flag = false;
    lexer::next();
    if (lexer::cur_lex_type == WHERE_ALL)
    {
        // where all
        
    
        for (Table->ReadFirst(); !Table->IsEnd(); )
        {
            Table->Delete();            
        }

    }
    else 
    {
        // read expression
        _logicaddition(Table);
        if (lexer::cur_lex_type == NOT)
        {
            not_flag = true;
            lexer::next();
        }
        if (lexer::cur_lex_type == LIKE) // LIKE-alter
        {
            if (parser::poliz.size() != 1 || parser::type_poliz[0] != IDN)
            {
                throw SentenceException("Incorrectly constructed WHERE clause");
            }
            Table->LastRecord();
            string where_column;

            if (Table->GetField(parser::poliz[0])->OfType() == TEXT )
            {
                where_column = parser::poliz[0];
            }
            else
            {
                throw SentenceException("Incorrectly constructed WHERE clause");
            }
            
            parser::poliz.clear();
            parser::type_poliz.clear();
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
            // transform regex
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
            
            for (Table->ReadFirst(); !Table->IsEnd();)
            {
                
                if (regex_match(Table->GetField(where_column)->Text(), sample_regex) ^ not_flag)
                {
                    Table->Delete();
                }
                else
                {
                    Table->ReadNext();
                }
                
            }

        }
        else if (lexer::cur_lex_type == IN) // IN-clause
        {
            vector<string> mask;
            bool is_long = true;
            // determine type of expression
            if (parser::type_poliz.size() == 1)
            {
                if (parser::type_poliz.back() == STR)
                {
                    
                    for(Table->ReadFirst(); ! Table->IsEnd(); Table->ReadNext())
                    {
                        mask.push_back(parser::poliz[0]);
                    }
                }
                else if (Table->GetField(parser::poliz.back())->OfType() == TEXT)
                {
                    for(Table->ReadFirst(); ! Table->IsEnd(); Table->ReadNext())
                    {
                        mask.push_back(Table->GetField(parser::poliz[0])->Text());
                    }
                }
                else
                {
                    throw SentenceException("Incorrectly constructed WHERE clause"); 
                }
                
                parser::poliz.clear();
                parser::type_poliz.clear();
                is_long = false;
            }
            else
            {   
                is_long = true;
                mask = ComputePoliz(Table);
                parser::poliz.clear();
                parser::type_poliz.clear();
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
            std::vector<std::string> const_list;
            // read constants
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
            for (Table->ReadFirst(); !Table->IsEnd(); i++)
            {
                if (CheckIn(mask[i], const_list) ^ not_flag)
                {    
                    Table->Delete();                
                }
                else
                {
                    Table->ReadNext();
                }
                
            }
            
        } 
        else // logic expression
        {
            if (not_flag)
            {
                throw SentenceException("Incorrectly constructed WHERE clause");
            }
            vector<bool> mask;
            mask = ComputeLogicPoliz(Table);
            int i = 0;
            for (Table->ReadFirst(); !Table->IsEnd(); i++)
            {
                if (mask[i])
                {
                    Table->Delete();
                }
                else
                {
                    Table->ReadNext();
                }
            }
        }
    }
    // end clause control
    lexer::next();
    if (lexer::cur_lex_type != END)
    {
        throw SentenceException("Incorrectly constructed DELETE clause");
    }
    delete Table;
}

void parser::CreateSen()
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
    // read columns
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
        // check columns
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
    // end clause control
    lexer::next();
    if (lexer::cur_lex_type != END)
    {
        throw SentenceException("Incorrectly constructed CREATE clause");
    }
    
    ITable * Table = MyTable::Create(Struct);
    delete Table;
    
}

void parser::DropSen()
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
    // end clause control
    lexer::next();
    if (lexer::cur_lex_type != END)
    {
        throw SentenceException("Incorrectly constructed DROP clause");
    }
    MyTable::Drop(name);
}

vector<string> parser::ComputePoliz(ITable * Table)
{
    vector <int> st;
    vector<string> mask;
    // check on non-logic expression
    for (int i = 0; i < parser::type_poliz.size(); i++)
    {
        switch(parser::type_poliz[i])
        {
            case AND:
            case OR:
            case NOT:
            case LE:
            case L:
            case BE:
            case B:
            case NE:
            case EQUAL:
                throw SentenceException("No logic operands allowed in this alternative");
                break;
            default:
                break;
        }
    }
    // compute for each record
    for (Table->ReadFirst(); ! Table->IsEnd(); Table->ReadNext())
    {

        st.clear();
        for (int j = 0; j < parser::poliz.size(); j++)
        {
            if (parser::type_poliz[j] == IDN)
            {
                st.push_back(Table->GetField(parser::poliz[j])->Long());
            } 
            else if (parser::type_poliz[j] == NUMBER)
            {
                st.push_back(stol(parser::poliz[j]));
            }
            else
            {
                // operation
                int r_val = st.back();
                st.pop_back();
                int l_val = st.back();
                st.pop_back();
                switch (parser::type_poliz[j])
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
                        throw SentenceException("Wrong operations on numbers");
                        break;                   
                }
            }
        }
        mask.push_back(to_string(st.back()));
    }
    return mask;
}


bool parser::CheckIn(string & str, vector <string> & list)
{
    // IN-clause check
    for (int i = 0; i < list.size(); i++)
    {
        if (list[i] == str)
        {
            return true;
        }
    }
    return false;
}


vector<bool> parser::ComputeLogicPoliz(ITable * Table)
{
   
    vector <string> st;
    vector <lex_type_t> type_st;
    vector<bool> mask;
    // computing poliz
    for (Table->ReadFirst(); ! Table->IsEnd(); Table->ReadNext())
    {
        st.clear();
        type_st.clear();

        for (int j = 0; j < parser::poliz.size(); j++)
        {
            
            if (parser::type_poliz[j] == IDN)
            {
                if (Table->GetField(parser::poliz[j])->OfType() == TEXT)
                {
                    st.push_back(Table->GetField(parser::poliz[j])->Text());
                    type_st.push_back(STR);
                }
                else
                {
                    st.push_back(to_string(Table->GetField(parser::poliz[j])->Long()));
                    type_st.push_back(NUMBER);
                }
            } 
            else if (parser::type_poliz[j] == NUMBER)
            {
                st.push_back(parser::poliz[j]);
                type_st.push_back(NUMBER);
            }
            else if (parser::type_poliz[j] == STR)
            {
                st.push_back(parser::poliz[j]);
                type_st.push_back(STR);
            }
            else
            {
                string r_val_str = st.back();
                st.pop_back();
                
                lex_type_t r_val_type = type_st.back();
                type_st.pop_back();
                if (parser::type_poliz[j] == NOT)
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
                        throw SentenceException("Wrong operations");
                    } 
                }
                else
                {
                    string l_val_str = st.back();
                    st.pop_back();
                    lex_type_t l_val_type = type_st.back();
                    type_st.pop_back();
                    // check types
                    if (l_val_type != r_val_type)
                    {
                        throw SentenceException("Mismatch of operand types");
                    }
                    if (l_val_type == STR)
                    {
                        // str operations
                        switch(parser::type_poliz[j])
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
                                throw SentenceException("Wrong operation on string");
                                break;
                        }
                    }
                    else if (l_val_type == BOOL)
                    {
                        // bool operations
                        switch(parser::type_poliz[j])
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
                                throw SentenceException("Wrong operation on bool");
                                break;

                        }   
                    }
                    else
                    {
                        // long operations
                        switch (parser::type_poliz[j])
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
                                throw SentenceException("Wrong operation on numbers");
                                break;                   
                        }
                    }
                }
            }    
        }
        // check result
        if (type_st.back() != BOOL)
        {
            throw SentenceException("Incorrectly constructed WHERE clause");
        }
        else
        {
            // convert str to bool
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

//  _logicaddition -> _logicmultiplication { OR _logicmultiplication }
//  _logicmultiplication -> _logicfactor { AND _logicfactor }
//  _logicfactor -> NOT _logicfactor | _compare
//  _compare -> _addition [ = _addition | != _addition | > _addition | < _addition |
//              >= _addition | <= _addition ]
//  _addition -> multiplication {  ADD _multiplication | SUB  _multiplication } | TEXT_FIELD | STR 
//  _multiplication -> _factor {  MULT _factor | DIV _factor | MOD _factor }
//  _factor -> LONG_FIELD | NUMBER | ( _logicaddition )

void parser::_logicaddition(ITable * Table)
{
    _logicmultiplication(Table);
    while (lexer::cur_lex_type == OR) {
        lexer::next();
        _logicmultiplication(Table);
        parser::poliz.push_back("||");
        parser::type_poliz.push_back(OR);
    }
}

void parser::_logicmultiplication(ITable * Table)
{
    _logicfactor(Table);
    while (lexer::cur_lex_type == AND) {
        lexer::next();
        _logicfactor(Table);
        parser::poliz.push_back("&&");
        parser::type_poliz.push_back(AND);
    }
}

void parser::_logicfactor(ITable * Table)
{
    if (lexer::cur_lex_type == NOT) {
        lexer::next();
        _logicfactor(Table);
        parser::poliz.push_back("!");
        parser::type_poliz.push_back(NOT);    
    } 
    else 
    {
        _compare(Table);
    }
}    

void parser::_compare(ITable * Table)
{
    lex_type_t cmp;
    
    _addition(Table);
    if (lexer::cur_lex_type == NE || lexer::cur_lex_type == EQUAL || lexer::cur_lex_type == LE ||
                lexer::cur_lex_type == BE || lexer::cur_lex_type == L || lexer::cur_lex_type == B)
    {
        cmp = lexer::cur_lex_type;
        lexer::next();
        _addition(Table);
        parser::type_poliz.push_back(cmp);
        parser::poliz.push_back("?");
    }
}

void parser::_addition(ITable * Table)
{
    enum lex_type_add {P, M};
    lex_type_add flag;
    if (lexer::cur_lex_type == IDN && (Table->GetField(lexer::cur_lex_text)->OfType() == TEXT))
    {
        parser::poliz.push_back(lexer::cur_lex_text);
        parser::type_poliz.push_back(IDN);
        lexer::next();   
    }
    else if (lexer::cur_lex_type == STR)
    {
        parser::poliz.push_back(lexer::cur_lex_text);
        parser::type_poliz.push_back(STR);
        lexer::next();
    }
    else
    {
        _multiplication(Table);
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
            _multiplication(Table);
            if (flag == P)
            {
                parser::poliz.push_back("+");
                parser::type_poliz.push_back(ADD);
            }
            else
            {   
                parser::poliz.push_back("-");
                parser::type_poliz.push_back(SUB);
            }
        }
    }
}



void parser::_multiplication(ITable * Table)
{
    enum lex_type_mult {MU, DI, MO};
    lex_type_mult flag;
    _factor(Table);
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
        _factor(Table);
        if (flag == MU)
        {
            parser::poliz.push_back("*");
            parser::type_poliz.push_back(ALL);
        }
        else if (flag == DI)
        {   
            parser::poliz.push_back("/");
            parser::type_poliz.push_back(DIV);
        } 
        else
        {
            parser::poliz.push_back("%");
            parser::type_poliz.push_back(MOD);
        }
    }
}

void parser::_factor(ITable * Table)
{
    if (lexer::cur_lex_type == IDN) {
        parser::poliz.push_back(lexer::cur_lex_text);
        parser::type_poliz.push_back(lexer::cur_lex_type);
        lexer::next();
        
    } else if (lexer::cur_lex_type == NUMBER) {
        parser::poliz.push_back(lexer::cur_lex_text);
        parser::type_poliz.push_back(lexer::cur_lex_type);
        lexer::next();
        
    } else if (lexer::cur_lex_type == OPEN) {
        lexer::next();
        _logicaddition(Table);
        if (lexer::cur_lex_type != CLOSE) {
            throw SentenceException("");
        }
        lexer::next();
    } else {
        throw SentenceException("Incorrectly constructed WHERE clause");
    }
}
