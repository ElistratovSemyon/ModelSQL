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


enum Type { TEXT, LONG };
class Xception {
public:
    string Message;
    virtual void PrintMessage ()
    {
        cerr << Message << endl;
    }
    Xception () {}
    Xception ( const string& aMessage ) {
        Message = aMessage; 
    }
    Xception ( const Xception& xception ) { 
        Message = xception.Message; 
    }
    virtual ~ Xception () {}
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
        throw Xception (); 
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
        throw Xception (); 
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
    
};




class MyTable:public ITable {
    
    string name;
    vector<struct Columns> cols;
    int fd;
    long line_size;
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
        
        write(fd, &tmp, sizeof(tmp));
        for (auto i: TableStruct->GetVector())
        {
            write(fd, &i, sizeof(i));
        }
        tmp.first_record = lseek(fd, 0L, SEEK_CUR);
        tmp.last_record = lseek(fd, 0L, SEEK_CUR);
        tmp.current_record = lseek(fd, 0L, SEEK_CUR);
        lseek(fd, 0L, SEEK_SET);
        write(fd, &tmp, sizeof(tmp));
        
        
        
        
        
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
            throw 1;
        }
        
        struct TableInfo info;
        read(fd, &info, sizeof(info));
        list.reserve(info.amount_cols);
        for(int i = 0; i < list.size(); i++)
        {
            struct Columns tmp;
            read(fd, &tmp, sizeof(tmp));
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
                write(fd, window[i]->Text().data(), cols[i].size);
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
        return 0;

    }
    virtual bool ReadFirst ()
    {
        lseek(fd, service_info.first_record, SEEK_SET);
        for (int i = 0; i < cols.size(); i++)
        {
            if (cols[i].type == TEXT)
            {
                read(fd, window[i]->Text().data(), cols[i].size);
            }
            else
            {
                read(fd, &(window[i]->Long()), sizeof(long));
            }
        }
        service_info.current_record = lseek(fd, 0L, SEEK_CUR);
        return true;
    }
    virtual bool ReadNext ()
    {
        for (int i = 0; i < cols.size(); i++)
        {
            if (cols[i].type == TEXT)
            {
                read(fd, window[i]->Text().data(), cols[i].size);
            }
            else
            {
                read(fd, &(window[i]->Long()), sizeof(long));
            }
        }
        service_info.current_record = lseek(fd, 0L, SEEK_CUR);
        return true;
    }
    virtual void Update ()
    {
         for (int i = 0; i < cols.size(); i++)
        {
            if (cols[i].type ==  TEXT)
            {
                write(fd, window[i]->Text().data(), cols[i].size);
            }
            else
            {
                write(fd, &(window[i]->Long()), sizeof(long));
            }
        }   
    }
};


#endif