#include <iostream>
#include "dbms.hpp"
using namespace std;
int main()
{
    try
    {    
        ITableStruct * Struct = MyTable::CreateTableStruct ();
        Struct->SetName("new_table");
        Struct->AddText( "Name", 10 );
        Struct->AddLong( "Age" );

        ITable * Table = MyTable::Create ( Struct );
        IField * Field1 = Table->GetField("Name");
        IField * Field2 = Table->GetField("Age");
        Table->ReadFirst();
        Field1->Text() = "123456";
        Field2->Long() = 2536;
        Table->Add();
        
        delete Table;

        
        ITable * Table2 = MyTable::Open ( "new_table" );
        Table2->ReadFirst();
        IField * Field3 = Table2->GetField("Name");
        cout << Field3->Text() << endl;
        
        delete Table2;
        
        cout << Field1->Text() << endl;
        MyTable::Drop("new_table");
    }
    catch(TableException & e)
    {
        e.PrintMessage();
    }
    return 0;
}