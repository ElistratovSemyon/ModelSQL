#include "dbms.hpp"
#include <vector>
#include <iostream>
using namespace std;

// TableException class
void TableException::PrintMessage ()
{
    cerr << Message << endl;
}

TableException::TableException () {}

TableException::TableException ( const string& aMessage ) {
    Message = aMessage; 
}

TableException::TableException ( const TableException& exception ) { 
    Message = exception.Message; 
}
TableException::~ TableException () {}


// IField class
IField::~IField(){}

// ITextField class
long & ITextField::Long ()
{ 
    throw TableException("Try to read/write long from text field."); 
}

// MyTextField class
MyTextField::MyTextField(const string & Text)
{
    text = Text;
}
MyTextField::MyTextField()
{
    text = "none";
}

Type MyTextField::OfType ()
{
    enum Type f_type = TEXT;//?
    return f_type;
}

string & MyTextField::Text ()
{
    return text;
}
MyTextField::~MyTextField()
{
    text.clear();
}


// ILongField class
string & ILongField::Text ()
{ 
    throw TableException("Try to read/write text from long field."); 
}

// MyLongField class
MyLongField::MyLongField(long Value )
{
    value = Value;
}
Type MyLongField::OfType ()
{
    enum Type f_type = LONG;
    return f_type;
}
long & MyLongField::Long ()
{
    return value;
}
MyLongField::~MyLongField(){}

// ITableStruct class
ITableStruct::~ITableStruct() {}
   
Columns Columns::operator=(Columns x)
{
    name = x.name;
    size = x.size;
    type = x.type;
    return *this;
}

// MyTableStruct class
MyTableStruct::MyTableStruct()
{
    list.clear();
    name = "none";
}

ITableStruct * MyTableStruct::AddText ( const string& Name, int Length )
{
    struct Columns tmp;
    tmp.name = Name;
    tmp.type = TEXT;
    tmp.size = Length;
    list.push_back(tmp);
    return this;
}

ITableStruct * MyTableStruct::AddLong ( const string& Name )
{
    struct Columns tmp;
    tmp.name = Name;
    
    tmp.type = LONG;
    tmp.size = sizeof(long);
    list.push_back(tmp);
    return this;
}

ITableStruct * MyTableStruct::SetName ( const string& Name )
{
    list.clear();
    name = Name;
    return this;
}

vector<struct Columns> & MyTableStruct::GetVector()
{
    return list;
}

string & MyTableStruct::GetName()
{
    return name;
}

MyTableStruct::~MyTableStruct()
{
    list.clear();
    name.clear();
}

ITable::~ ITable () {};

MyTable::MyTable(int File, vector<struct Columns> & list, string Name, struct TableInfo info)
{
    name = Name;
    record_read_flag = false;
    cols.clear();
    // copy columns to class field
    for (int i = 0; i < list.size(); i++)
    {
        cols.push_back(list[i]);
    }
    service_info = info;
    fd = File;
    window.clear();
    // create window
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
}

void MyTable::ReadRecord()
{
    for (int i = 0; i < cols.size(); i++)
    {
        if (cols[i].type == TEXT)
        {
            char buf[MAX_LENGTH] = {0};
            if (read(fd, buf, cols[i].size) < 0)
            {
                throw TableException("Can't read a record from table " + name);   
            }
            window[i]->Text() = buf;
        }
        else
        {
            if (read(fd, &(window[i]->Long()), sizeof(long)) < 0)
            {
                throw TableException("Can't read a record from table " + name);   
            }
        }
    }
    record_read_flag = true;
    lseek(fd, service_info.current_record, SEEK_SET); // update current record
}
void MyTable::WriteRecord()
{
    for(int i = 0; i < window.size(); i++)
    {
        
        if (window[i]->OfType() == TEXT)
        {   
            // check for text size
            if (window[i]->Text().size() > cols[i].size)
            {
                throw TableException("The row size exceeds the field size in table: " + name);
            }
            if (write(fd, window[i]->Text().data(), cols[i].size) < 0)
            {
                throw TableException("Can't add a record to table " + name);
            }          
        }
        else
        {
            if (write(fd, &(window[i]->Long()), sizeof(long)) < 0)
            {
                throw TableException("Can't add a record to table " + name);
            }
        }
    }
    lseek(fd, service_info.current_record, SEEK_SET); // update current record
}
void MyTable::ModifyServiceInfo()
{
    // to start
    lseek(fd, 0L, SEEK_SET);
    if (write(fd, &service_info, sizeof(service_info)) < 0)
    {
        throw TableException("Can't update service info in table: " + name);
    } 
    // to current record
    lseek(fd, service_info.current_record, SEEK_SET);
}
string MyTable::GetName()
{
    return name;
}
MyTable::~MyTable()
{
    close(fd);
    for (int i = 0; i < window.size(); i++)
    {
        delete window[i];
    }
    cols.clear();
    window.clear();
}

ITableStruct * MyTable::CreateTableStruct ()
{
    ITableStruct * new_struct = new MyTableStruct();
    return new_struct;
}

ITable * MyTable::Create ( ITableStruct * TableStruct )
{
    int fd;
    fd = open(TableStruct->GetName().c_str(), O_CREAT | O_RDWR | O_EXCL, 0666);
    if (fd == -1)
    {
        switch (errno)
        {
            case EEXIST:
                throw TableException("File already exist.");
                break;
            case EACCES:
                throw TableException("Permission denied.");
                break;
            default:
                throw TableException("Can't create file.");
                break;
        }
        
    }
    struct TableInfo tmp = {0};
    tmp.amount_cols = TableStruct->GetVector().size();
    long record_size = 0;
    // add blank service info
    if (write(fd, &tmp, sizeof(tmp)) < 0)
    {
        throw TableException("Can't add service info to table: " + TableStruct->GetName());
    }
    // add columns
    for (auto i: TableStruct->GetVector())
    {
        if (write(fd, &i, sizeof(i)) < 0)
        {
            throw TableException("Can't add columns names to table: " + TableStruct->GetName());
        }
        record_size += i.size;
    }
    // update service info
    tmp.first_record = lseek(fd, 0L, SEEK_CUR);
    tmp.last_record = lseek(fd, 0L, SEEK_CUR);
    tmp.current_record = lseek(fd, 0L, SEEK_CUR);
    tmp.record_size = record_size;
    lseek(fd, 0L, SEEK_SET);
    if (write(fd, &tmp, sizeof(tmp)) < 0)
    {
        throw TableException("Can't add service info to table: " + TableStruct->GetName());
    }

    lseek(fd, tmp.first_record, SEEK_SET);
    // call constructor
    ITable * new_table = new MyTable(fd, TableStruct->GetVector(), TableStruct->GetName(), tmp);
    return new_table;
}

void MyTable::Drop ( const string & Name )
{
    remove(Name.c_str());
}

ITable * MyTable::Open ( const string & Name )
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
    // read service info
    if (read(fd, &info, sizeof(info)) < 0)
    {
        throw TableException("Can't read service info from table " + Name);
    }
    // read columns
    for(int i = 0; i < info.amount_cols; i++)
    {
        struct Columns tmp;
        if (read(fd, &tmp, sizeof(tmp)) < 0)
        {
            throw TableException("Can't read columns names from table " + Name);
        }
        list.push_back(tmp);
    }
    info.current_record = lseek(fd, 0L, SEEK_CUR);
    ITable * new_table = new MyTable(fd, list, Name, info);
    return new_table;           
}
int MyTable::AmountCols()
{
    return service_info.amount_cols;
}
void MyTable::Add ()
{
    WriteRecord();
    // update last record
    if (service_info.current_record == service_info.last_record)
    {
        service_info.last_record = lseek(fd, service_info.record_size, SEEK_CUR);
        ModifyServiceInfo(); // update service info
    }
}
void MyTable::Delete ()
{
    if (service_info.current_record == service_info.last_record)
    {
        // nothing to delete
    }
    else if ((service_info.current_record + service_info.record_size) == service_info.last_record)
    {
        // delete record
        ftruncate(fd, service_info.current_record);
        
        service_info.last_record = service_info.current_record;
        ModifyServiceInfo(); 
    }
    else
    {
        long current_offset_buf = service_info.current_record;
        // swap all records
        for (; !IsEnd(); ReadNext())
        {
            ReadNext();
            GetField(cols[0].name);
            ReadPrevious();
            WriteRecord();
        }   
        // pointer on new record
        ReadPrevious();
        service_info.last_record = service_info.current_record;
        // delete record
        ftruncate(fd, service_info.last_record);
        service_info.current_record = current_offset_buf;
        lseek(fd, service_info.current_record, SEEK_SET);
        // update service info
        ModifyServiceInfo();
    }
}
IField * MyTable::GetField ( const string& Name )
{
    // if window ain't store current record, we read it 
    if (record_read_flag == false) 
    {
        ReadRecord();
    }
    for(int i = 0; i < cols.size(); i++)
    {
        if (cols[i].name == Name)
        {
            return window[i];
        }
    }
    throw TableException("Attempt to access a nonexistent column with name: " + Name + " in table: " + name);
}
IField * MyTable::GetField ( int number )
{
    // if window ain't store current record, we read it 
    if (record_read_flag == false )
    {
        ReadRecord();
    }
    if (number > window.size())
    {
        throw TableException("Attempt to access a nonexistent column in table: " + name);
    }
    return window[number];
}
bool MyTable::ReadFirst()
{
    record_read_flag = false;
    lseek(fd, service_info.first_record, SEEK_SET);
    service_info.current_record = lseek(fd, 0L, SEEK_CUR);
    return true;
}
bool MyTable::ReadNext ()
{
    if (service_info.current_record == service_info.last_record)
    {
        throw TableException("Attempt to access a nonexistent record in table: " + name);
    }
    lseek(fd, service_info.record_size, SEEK_CUR);
    service_info.current_record = lseek(fd, 0L, SEEK_CUR);
    record_read_flag = false;
    return true;
}
bool MyTable::ReadPrevious ()
{
    if (service_info.current_record == service_info.first_record)
    {
        throw TableException("Attempt to access a 0 record in table: " + name);
    }
    lseek(fd, - service_info.record_size, SEEK_CUR);
    service_info.current_record = lseek(fd, 0L, SEEK_CUR);
    record_read_flag = false;
    return true;
}
bool MyTable::LastRecord ()
{
    lseek(fd, service_info.last_record, SEEK_SET);
    service_info.current_record = service_info.last_record;
    record_read_flag = false;
    return true;
}
bool MyTable::IsEnd()
{
    return service_info.current_record == service_info.last_record;
}

vector<Columns> MyTable::GetColumns()
{
    return cols;
}
