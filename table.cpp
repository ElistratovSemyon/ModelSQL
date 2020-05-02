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
        Struct->AddText( "Surname", 20 );
        Struct->AddText( "City", 10 );
        Struct->AddLong( "Age" );
        
        ITable * Table = MyTable::Create ( Struct );
        
        Table->ReadFirst();
        Table->GetField("Name")->Text() = "Semen";
        Table->GetField("Surname")->Text() = "Elistratov";
        Table->GetField("City")->Text() = "Samara";
        Table->GetField("Age")->Long() = 10;
        
        Table->Add();
        
        Table->ReadNext();
        Table->GetField("Name")->Text() = "Irina";
        Table->GetField("Surname")->Text() = "Elistratova";
        Table->GetField("City")->Text() = "Samara";
        Table->GetField("Age")->Long() = 20;
        Table->Add();
        Table->ReadNext();
        Table->GetField("Name")->Text() = "Yuri";
        Table->GetField("Surname")->Text() = "Elistratov";
        Table->GetField("City")->Text() = "Samara";
        Table->GetField("Age")->Long() = 19;
        Table->Add();
        Table->ReadNext();
        Table->GetField("Name")->Text() = "Philipp";
        Table->GetField("Surname")->Text() = "Elistratov";
        Table->GetField("City")->Text() = "Samara";
        Table->GetField("Age")->Long() = 14;
        Table->Add();


        Table->ReadFirst();
    
        
        delete Table;
        delete Struct;
        
    }
    catch(TableException & e)
    {
        e.PrintMessage();
        
    }

    try
    {
        ITable * Table =  MyTable::Open ( "new_table" );
        for (Table->ReadFirst(); !Table->IsEnd(); Table->ReadNext())
        {
            //cout << "fd";
            cout << Table->GetField("Name")->Text() << " " << Table->GetField("Surname")->Text()<< " " 
            << Table->GetField("City")->Text() << " " << Table->GetField("Age")->Long() << endl;
        }
        delete Table;
        MyTable::Drop("new_table");
        
    }
    catch(TableException & e)
    {
        e.PrintMessage();
        
    }
    
    return 0;
}