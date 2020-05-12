#include "sock_wrap.hpp"
using namespace std;



ModelSQL::Exception::Exception (int errcode ) : m_ErrCode(errcode) {}
void  ModelSQL::Exception::Report ()
{
    cout << "Error with number: " << m_ErrCode << endl;
}
       

ModelSQL::SocketException::SocketException (SocketExceptionCode errcode): Exception(errcode){}
          
ModelSQL::SocketAddress::SocketAddress () : m_pAddr(NULL) {} // initialize structure
ModelSQL::SocketAddress::~SocketAddress () {}

ModelSQL::SocketAddress::operator struct sockaddr * ()
{
    return (struct sockaddr *) m_pAddr;
}


    
ModelSQL::InSocketAddress::InSocketAddress (const char * HostName, short PortNum)
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
}

ModelSQL::InSocketAddress::InSocketAddress (const InSocketAddress & x)
{
    m_pAddr = new sockaddr_in();
    m_pAddr->sin_family = AF_INET;
    m_pAddr->sin_port =  x.m_pAddr->sin_port;
    // copy address to structure
    memcpy ((void *) (&(m_pAddr->sin_addr)), (const void *) &((x.m_pAddr)->sin_addr),  
        (size_t) sizeof(x.m_pAddr->sin_addr));
}
ModelSQL::InSocketAddress::~InSocketAddress ()
{
    delete m_pAddr;
}
int ModelSQL::InSocketAddress::GetLength ()
{
    return sizeof(*m_pAddr);
}


ModelSQL::InSocketAddress::SocketAddress * ModelSQL::InSocketAddress::Clone()
{
    ModelSQL::InSocketAddress * tmp = new ModelSQL::InSocketAddress(*this);
    return tmp;
}


    
ModelSQL::BaseSocket::BaseSocket (int sd, SocketAddress * pAddr): m_Socket(sd), m_pAddr(pAddr) {}
ModelSQL::BaseSocket::~ BaseSocket()
{
    close(m_Socket);
    //shutdown(m_Socket, RDWR);
    delete m_pAddr;
}
void ModelSQL::BaseSocket::Write(void * buf, int len)
{
    if (send(m_Socket, buf, len, 0) < 0)
    {
        perror(0);
        throw SocketException(SocketException::ESE_SOCKSEND);
    }
}
void ModelSQL::BaseSocket::PutChar(int c)
{
    if (send(m_Socket, &c, sizeof(c), 0) < 0)
    {
        perror(0);
        throw SocketException(SocketException::ESE_SOCKSEND);
    }
}
void ModelSQL::BaseSocket::PutString(const char * str)
{
    if (send(m_Socket, str, strlen(str), 0) < 0)
    {
        perror(0);
        throw SocketException(SocketException::ESE_SOCKSEND);
    }
    
}
void ModelSQL::BaseSocket::PutString(const std::string& s)
{
    if (send(m_Socket, s.data(), s.size(), 0) < 0)
    {
        perror(0);
        throw SocketException(SocketException::ESE_SOCKSEND);
    }
}
int ModelSQL::BaseSocket::Read (void * buf, int len)
{
    int res;
    if ((res = recv(m_Socket, buf, len, 0)) < 0)
    {
        perror(0);
        throw SocketException(SocketException::ESE_SOCKSEND);
    }
    return res;
}
int ModelSQL::BaseSocket::GetChar()
{
    int c = 0;
    if (recv(m_Socket, &c, sizeof(char), 0) < 0)
    {
        perror(0);
        throw SocketException(SocketException::ESE_SOCKRECV);
    }
    return c;
}

std::string ModelSQL::BaseSocket::GetString()
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


int ModelSQL::BaseSocket::GetSockDescriptor()
{
    return m_Socket;
}
    


    
void ModelSQL::ClientSocket::Connect()
{
    socklen_t len = m_pAddr->GetLength();
    if ( connect( m_Socket, (struct sockaddr *) *m_pAddr, len) < 0 ) 
    {
        perror(0);
        throw SocketException(SocketException::ESE_SOCKCONN);
    }
}
   


ModelSQL::BaseSocket * ModelSQL::ServerSocket::Accept()
{
    //sockaddr * fromsin = new InSocketAddress();
    socklen_t len = m_pAddr->GetLength();
    int d;
    if((d = accept ( m_Socket, (struct sockaddr *) *m_pAddr, &len)) < 0){
        perror (0);
        throw SocketException(SocketException::ESE_SOCKACCEPT);
    }
    BaseSocket * s = new ModelSQL::BaseSocket(d, m_pAddr->Clone());
    OnAccept(s);
    
    return s;
}

void ModelSQL::ServerSocket::Bind()
{
    // in c++11 appeared new function bind, so must specified global namespace
    int a = ::bind(  m_Socket, (struct sockaddr *) *m_pAddr, m_pAddr->GetLength());
    if ( a < 0 ){
        perror(0);
        throw SocketException(SocketException::ESE_SOCKBIND);
    }       
}

void ModelSQL::ServerSocket::Listen(int BackLog)
{
    if ( listen ( m_Socket, BackLog) < 0 ) {
        perror(0);
        throw SocketException(SocketException::ESE_SOCKLISTEN);
    }
}

void ModelSQL::ServerSocket::OnAccept (BaseSocket * pConn) {}
   
ModelSQL::InClientSocket::InClientSocket(const char * HostName, short PortNum)
{
    m_pAddr = new ModelSQL::InSocketAddress(HostName, PortNum);
    if ((m_Socket = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    {
        throw SocketException(SocketException::ESE_SOCKCREATE);
    }
}
    

    
ModelSQL::InServerSocket::InServerSocket(short PortNum)
{
    char HostName[64];
    gethostname (HostName, sizeof (HostName));
    m_pAddr = new ModelSQL::InSocketAddress(HostName, PortNum);
    if ((m_Socket = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
        throw SocketException(SocketException::ESE_SOCKCREATE);
    }
    Bind();
    Listen(BACK_LOG);
}
