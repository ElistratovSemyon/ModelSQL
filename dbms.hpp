#if !defined _DBMS_H_
#define _DBMS_H_
#include <vector>
#include <string>
#include <map>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#define MAX_LENGTH 256
using namespace std;


/*
        TODO: 1. Исправить имена (переименовать name to table_name in notepad)

*/




enum Type { TEXT, LONG };
class TableException {
public:
    string Message;
    virtual void PrintMessage ()
    {
        cerr << Message << endl;
    }
    TableException () {}
    TableException ( const string& aMessage ) {
        Message = aMessage; 
    }
    TableException ( const TableException& exception ) { 
        Message = exception.Message; 
    }
    virtual ~ TableException () {}
};



class IField {
public:
    virtual Type OfType () = 0;
    virtual string & Text () = 0;
    virtual long & Long () = 0;
    //virtual IField * operator 
};

class ITextField: public IField {
public:
    virtual long & Long () { 
        throw TableException("Try to read long from text field."); 
    }
};

class MyTextField:public ITextField {
    string text;
public:
    MyTextField(const string & Text)
    {
        text = Text;
    }
    MyTextField()
    {
        text = "none";
    }
    virtual Type OfType ()
    {
        enum Type f_type = TEXT;//?
        return f_type;
    }
    virtual string & Text ()
    {
        return text;
    }
    ~MyTextField()
    {
        text.clear();
    }
};

class ILongField: public IField {
public:
    virtual string & Text () { 
        throw TableException("Try to read text from long field."); 
    }
};

class MyLongField:public ILongField {
    long value;
public:
    MyLongField(long Value = 0)
    {
        value = Value;
    }
    virtual Type OfType ()
    {
        enum Type f_type = LONG;//?
        return f_type;
    }
    virtual long & Long ()
    {
        return value;
    }

};


class ITableStruct {
public:
    virtual ITableStruct * AddText ( const string& Name, int Length ) = 0;
    virtual ITableStruct * AddLong ( const string& Name ) = 0;
    virtual ITableStruct * SetName ( const string& Name ) = 0;
    virtual vector<struct Columns> & GetVector() = 0;
    virtual string & GetName() = 0;
};


struct Columns
{
    //char name[MAX_LENGTH];
    string name;
    int size;
    enum Type type;
    Columns operator=(Columns x)
    {
        name = x.name;
        size = x.size;
        type = x.type;
        return *this;
    }
};

class MyTableStruct:public ITableStruct {
protected: 
    vector<struct Columns> list;
    string name;
public:
    MyTableStruct()
    {
        list.clear();
        name = "none";
    }
    virtual ITableStruct * AddText ( const string& Name, int Length )
    {
        struct Columns tmp;
        tmp.name = Name;
        tmp.type = TEXT;
        tmp.size = Length;
        list.push_back(tmp);
        return this;
    }
    virtual ITableStruct * AddLong ( const string& Name )
    {
        struct Columns tmp;
        tmp.name = Name;
        
        tmp.type = LONG;
        tmp.size = sizeof(long);
        list.push_back(tmp);
        return this;
    }
    virtual ITableStruct * SetName ( const string& Name )
    {
        list.clear();
        name = Name;
        return this;
    }
    virtual vector<struct Columns> & GetVector()
    {
        return list;
    }

    virtual string & GetName()
    {
        return name;
    }
    ~MyTableStruct()
    {
        list.clear();
        name.clear();
    }
};


class ITable {
public:    
    virtual void Add () = 0;
    virtual void Delete () = 0;
    virtual void Drop () = 0;
    virtual IField * GetField ( const string& Name ) = 0;
    virtual bool ReadFirst () = 0;
    virtual bool ReadNext () = 0;
    virtual void Update () = 0;
    virtual ~ ITable () {};
};

struct TableInfo
{
    long amount_record;
    long amount_cols;
    long first_record;
    long last_record;
    long current_record;
    long current_field;
    long record_size;
    
};




class MyTable:public ITable {
    
    string name;
    vector<struct Columns> cols;
    int fd;
    int amount_cols;
    struct TableInfo service_info;
    vector<IField *> window;


private:    
    MyTable(int File, vector<struct Columns> & list, string Name, struct TableInfo info)
    {
        cout << "in constructor\n";
        name = Name;
        
        cols.clear();
        for (int i = 0; i < list.size(); i++)
        {
            cols.push_back(list[i]);
        }
        service_info = info;
        fd = File;
        window.clear();
        for (int i = 0; i < cols.size(); i++)
        {
            if(cols[i].type == TEXT)
            {
                IField * new_field = new MyTextField("");
                window.push_back(new_field);
            
            }
            else
            {
                IField * new_field = new MyLongField();
                window.push_back(new_field);
            }
        }
        lseek(fd, service_info.first_record, SEEK_SET);
        if (service_info.first_record != service_info.last_record)
        cout << service_info.last_record << " - info" << endl;

    }
    string GetName()
    {
        return name;
    }
    ~MyTable()
    {
        close(fd);
        cols.clear();
        window.clear();
    }
public:
    static ITableStruct * CreateTableStruct ()
    {
        ITableStruct * new_struct = new MyTableStruct();
        return new_struct;
    }

    static ITable * Create ( ITableStruct * TableStruct )
    {
    
        int fd;
        fd = open(TableStruct->GetName().c_str(), O_CREAT | O_RDWR , 0666);
        if (fd == -1)
        {
            perror(0);
            exit(1);
        }
        
        struct TableInfo tmp = {0};

        tmp.amount_cols = TableStruct->GetVector().size();
        long record_size = 0;
        if (write(fd, &tmp, sizeof(tmp)) < 0)
        {
            throw TableException("Can't add service info to table: " + name);
        }
        for (auto i: TableStruct->GetVector())
        {
            if (write(fd, &i, sizeof(i)) < 0)
            {
                throw TableException("Can't add columns names to table: " + name);
            }
            record_size += i.size;

        }
        tmp.first_record = lseek(fd, 0L, SEEK_CUR);
        tmp.last_record = lseek(fd, 0L, SEEK_CUR);
        tmp.current_record = lseek(fd, 0L, SEEK_CUR);
        tmp.record_size = record_size;
        lseek(fd, 0L, SEEK_SET);
        if (write(fd, &tmp, sizeof(tmp)) < 0)
        {
            throw TableException("Can't add service info to table: " + name);
        }

        ITable * new_table = new MyTable(fd, TableStruct->GetVector(), TableStruct->GetName(), tmp);
        return new_table;
    }

    static void Drop ( const string & Name )
    {
        remove(Name.c_str());
    }

    static ITable * Open ( const string & Name )
    {
        int fd;
        vector<struct Columns> list;
        fd = open(Name.data(), O_RDWR);
        if (fd == -1)
        {
            perror(0);
            throw TableException("Can't open file.");
        }
        
        struct TableInfo info;
        if (read(fd, &info, sizeof(info)) < 0)
        {
            throw TableException("Can't read service info from table " + Name);
        }
        list.reserve(info.amount_cols);
        for(int i = 0; i < list.size(); i++)
        {
            struct Columns tmp;
            if (read(fd, &tmp, sizeof(tmp)) < 0)
            {
                throw TableException("Can't read columns names from table " + Name);
            }
            list.push_back(tmp);
        }


    
        
        
        ITable * new_table = new MyTable(fd, list, Name, info);
        cout << "open call constructor\n";
        return new_table;   
        
    }

    virtual void Add ()
    {
        for(int i = 0; i < window.size(); i++)
        {
            if (window[i]->OfType() == TEXT)
            {   
                if (write(fd, window[i]->Text().data(), cols[i].size) < 0)
                {
                    throw TableException("Can't add a record to table " + name);
                }
            }
        }
        service_info.last_record = service_info.current_record;
    }
    virtual void Delete (){}
    virtual void Drop ()
    {
        close(fd);
        remove(name.c_str());
    }
    virtual IField * GetField ( const string& Name )
    {
        for(int i = 0; i < cols.size(); i++)
        {
            if (cols[i].name == Name)
            {
                return window[i];
            }
        }
        throw TableException("Attempt to access a nonexistent column with name: " + Name + " in table: " + name);

    }
    virtual bool ReadFirst ()
    {
        lseek(fd, service_info.first_record, SEEK_SET);
        service_info.current_record = lseek(fd, 0L, SEEK_CUR);
        return true;
    }
    virtual bool ReadNext ()
    {
        if (service_info.current_record == service_info.last_record)
        {
            throw TableException("Attempt to access a nonexistent record in table: " + name);
        }
        lseek(fd, service_info.record_size, SEEK_CUR);
        service_info.current_record = lseek(fd, 0L, SEEK_CUR);
        return true;
    }
    virtual void Update ()
    {
         for (int i = 0; i < cols.size(); i++)
        {
            if (cols[i].type ==  TEXT)
            {
                if (write(fd, window[i]->Text().data(), cols[i].size) < 0)
                {
                    throw TableException("Can't update a record " + cols[i].name + " in table: " + name);
                }
            }
            else
            {
                if (write(fd, &(window[i]->Long()), sizeof(long)) < 0)
                {
                    throw TableException("Can't update a record " + cols[i].name + " in table: " + name); 
                }
            }
        }   
    }
};


#endif