#include <iostream>

#include "sock_wrap.hpp"
#include "dbms.hpp"
#include "inter.hpp"

#define PORT_NUM 8008

extern bool parser::answer_flag;
extern bool parser::end_flag;
extern std::string parser::server_answer;

class MyServerSocket : public ModelSQL::InServerSocket {
public:
    MyServerSocket () : InServerSocket (PORT_NUM) {}
protected:
    void OnAccept (BaseSocket * pConn)
    {
        // print start string
        pConn->PutString(">> ");
        // until client send EXIT or turn off
        while(parser::end_flag == false)
        {
            try
            {
                while (true)
                {
                    
                    std::string sentence =  pConn->GetString();
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
                std::string str = s;
                pConn->PutString(str + "\n" + ">> ");
            }
            catch(...)
            {
                pConn->PutString(std::string{"ERROR."} + "\n" + ">> ");
            }
        }
        
        delete pConn;
    }
};

int main(int argc, char* argv[])
{
    try
    {
        // create server socket
        MyServerSocket sock;
        // start work with clients
        sock.Accept();
    }
    catch (ModelSQL::Exception & e)
    {
        e.Report();
    }
    return 0;
}