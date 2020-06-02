#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstdio>
#include "sock_wrap.hpp"

using namespace socket_shell;
#define PORT_NUM 8008 

int main(int argc, char* argv[])
{
    try
    {
        char host[64];
        // request network name
        if (gethostname(host, sizeof(host)) < 0)
        { 
            perror("Host name");
            exit (-1);
        }
        // create socket
        BaseSocket sock(host, PORT_NUM);

        // start work
        while(true)
        {
            std::string str;
            str = sock.GetString();
            
            if (str[0] == '#') // spec exit symbol
            {
                return 0;
            }
            else
            {
                // else print server message
                cout << str;
            }
            str.clear();
            // read user command
            std::getline(std::cin, str);
            sock.PutString(str);
            str.clear();
        }
    } 
    catch (Exception & e)
    {
        e.Report();
    }
    return 0;
}