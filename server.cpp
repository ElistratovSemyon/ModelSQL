#include <iostream>
#include "sock_wrap.hpp" //см. приложение
#include "dbms.hpp"
#include "inter.hpp"
using namespace std;
using namespace ModelSQL;
#define PORT_NUM 8006 // номер порта процесса-сервера

class MyServerSocket : public InServerSocket {
public:
    MyServerSocket () : InServerSocket (PORT_NUM) {}
protected:
    void OnAccept (BaseSocket * pConn)
    {
        try
        {
           
            pConn->PutString("Ready");
            parser::init(pConn);
            
            while (parser::end_flag == false)
            {
                
                
                parser::Start();
            }
            
        }
        catch(const char * s)
        {
            std::cerr << s << '\n';
        }
        delete pConn;
    }
};

int main(int argc, char* argv[])
{
    try {
        // создаём серверный сокет
        MyServerSocket sock;
        for (int i = 0; i < 1; i++)
        // слушаем запросы на соединение
            sock.Accept();
        } catch (Exception& e) {
        // ошибка --- выводим текст сообщения на экран
        e.Report();
    }
    return 0;
}