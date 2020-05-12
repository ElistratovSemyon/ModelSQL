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

namespace ModelSQL {
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

 

    // Base abstract class for networks address
    class SocketAddress {
    protected:
        struct sockaddr_in * m_pAddr;
    public:
        SocketAddress ();
        virtual ~ SocketAddress ();
        // get size of structure
        virtual int GetLength() = 0;
        // clone structure (copy)
        virtual SocketAddress * Clone() = 0;
        // transform to the base type
        operator struct sockaddr * ();
    };

    // InSocketAddress -AF_INET socket address
    class InSocketAddress : public SocketAddress {
    public:
        InSocketAddress (const char * HostName, short PortNum);
        InSocketAddress (const InSocketAddress & x);
        ~ InSocketAddress (); 
        SocketAddress * Clone();
        int GetLength();        
    };


    // Base Socket class
    class BaseSocket {
    public:
        
        explicit BaseSocket (int sd = -1, SocketAddress * pAddr = NULL);
        
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
    protected:
        int m_Socket;
        SocketAddress * m_pAddr;
    };


    // ClientSocket - base client socket class
    class ClientSocket: public BaseSocket {
    public:
        // call connection socket function
        void Connect();
    };


    // ServerSocket - base server socket class
    class ServerSocket: public BaseSocket {
    public:
        BaseSocket * Accept();
    protected:
        // call bing socket function       
        void Bind();
        // call listen socket function
        void Listen(int BackLog);
        // virtual function to communicate with client
        virtual void OnAccept (BaseSocket * pConn);
    };


    // InClientSocket - AF_INET client socket
    class InClientSocket: public ClientSocket {
    public:
        InClientSocket(const char * HostName, short PortNum);
    };

    // InServerSocket - AF_INET server socket
    class InServerSocket: public ServerSocket {
    public:
        InServerSocket(short PortNum);
    };
};

#endif
