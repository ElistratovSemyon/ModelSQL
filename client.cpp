#include <iostream>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include "sock_wrap.hpp" //см. приложение
using namespace std;
using namespace ModelSQL;
#define PORT_NUM 8004 // номер порта процесса-сервера
// В этом примере клиент и сервер выполняются на одном компьютере,
// но программа легко обобщается на случай разных компьютеров. Для
// этого можно, например, использовать сетевое имя не собственного
// компьютера, как ниже, а имя компьютера, на котором выполняется
// процесс-сервер
int main(int argc, char* argv[])
{
    try
    {
        char host[64];
        // запрос сетевого имени собственной ЭВМ
        if (gethostname(host, sizeof(host)) < 0) { // <
        // ошибка --- досрочно завершаем выполнение
            //cerr << GetLastError();
            perror("Host name");
            exit (-1);
        }
        //cout << endl << "TRABANT" << endl;
        // создаём сокет
        InClientSocket sock(host, PORT_NUM);
        // устанавливаем соединение
        sock.Connect();
        // отправляем серверу строку
        
        std::string str;
        //sock.PutChar(a);
        cin >> str;
        sock.PutString(str);
        // печатаем на экран ответ от сервера
        
        cout << sock.GetString() << endl;
        //cout << "Read from server: " << sock.GetChar() << endl;
        // продолжаем диалог с сервером, пока в этом есть необходимость
        // ...
    } 
    catch (Exception & e)
    {
        // ошибка --- выводим текст сообщения на экран
        e.Report();
    }
    return 0;
}