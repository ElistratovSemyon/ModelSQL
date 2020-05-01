#include <iostream>
#include "sock_wrap.hpp" //см. приложение
using namespace std;
using namespace ModelSQL;
#define PORT_NUM 8004 // номер порта процесса-сервера

class MyServerSocket : public InServerSocket {
public:
    MyServerSocket () : InServerSocket (PORT_NUM) {}
protected:
    void OnAccept (BaseSocket * pConn)
    {
        // установлено соединение с клиентом, читаем сообщение
        cout << "Read from client: " <<  pConn->GetString() << endl;
        //cout << "Read from client: " << (char) pConn->GetChar() << endl;
        
        //for (int i = 0; i < 10; i++)
        //    cout << (char *) pConn->GetChar();
        // отправляем ответ
        
        pConn->PutString("OK");
        
        // продолжаем диалог с клиентом, пока в этом есть необходимость
        // ...
        
        delete pConn;
    }
};

int main(int argc, char* argv[])
{
    try {
        // создаём серверный сокет
        MyServerSocket sock;
        for (int i = 0; i < 3; i++)
        // слушаем запросы на соединение
            sock.Accept();
        } catch (Exception& e) {
        // ошибка --- выводим текст сообщения на экран
        e.Report();
    }
    return 0;
}