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
#define MAX_BUF_LENGTH 256

using namespace std;

namespace ModelSQL {
    // Exception --- класс исключений, генерируемых библиотекой
    class Exception {
    protected:
        int m_ErrCode;
    public:
        Exception (int errcode ) : m_ErrCode(errcode) {}
        void Report ()
        {
            cout << "Error with number: " << m_ErrCode << endl;
        }
        //virtual std::string GetMessage() = 0;
    };

    // SocketException --- класс исключений
    class SocketException : public Exception {
        static char * m_Message[];
        /*
        static char * m_Message[] = {
            "Success exception",
            "Create exception",
            "Connect exception",
            "Illegal socket",
            "Hostname exception",
            "Send exception",
            "Recv exception",
            "Bind exception",
            "Listen exception",
            "Accept exception"
        };
        */
    public:
        enum SocketExceptionCode {
            ESE_SUCCESS, // 0 
            ESE_SOCKCREATE,
            ESE_SOCKCONN, // 2
            ESE_SOCKILLEGAL,
            ESE_SOCKHOSTNAME, // 4
            ESE_SOCKSEND,
            ESE_SOCKRECV, // 6
            ESE_SOCKBIND,
            ESE_SOCKLISTEN, // 8
            ESE_SOCKACCEPT,
        };

        SocketException (SocketExceptionCode errcode): Exception(errcode){}
        /*
        std::string GetMessage()
        {
            return (std::string) m_Message[m_ErrCode];
        }
        */
    };

 

    // SocketAddress --- базовый абстрактный класс для представления
    // сетевых адресов
    // сразу добавил sockaddr_in ????
    class SocketAddress {
    protected:
        struct sockaddr_in * m_pAddr;
    public:
        SocketAddress () : m_pAddr(NULL) {}
        virtual ~ SocketAddress () {}
        virtual int GetLength() = 0;
        virtual SocketAddress * Clone() = 0;
        operator struct sockaddr * ()
        {
            return (struct sockaddr *) m_pAddr;
        }
    };

    // InSocketAddress --- представление адреса семейства AF_INET
    class InSocketAddress : public SocketAddress {
    public:
        InSocketAddress (const char * HostName, short PortNum)
        {
            struct sockaddr_in * sin = new sockaddr_in();
            sin->sin_family = AF_INET;
            sin->sin_port = htons (PortNum);
            struct hostent *hp;

           
            if ((hp = gethostbyname (HostName)) == NULL) {
                perror(0);
                throw SocketException(SocketException::ESE_SOCKHOSTNAME);
            }
            memcpy ( &(sin->sin_addr), hp->h_addr, hp->h_length);
            m_pAddr = sin;
        }
        
        InSocketAddress (const InSocketAddress & x)
        {
            m_pAddr = new sockaddr_in();
            m_pAddr->sin_family = AF_INET;
            m_pAddr->sin_port =  x.m_pAddr->sin_port;
            memcpy ((void *) (&(m_pAddr->sin_addr)), (const void *) &((x.m_pAddr)->sin_addr), (size_t) sizeof(x.m_pAddr->sin_addr));
        }
        ~ InSocketAddress ()
        {
            delete m_pAddr;
        }
        int GetLength ()
        {
            return sizeof(*m_pAddr);
        }// ???????
        
        
        SocketAddress * Clone()
        {
            InSocketAddress * tmp = new InSocketAddress(*this);
            return tmp;
        }

        
    };


    // BaseSocket --- базовый класс для сокетов
    class BaseSocket {
    public:
        explicit BaseSocket (int sd = -1, SocketAddress * pAddr = NULL): m_Socket(sd), m_pAddr(pAddr) {}
        virtual ~ BaseSocket()
        {
            close(m_Socket);
            //shutdown(m_Socket, RDWR);
            delete m_pAddr;
        }
        void Write(void * buf, int len)
        {
            if (send(m_Socket, buf, len, 0) < 0)
            {
                perror(0);
                throw SocketException(SocketException::ESE_SOCKSEND);
            }
        }
        void PutChar(int c)
        {
            if (send(m_Socket, &c, sizeof(c), 0) < 0)
            {
                perror(0);
                throw SocketException(SocketException::ESE_SOCKSEND);
            }
        }
        void PutString(const char * str)
        {
            if (send(m_Socket, str, strlen(str), 0) < 0)
            {
                perror(0);
                throw SocketException(SocketException::ESE_SOCKSEND);
            }
            
        }
        void PutString(const std::string& s)
        {
            if (send(m_Socket, s.data(), s.size(), 0) < 0)
            {
                perror(0);
                throw SocketException(SocketException::ESE_SOCKSEND);
            }
        }
        int Read (void * buf, int len)
        {
            int res;
            if ((res = recv(m_Socket, buf, len, 0)) < 0)
            {
                perror(0);
                throw SocketException(SocketException::ESE_SOCKSEND);
            }
            return res; // ???????
        }
        int GetChar()
        {
            int c = 0;
            if (recv(m_Socket, &c, sizeof(char), 0) < 0)
            {
                perror(0);
                throw SocketException(SocketException::ESE_SOCKSEND);
            }
            return c;
        }
        
        std::string GetString()
        {
            std::vector<char> buffer(MAX_BUF_LENGTH, 0);
            std::string rcv;   
            int bytesReceived = 0;
            do {
                bytesReceived = recv(m_Socket, &buffer[0], buffer.size(), 0);
                // append string from buffer.
                if ( bytesReceived == -1 ) { 
                    // error 
                } else {
                    rcv.append( buffer.cbegin(), buffer.cend() );
                }
            } while ( bytesReceived == MAX_BUF_LENGTH);
            
           
            return rcv;
        }
        

        int GetSockDescriptor()
        {
            return m_Socket;
        }
    protected:
        int m_Socket;
        SocketAddress * m_pAddr;
        //void CheckSocket();
    };


    // ClientSocket --- базовый класс для клиентских сокетов
    class ClientSocket: public BaseSocket {
    public:
        void Connect()
        {
            socklen_t len = m_pAddr->GetLength();
            if ( connect( m_Socket, (struct sockaddr *) *m_pAddr, len) < 0 ) 
            {
                perror(0);
                throw SocketException(SocketException::ESE_SOCKCONN);
            }
        }
    };


    // ServerSocket --- базовый класс для серверных сокетов
    class ServerSocket: public BaseSocket {
    public:
        BaseSocket * Accept()
        {
            
            //sockaddr * fromsin = new InSocketAddress();
            socklen_t len = m_pAddr->GetLength();
            int d;
            if((d = accept ( m_Socket, (struct sockaddr *) *m_pAddr, &len)) < 0){
                perror (0);
                throw SocketException(SocketException::ESE_SOCKACCEPT);
            }
            BaseSocket * s = new BaseSocket(d, m_pAddr->Clone());
            OnAccept(s);
            
            return s;

        }
    protected:
        void Bind()
        {
            int a = ::bind(  m_Socket, (struct sockaddr *) *m_pAddr, m_pAddr->GetLength());
            if ( a < 0 ){
                perror(0);
                throw SocketException(SocketException::ESE_SOCKBIND);
            }       
        }
        void Listen(int BackLog)
        {
            if ( listen ( m_Socket, BackLog) < 0 ) {
                perror(0);
                throw SocketException(SocketException::ESE_SOCKLISTEN);
            }
        }
        virtual void OnAccept (BaseSocket * pConn) {}
    };


    // InClientSocket --- представление клиентского сокета семейства
    // AF_INET


    class InClientSocket: public ClientSocket {
    public:
        InClientSocket(const char * HostName, short PortNum)
        {
            m_pAddr = new InSocketAddress(HostName, PortNum);
            if ((m_Socket = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
                throw SocketException(SocketException::ESE_SOCKCREATE);
            }
            

        }
    };

    // InServerSocket --- представление серверного сокета семейства
    // AF_INET
    class InServerSocket: public ServerSocket {
    public:
        InServerSocket(short PortNum)
        {
            char HostName[64];
            gethostname (HostName, sizeof (HostName));
            m_pAddr = new InSocketAddress(HostName, PortNum);
            if ((m_Socket = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
                throw SocketException(SocketException::ESE_SOCKCREATE);
            }
            Bind();
            Listen(BACK_LOG);
        }
    };
}; // конец namespace ModelSQL
#endif
