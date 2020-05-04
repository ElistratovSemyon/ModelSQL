#include <iostream>
#include "sock_wrap.hpp" //см. приложение
#include "dbms.hpp"
#include "inter.hpp"
using namespace std;
using namespace ModelSQL;
#define PORT_NUM 8008 // номер порта процесса-сервера

class MyServerSocket : public InServerSocket {
public:
    MyServerSocket () : InServerSocket (PORT_NUM) {}
protected:
    void OnAccept (BaseSocket * pConn)
    {
        pConn->PutString(">> ");
        while(parser::end_flag == false)
        {
            try
            {
                while (true)
                {
                    
                    string sentence = pConn->GetString();
                    if (sentence.size() < 4)
                    {
                        throw parser::SentenceException("Incorrectly clause");
                    }
                    parser::init(sentence);
                    parser::Start();
                    if (parser::end_flag == true)
                    {
                        pConn->PutString("#");
                        sleep(1);
                        break;
                    }
                    //cout << "GET";
                    if (parser::answer_flag == true)
                    {
                        pConn->PutString(parser::server_answer + ">> ");
                    }
                    else
                    {
                        pConn->PutString(">> ");
                    }
                }
                
            }
            catch(parser::SentenceException & e)
            {
                pConn->PutString(e.Message + "\n" + ">> ");
            }
            catch(TableException & e)
            {
                pConn->PutString(e.Message + "\n" + ">> ");
            }
            catch(const char * s)
            {
                string str = s;
                pConn->PutString(str + "\n" + ">> ");
            }
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