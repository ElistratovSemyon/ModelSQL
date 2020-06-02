#ifndef __SOCK_WRAP_HPP__
#define __SOCK_WRAP_HPP__
#include <string>
#include <vector>
#include <iostream>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <stdlib.h>
#include <unistd.h>
#define BACK_LOG 5
#define MAX_BUF_LENGTH 4096

using namespace std;

namespace socket_shell {
    // Base Exception class
    class Exception {
    protected:
        int m_ErrCode;
    public:
        Exception (int errcode );
        // print error
        void Report ();
    };

    // Socket Exception class
    class SocketException : public Exception {
        static char * m_Message[];
    public:
        enum SocketExceptionCode {
            ESE_SOCKCREATE,     // create error
            ESE_SOCKCONN,       // connect error
            ESE_SOCKHOSTNAME,   // HostName gen error
            ESE_SOCKSEND,       // send error
            ESE_SOCKRECV,       // recv error
            ESE_SOCKBIND,       // bind error
            ESE_SOCKLISTEN,     // listen error
            ESE_SOCKACCEPT,     // accept error
        };

        SocketException (SocketExceptionCode errcode);
    };

 
    // Base Socket class
    class BaseSocket 
    {
        int m_Socket;
        sockaddr_in m_pAddr;
    public:
        BaseSocket();
        BaseSocket (int sd, sockaddr_in pAddr);
        BaseSocket (int sd);
        BaseSocket(const char * HostName, short PortNum);
        virtual ~ BaseSocket();
        // write to socket
        void Write(void * buf, int len);
        void PutChar(int c);
        void PutString(const char * str);
        void PutString(const std::string& s);
        // read to specified buffer
        int Read (void * buf, int len);
        int GetChar();
        // read the whole message
        std::string GetString();
        // return socket descriptor
        int GetSockDescriptor();
        // call connection socket function
        void Connect(const char * HostName, short PortNum);
private:
        // auto connection in socket constuctor
        void Connect();
    };

    // ServerSocket - server socket class
    // socket for connecting
    class ServerSocket
    {
        int m_Socket;
        sockaddr_in m_pAddr;
    public:
        ServerSocket(short PortNum);
        ServerSocket(const char * HostName, short PortNum);
        // create socket for messaging
        BaseSocket Accept();
    private:
        // call bind socket function       
        void Bind();
        // call listen socket function
        void Listen(int BackLog);
    };

    struct sockaddr_in ConvertAddress(const char * HostName, short PortNum);

};

#endif
