#include "sock_wrap.hpp"
using namespace std;

socket_shell::Exception::Exception (int errcode ) : m_ErrCode(errcode) {}
void  socket_shell::Exception::Report ()
{
    cout << "Error with number: " << m_ErrCode << endl;
}
       
socket_shell::SocketException::SocketException (SocketExceptionCode errcode): Exception(errcode){}
          
socket_shell::BaseSocket::BaseSocket (int sd, sockaddr_in * pAddr): m_Socket(sd), m_pAddr(pAddr) {}
socket_shell::BaseSocket::~ BaseSocket()
{
    close(m_Socket);
    //shutdown(m_Socket, RDWR);
    delete m_pAddr;
}
void socket_shell::BaseSocket::Write(void * buf, int len)
{
    if (send(m_Socket, buf, len, 0) < 0)
    {
        perror(0);
        throw SocketException(SocketException::ESE_SOCKSEND);
    }
}
void socket_shell::BaseSocket::PutChar(int c)
{
    if (send(m_Socket, &c, sizeof(c), 0) < 0)
    {
        perror(0);
        throw SocketException(SocketException::ESE_SOCKSEND);
    }
}
void socket_shell::BaseSocket::PutString(const char * str)
{
    if (send(m_Socket, str, strlen(str), 0) < 0)
    {
        perror(0);
        throw SocketException(SocketException::ESE_SOCKSEND);
    }
    
}
void socket_shell::BaseSocket::PutString(const std::string& s)
{
    if (send(m_Socket, s.data(), s.size(), 0) < 0)
    {
        perror(0);
        throw SocketException(SocketException::ESE_SOCKSEND);
    }
}
int socket_shell::BaseSocket::Read (void * buf, int len)
{
    int res;
    if ((res = recv(m_Socket, buf, len, 0)) < 0)
    {
        perror(0);
        throw SocketException(SocketException::ESE_SOCKSEND);
    }
    return res;
}
int socket_shell::BaseSocket::GetChar()
{
    int c = 0;
    if (recv(m_Socket, &c, sizeof(char), 0) < 0)
    {
        perror(0);
        throw SocketException(SocketException::ESE_SOCKRECV);
    }
    return c;
}

std::string socket_shell::BaseSocket::GetString()
{
    std::vector<char> buffer(MAX_BUF_LENGTH, 0);
    std::string rcv;  
    // read until 0 bytes received 
    int bytes_recv = 0;
    do {
        bytes_recv = recv(m_Socket, &buffer[0], buffer.size(), 0);
        // append string from buffer.
        if ( bytes_recv == -1 )
        { 
            perror(0);
            throw SocketException(SocketException::ESE_SOCKRECV);
        }
        else
        {
            rcv.append( buffer.cbegin(), buffer.cend() );
        }
    } while ( bytes_recv == 0);
    
    
    return rcv;
}


int socket_shell::BaseSocket::GetSockDescriptor()
{
    return m_Socket;
}

struct sockaddr_in * socket_shell::BaseSocket::ConvertAddress(const char * HostName, short PortNum)
{
    struct sockaddr_in * sin = new sockaddr_in();
    sin->sin_family = AF_INET;
    sin->sin_port = htons (PortNum);
    struct hostent *hp;

    // get host
    if ((hp = gethostbyname (HostName)) == NULL) {
        perror(0);
        throw SocketException(SocketException::ESE_SOCKHOSTNAME);
    }
    // copy address to structure 
    memcpy ( &(sin->sin_addr), hp->h_addr, hp->h_length);
    m_pAddr = sin;
    return sin;
}


void socket_shell::ClientSocket::Connect()
{
    if ( connect( m_Socket, (struct sockaddr *) m_pAddr, sizeof(*m_pAddr)) < 0 ) 
    {
        perror(0);
        throw SocketException(SocketException::ESE_SOCKCONN);
    }
}
   

socket_shell::BaseSocket * socket_shell::ServerSocket::Accept()
{
    int d;
    socklen_t size = sizeof(*m_pAddr);
    if((d = accept ( m_Socket, (struct sockaddr *) m_pAddr, &size)) < 0){
        perror (0);
        throw SocketException(SocketException::ESE_SOCKACCEPT);
    }
    sockaddr_in * new_addr = new struct sockaddr_in;
    *new_addr = *m_pAddr;
    BaseSocket * s = new socket_shell::BaseSocket(d, new_addr);
    
    return s;
}

void socket_shell::ServerSocket::Bind()
{
    // in c++11 appeared new function bind, so must specified global namespace
    int a = ::bind(  m_Socket, (struct sockaddr *) m_pAddr, sizeof(*m_pAddr));
    if ( a < 0 ){
        perror(0);
        throw SocketException(SocketException::ESE_SOCKBIND);
    }       
}

void socket_shell::ServerSocket::Listen(int BackLog)
{
    if ( listen ( m_Socket, BackLog) < 0 ) {
        perror(0);
        throw SocketException(SocketException::ESE_SOCKLISTEN);
    }
}

socket_shell::ClientSocket::ClientSocket(const char * HostName, short PortNum)
{
    m_pAddr = ConvertAddress(HostName, PortNum);
    if ((m_Socket = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    {
        throw SocketException(SocketException::ESE_SOCKCREATE);
    }
    Connect();
}
        
socket_shell::ServerSocket::ServerSocket(short PortNum)
{
    char HostName[64];
    gethostname (HostName, sizeof (HostName));
    m_pAddr = ConvertAddress(HostName, PortNum);
    if ((m_Socket = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
        throw SocketException(SocketException::ESE_SOCKCREATE);
    }
    Bind();
    Listen(BACK_LOG);
}
