#ifndef _DBMS_HPP_
#define _DBMS_HPP_
#include <vector>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#define MAX_LENGTH 4096
using namespace std;

// type of field
enum Type { TEXT, LONG };

// Exception class
class TableException {
public:
    string Message;
    virtual void PrintMessage ();
    TableException ();
    TableException ( const string& aMessage );
    TableException ( const TableException& exception );
    virtual ~ TableException ();
};

// base abstract field class
class IField {
public:
    virtual Type OfType () = 0;
    virtual string & Text () = 0;
    virtual long & Long () = 0;
    virtual ~IField();
};

// base abstract text field class
class ITextField: public IField {
public:
    virtual long & Long ();
};

// text field class
class MyTextField:public ITextField {
    string text;
public:
    MyTextField(const string & Text);
    MyTextField();
    // return type of field
    virtual Type OfType ();
    // return text from field
    virtual string & Text ();
    ~MyTextField();
};

// base abstract long field class
class ILongField: public IField {
public:
    virtual string & Text ();
};

// long field class
class MyLongField:public ILongField {
    long value;
public:
    MyLongField(long Value = 0);
    // return type of field
    virtual Type OfType ();
    // return long value from field
    virtual long & Long ();
    ~MyLongField();
};

// base class for creating table
class ITableStruct {
public:
    // add text column
    virtual ITableStruct * AddText ( const string& Name, int Length ) = 0;
    // add long column
    virtual ITableStruct * AddLong ( const string& Name ) = 0;
    // set table name
    virtual ITableStruct * SetName ( const string& Name ) = 0;
    // get columns vector
    virtual vector<struct Columns> & GetVector() = 0;
    // return table name
    virtual string & GetName() = 0;
    virtual ~ITableStruct();
};

// column structure
struct Columns
{
    string name;
    // size of field in this column
    int size;
    enum Type type;
    Columns operator=(Columns x);
};

class MyTableStruct:public ITableStruct {
protected: 
    vector<struct Columns> list;
    string name;
public:
    MyTableStruct();
    virtual ITableStruct * AddText ( const string& Name, int Length );
    virtual ITableStruct * AddLong ( const string& Name );
    virtual ITableStruct * SetName ( const string& Name );
    virtual vector<struct Columns> & GetVector();
    virtual string & GetName();
    ~MyTableStruct();
};

// base abstract table class
class ITable {
public:    
    // add or rewrite record in table
    virtual void Add () = 0;
    // delete record in table
    virtual void Delete () = 0;
    // return field of current record
    virtual IField * GetField ( const string& Name ) = 0;
    virtual IField * GetField ( int number ) = 0;
    virtual vector<Columns> GetColumns() = 0;
    // read first record
    virtual bool ReadFirst () = 0;
    virtual bool ReadNext () = 0;
    virtual bool ReadPrevious () = 0;
    // pointer to last (new) record
    virtual bool LastRecord () = 0;
    // is current record last
    virtual bool IsEnd() = 0;
    // return amount of cols
    virtual int AmountCols() = 0;
    virtual ~ ITable ();
};

// special structure of service table info
struct TableInfo
{
    long amount_record;
    long amount_cols;
    long first_record;
    long last_record;
    long current_record;
    long record_size;
};


class MyTable:public ITable {
    
    string name;
    vector<struct Columns> cols;
    int fd;
    struct TableInfo service_info;
    vector<IField *> window; // special vector which store current record
    bool record_read_flag;

private:    
    MyTable(int File, vector<struct Columns> & list, string Name, struct TableInfo info);
    // put current record to field
    void ReadRecord();
    // write record from window
    void WriteRecord();
    // modify service info in the start of the file
    void ModifyServiceInfo();
    // return table name
    string GetName();
    ~MyTable();
public:
    // static function to create structure of new table
    static ITableStruct * CreateTableStruct ();
    // static function to create table
    static ITable * Create ( ITableStruct * TableStruct );
    // delete table
    static void Drop ( const string & Name );
    // open table
    static ITable * Open ( const string & Name );
    virtual vector<Columns> GetColumns();
    virtual int AmountCols();
    virtual void Add ();
    virtual void Delete ();
    virtual IField * GetField ( const string& Name );
    virtual IField * GetField ( int number );
    virtual bool ReadFirst ();
    virtual bool ReadNext ();
    virtual bool ReadPrevious ();
    virtual bool LastRecord ();
    virtual bool IsEnd();
};


#endif