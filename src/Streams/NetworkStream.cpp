#include "TessesFramework/Streams/NetworkStream.hpp"
#include "TessesFramework/Http/HttpUtils.hpp"
#include <iostream>
using HttpUtils = Tesses::Framework::Http::HttpUtils;

#if defined(TESSESFRAMEWORK_ENABLE_NETWORKING)


#if defined(GEKKO)

#define ss_family sin_family
#endif
#if defined(GEKKO) && !(defined(TESSESFRAMEWORK_USE_WII_SOCKET) && defined(HW_RVL)) 
#include <network.h>
#define NETWORK_GETSOCKNAME net_getsockname
#define NETWORK_RECV net_recv
#define sockaddr_storage sockaddr_in
#error "Not supported yet"
#else


#if defined(_WIN32)
#include <ws2tcpip.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <windows.h>
#undef min
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")
#else
extern "C" {
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
}
#endif
#if defined(GEKKO)

extern "C" uint32_t if_config( char *local_ip, char *netmask, char *gateway,bool use_dhcp, int max_retries);
#elif !defined(_WIN32) && !defined(__ANDROID__) && !defined(__SWITCH__)
#include <ifaddrs.h>
#endif

#define NETWORK_SEND send
#define NETWORK_RECV recv
#define NETWORK_SENDTO sendto
#define NETWORK_RECVFROM recvfrom
#define NETWORK_SOCKET socket
#define NETWORK_SETSOCKOPT setsockopt
#define NETWORK_CONNECT connect
#define NETWORK_BIND bind
#define NETWORK_LISTEN listen
#define NETWORK_ACCEPT accept
#define NETWORK_GETADDRINFO getaddrinfo
#define NETWORK_FREEADDRINFO freeaddrinfo
#define NETWORK_GETSOCKNAME getsockname
#if defined(_WIN32)
#define NETWORK_CLOSE closesocket
#else
#define NETWORK_CLOSE close
#endif
#endif

#undef AF_INET6


namespace Tesses::Framework::Streams {
    std::string StringifyIP(struct sockaddr* addr);
    std::vector<std::pair<std::string,std::string>> NetworkStream::GetIPs(bool ipV6)
    {
        std::vector<std::pair<std::string, std::string>> ipConfig;
        
        #if defined(GEKKO)
        //if_config( char *local_ip, char *netmask, char *gateway,bool use_dhcp, int max_retries);
        char localIp[16];
        char netmask[16];
        char gateway[16];
        if_config(localIp,netmask, gateway, true, 1);
        ipConfig.push_back(std::pair<std::string,std::string>("net",localIp));
        #elif defined(_WIN32)
        //Thanks https://www.youtube.com/watch?v=K446bcFeE3s
        ULONG family = ipV6 ? 0 : AF_INET;
        ULONG flags = 0;
        ULONG size = 15000;
        PIP_ADAPTER_ADDRESSES addresses = NULL;
        addresses = (PIP_ADAPTER_ADDRESSES)malloc(size);
        
        int retval = GetAdaptersAddresses(family, flags, 0, addresses, &size);
        if(retval != 0) {
            free(addresses);
            return {};
        }
        PIP_ADAPTER_ADDRESSES addrPtr = addresses;
        while(addrPtr != NULL)
        {
            auto fname = addrPtr->FriendlyName;

            size_t len = WideCharToMultiByte(CP_UTF8, 0,fname,-1,NULL, 0,  NULL, NULL);
            std::string name(len, 0);
            WideCharToMultiByte(CP_UTF8, 0,fname,-1,name.data(), len,  NULL, NULL);

            auto ip = addrPtr->FirstUnicastAddress;
            while(ip != NULL)
            {
                ipConfig.push_back(std::pair<std::string,std::string>(name, StringifyIP(ip->Address.lpSockaddr)));
                ip = ip->Next;
            }

            addrPtr = addrPtr->Next;
        }
        free(addresses);


        #elif defined(__ANDROID__) || defined(__SWITCH__)

        #else
        struct ifaddrs *ifAddrStruct = NULL;
        getifaddrs(&ifAddrStruct);

        for (struct ifaddrs *ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr->sa_family == AF_INET) { // IPv4
                
                ipConfig.push_back(std::pair<std::string,std::string>(ifa->ifa_name, StringifyIP(ifa->ifa_addr)));
                
            }
            #if defined(AF_INET6)
            if (ifa->ifa_addr->sa_family == AF_INET6 && ipV6) { // IPv6
                
                ipConfig.push_back(std::pair<std::string,std::string>(ifa->ifa_name, StringifyIP(ifa->ifa_addr)));
                
            }
            #endif
        }

        freeifaddrs(ifAddrStruct);
        #endif
        return ipConfig;
        
    }
    void SetPort(struct sockaddr* addr, uint16_t port)
    {
        if(addr->sa_family == AF_INET)
        {
            struct sockaddr_in* a = (struct sockaddr_in*)addr;\
            a->sin_port = htons(port);
        }
        #if defined(AF_INET6)
        if(addr->sa_family == AF_INET6)
        {
            struct sockaddr_in6* a = (struct sockaddr_in6*)addr;\
            a->sin6_port = htons(port);
            
        }
        #endif
        
    }
    static uint16_t getPort(struct sockaddr* addr)
    {
        if(addr->sa_family == AF_INET)
        {
            struct sockaddr_in* a = (struct sockaddr_in*)addr;\
            return ntohs(a->sin_port);
        }
        #if defined(AF_INET6)
        if(addr->sa_family == AF_INET6)
        {
            struct sockaddr_in6* a = (struct sockaddr_in6*)addr;\
            return ntohs(a->sin6_port);
            
        }
        #endif
        return 0;
    }
    bool IPParse(std::string str,struct sockaddr_storage* addr)
    {    
        memset(addr,0,sizeof(struct sockaddr_storage));
        uint8_t ip[16];
        if(inet_pton(AF_INET,str.c_str(),ip)==1)
        {
            struct sockaddr_in* inAddr = (struct sockaddr_in*)addr;
            inAddr->sin_family = AF_INET;
            memcpy(&inAddr->sin_addr,ip,4);
            return true;
        }
        #if defined(AF_INET6)
        if(inet_pton(AF_INET6,str.c_str(),ip)==1)
        {

            struct sockaddr_in6* inAddr = (struct sockaddr_in6*)addr;
            
            inAddr->sin6_family = AF_INET6;
            memcpy(&inAddr->sin6_addr,ip,16);
            return 6;
        }
        #endif
        return false;
    }

    std::string StringifyIP(struct sockaddr* addr)
    {
        if(addr->sa_family == AF_INET)
        {
            uint8_t* ip = (uint8_t*)&(((struct sockaddr_in*)addr)->sin_addr);
            return std::to_string((uint32_t)ip[0]) + "." + std::to_string((uint32_t)ip[1]) +  "." + std::to_string((uint32_t)ip[2]) + "." + std::to_string((uint32_t)ip[3]);
        }
        #if defined(AF_INET6)
        if(addr->sa_family == AF_INET6)
        {
            uint8_t* ip = (uint8_t*)&(((struct sockaddr_in6*)addr)->sin6_addr);

            return std::string({
            HttpUtils::NibbleToHex((ip[0] >> 4) & 0x0F),
            HttpUtils::NibbleToHex(ip[0] & 0x0F),
            HttpUtils::NibbleToHex((ip[1] >> 4) & 0x0F),
            HttpUtils::NibbleToHex(ip[1] & 0x0F),
            ':',
            HttpUtils::NibbleToHex((ip[2] >> 4) & 0x0F),
            HttpUtils::NibbleToHex(ip[2] & 0x0F),
            HttpUtils::NibbleToHex((ip[3] >> 4) & 0x0F),
            HttpUtils::NibbleToHex(ip[3] & 0x0F),
            ':',
            HttpUtils::NibbleToHex((ip[4] >> 4) & 0x0F),
            HttpUtils::NibbleToHex(ip[4] & 0x0F),
            HttpUtils::NibbleToHex((ip[5] >> 4) & 0x0F),
            HttpUtils::NibbleToHex(ip[5] & 0x0F),
            ':',
            HttpUtils::NibbleToHex((ip[6] >> 4) & 0x0F),
            HttpUtils::NibbleToHex(ip[6] & 0x0F),
            HttpUtils::NibbleToHex((ip[7] >> 4) & 0x0F),
            HttpUtils::NibbleToHex(ip[7] & 0x0F),
            ':',
            HttpUtils::NibbleToHex((ip[8] >> 4) & 0x0F),
            HttpUtils::NibbleToHex(ip[8] & 0x0F),
            HttpUtils::NibbleToHex((ip[9] >> 4) & 0x0F),
            HttpUtils::NibbleToHex(ip[9] & 0x0F),
            ':',
            HttpUtils::NibbleToHex((ip[10] >> 4) & 0x0F),
            HttpUtils::NibbleToHex(ip[10] & 0x0F),
            HttpUtils::NibbleToHex((ip[11] >> 4) & 0x0F),
            HttpUtils::NibbleToHex(ip[11] & 0x0F),
            ':',
            HttpUtils::NibbleToHex((ip[12] >> 4) & 0x0F),
            HttpUtils::NibbleToHex(ip[12] & 0x0F),
            HttpUtils::NibbleToHex((ip[13] >> 4) & 0x0F),
            HttpUtils::NibbleToHex(ip[13] & 0x0F),
            ':',
            HttpUtils::NibbleToHex((ip[14] >> 4) & 0x0F),
            HttpUtils::NibbleToHex(ip[14] & 0x0F),
            HttpUtils::NibbleToHex((ip[15] >> 4) & 0x0F),
            HttpUtils::NibbleToHex(ip[15] & 0x0F),
            });

            
            
            
        }
        #endif
        return "";
    }

    typedef union {
        uint32_t addr;
        uint8_t addr_parts[4];
    } my_addr_t;

    TcpServer::TcpServer(uint16_t port, int32_t backlog)
    {
        this->owns=true;
        this->sock = NETWORK_SOCKET(AF_INET, SOCK_STREAM, 0);   
       
        if(this->sock < 0) 
        {
            std::cout << "FAILED TO CREATE SOCKET FOR SOME REASON" << std::endl;
            this->valid=false;
            return;
        }    

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        if(port > 0)
        {
        int on=1;
        #if defined(SO_REUSEPORT)
        NETWORK_SETSOCKOPT(this->sock,SOL_SOCKET,SO_REUSEPORT,(const char*)&on, (socklen_t)sizeof(on));
        #endif
        #if defined(SO_REUSEADDR)
        NETWORK_SETSOCKOPT(this->sock,SOL_SOCKET,SO_REUSEADDR,(const char*)&on, (socklen_t)sizeof(on));
        #endif
        }
        else
        {
            my_addr_t addr2;
            addr2.addr_parts[0] = 127;
            addr2.addr_parts[1] = 0;
            addr2.addr_parts[2] = 0;
            addr2.addr_parts[3] = 1;
            addr.sin_addr.s_addr=addr2.addr;
        }
        if(NETWORK_BIND(this->sock, (const sockaddr*)&addr, (socklen_t)sizeof(addr)) != 0)
        {
            std::cout << "FAILED TO BIND FOR SOME REASON" << std::endl;
            this->valid = false;
            return;
        }

        if(NETWORK_LISTEN(this->sock, backlog) != 0) 
        {
            std::cout << "FAILED TO LISTEN FOR SOME REASON" << std::endl;
            this->valid = false;
            return;
        }
        this->valid = true;
    }
    TcpServer::TcpServer(int32_t backlog) : TcpServer(0U, backlog)
    {

    }
    uint16_t TcpServer::GetPort()
    {
        struct sockaddr_storage addr;
        memset(&addr, 0, sizeof(addr));
        socklen_t len = sizeof(addr);
        NETWORK_GETSOCKNAME(this->sock, (struct sockaddr*)&addr, &len);
        if(addr.ss_family == AF_INET)
        {
            return (uint16_t)ntohs(((struct sockaddr_in*)&addr)->sin_port);
        }
        #if defined(AF_INET6)
        if(addr.ss_family == AF_INET6)
        {
            return (uint16_t)ntohs(((struct sockaddr_in6*)&addr)->sin6_port);
        }
        #endif
        return 0;
    }
    uint16_t NetworkStream::GetPort()
    {
        struct sockaddr_storage addr;
        memset(&addr, 0, sizeof(addr));
        socklen_t len = sizeof(addr);
        NETWORK_GETSOCKNAME(this->sock, (struct sockaddr*)&addr, &len);
        if(addr.ss_family == AF_INET)
        {
            return (uint16_t)(((struct sockaddr_in*)&addr)->sin_port);
        }
        #if defined(AF_INET6)
        if(addr.ss_family == AF_INET6)
        {
            return (uint16_t)(((struct sockaddr_in6*)&addr)->sin6_port);
        }
        #endif
        return 0;
    }
    bool TcpServer::IsValid()
    {
        return this->valid;
    }
    TcpServer::TcpServer(std::string ip, uint16_t port, int32_t backlog)
    {
        this->owns=true;
        struct sockaddr_storage addr;
        memset(&addr, 0, sizeof(addr));

        uint8_t ipBytes[16];
        bool success = IPParse(ip, &addr);
        if(!success) 
        {
            this->valid=false;
            return;
        }
       
        SetPort((struct sockaddr*)&addr, port);

        this->sock = NETWORK_SOCKET((int)addr.ss_family, SOCK_STREAM, 0);    
        if(this->sock < 0) 
        {
            this->valid=false;
            return;
        }    
        
        
    
        
        if(NETWORK_BIND(this->sock, (const sockaddr*)&addr, (socklen_t)sizeof(addr)) != 0)
        {
            this->valid = false;
            return;
        }

        if(NETWORK_LISTEN(this->sock, backlog) != 0) 
        {
            this->valid = false;
            return;
        }
        this->valid = true;
    }
    TcpServer::TcpServer(int32_t sock, bool owns)
    {
        this->valid = sock >= 0;
        this->sock = sock;
        this->owns = owns;
    }
    TcpServer::~TcpServer()
    {
        if(this->valid && this->owns)
            Close();
    }
    void TcpServer::Close()
    {
        if(this->valid)
            NETWORK_CLOSE(this->sock);
        this->valid=false;
    }
    NetworkStream* TcpServer::GetStream(std::string& ip, uint16_t& port)
    {
        struct sockaddr_storage storage;
        memset(&storage,0, sizeof(storage));
        socklen_t addrlen=(socklen_t)sizeof(storage);
        
        int s = NETWORK_ACCEPT(this->sock, (struct sockaddr*)&storage, &addrlen);
        if(s < 0)
        {
            return nullptr;
        }

        ip = StringifyIP((struct sockaddr*)&storage);
        port = getPort((struct sockaddr*)&storage);
        
        return new NetworkStream(s,true);
    }
    bool NetworkStream::CanRead()
    {
        return this->success;
    }
    bool NetworkStream::CanWrite()
    {
        return this->success;
    }
    NetworkStream::NetworkStream(bool ipV6,bool datagram)
    {
        this->endOfStream=false;
        this->owns = true;
        this->success=false;
        if(ipV6)
        {
            #if defined(AF_INET6)
            this->sock = NETWORK_SOCKET(AF_INET6, datagram ? SOCK_DGRAM : SOCK_STREAM, 0);
            if(this->sock >= 0) this->success=true;
            #endif
        }
        else
        {
            #if defined(AF_INET)
            this->sock = NETWORK_SOCKET(AF_INET, datagram ? SOCK_DGRAM : SOCK_STREAM, 0);
            if(this->sock >= 0) this->success=true;
            #endif
        }
    }
    NetworkStream::NetworkStream(std::string ipOrFqdn, uint16_t port, bool datagram, bool broadcast, bool supportIPv6)
    {
        this->endOfStream = false;
        this->owns=true;
        this->success=false;
        std::string portStr = std::to_string((uint32_t)port);
        struct addrinfo hint;
        memset(&hint, 0, sizeof(hint));
        #if defined(AF_INET6)
        hint.ai_family = supportIPv6 ? AF_UNSPEC : AF_INET;
        #else
        hint.ai_family = AF_INET;
        #endif
        hint.ai_socktype = datagram ? SOCK_DGRAM : SOCK_STREAM;

        struct addrinfo* result;   


        int status = NETWORK_GETADDRINFO(ipOrFqdn.c_str(),portStr.c_str(), &hint, &result);
        if(status != 0)
        {
            return;
        }

        struct addrinfo* tmp=result;


        while(tmp != nullptr)
        {
            sock = NETWORK_SOCKET(tmp->ai_family,tmp->ai_socktype,tmp->ai_protocol);
            if(broadcast)
            {
                int broadcast = 1;
                if (NETWORK_SETSOCKOPT(sock, SOL_SOCKET, SO_BROADCAST, (const char*)&broadcast, sizeof(broadcast)) != 0) {
                    this->success=false;
                    NETWORK_CLOSE(this->sock);
                    continue;
                }
            }

            if(sock != -1)
            {
                this->success=true;
                if(NETWORK_CONNECT(this->sock,(const sockaddr*)tmp->ai_addr,tmp->ai_addrlen) == 0) break;
                this->success=false;
                NETWORK_CLOSE(this->sock);
            }
            tmp = tmp->ai_next;
        }
        NETWORK_FREEADDRINFO(result);

    }
    NetworkStream::NetworkStream(int32_t sock, bool owns)
    {
        this->endOfStream = false;
        this->success = sock >= 0;
        this->sock = sock;
        this->owns = owns;
    }
    bool NetworkStream::EndOfStream()
    {
        return this->endOfStream;
    }
    void NetworkStream::Listen(int32_t backlog)
    {   
        if(this->success)
            NETWORK_LISTEN(this->sock, backlog);
    }
    
    void NetworkStream::Bind(std::string ip, uint16_t port)
    {
        if(!this->success) return;
        struct sockaddr_storage addr;
        memset(&addr, 0, sizeof(addr));

        uint8_t ipBytes[16];
        bool success = IPParse(ip, &addr);
        if(!success) 
        {
            this->success=false;
            if(this->owns)
                NETWORK_CLOSE(this->sock);
            return;
        }
       
        SetPort((struct sockaddr*)&addr, port);
        int on=1;
        #if defined(SO_REUSEPORT)
        NETWORK_SETSOCKOPT(this->sock,SOL_SOCKET,SO_REUSEPORT,(const char*)&on, (socklen_t)sizeof(on));
        #endif
        #if defined(SO_REUSEADDR)
        NETWORK_SETSOCKOPT(this->sock,SOL_SOCKET,SO_REUSEADDR,(const char*)&on, (socklen_t)sizeof(on));
        #endif
        int r = NETWORK_BIND(this->sock, (struct sockaddr*)&addr, sizeof(addr));
        if(r != 0)
        {
            this->success=false;
            if(this->owns)
                NETWORK_CLOSE(this->sock);
            return;
        }
    }
    void NetworkStream::SetBroadcast(bool bC)
    {
        if(!this->success) return;
        int broadcast = 1;
        if (NETWORK_SETSOCKOPT(sock, SOL_SOCKET, SO_BROADCAST, (const char*)&broadcast, sizeof(broadcast)) != 0) 
        {
            this->success=false;
            if(this->owns)
                NETWORK_CLOSE(this->sock);
                    
        }
    }
    NetworkStream* NetworkStream::Accept(std::string& ip, uint16_t& port)
    {
        if(!this->success) return nullptr;
        struct sockaddr_storage storage;
        socklen_t addrlen=(socklen_t)sizeof(storage);
        int s = NETWORK_ACCEPT(this->sock, (struct sockaddr*)&storage, (socklen_t*)&addrlen);
        if(s < 0)
        {
            return nullptr;
        }

        ip = StringifyIP((struct sockaddr*)&storage);
        port = getPort((struct sockaddr*)&storage);
        
        return new NetworkStream(s,true);
    }
    size_t NetworkStream::Read(uint8_t* buff, size_t sz)
    {
        
        if(!this->success) return 0;
        auto r = NETWORK_RECV(this->sock,(char*)buff,sz,0);

        if(r <= 0)
        {
            this->endOfStream=true;
            return 0;
        }
        
        return (size_t)r;
    }
    size_t NetworkStream::Write(const uint8_t* buff, size_t sz)
    {

        if(!this->success) return 0;
       
        auto sz2 = NETWORK_SEND(this->sock,(const char*)buff,sz, 0);
        if(sz2 <= 0) {
            this->endOfStream=true;
            return 0;
        }
        
        return (size_t)sz;
    }
    size_t NetworkStream::ReadFrom(uint8_t* buff, size_t sz, std::string& ip, uint16_t& port)
    {
        if(!this->success) return 0;
        struct sockaddr_storage storage;
        socklen_t addrlen=(socklen_t)sizeof(storage);
        auto r = NETWORK_RECVFROM(this->sock,(char*)buff,sz,0, (struct sockaddr*)&storage, (socklen_t*)&addrlen);
        ip = StringifyIP((struct sockaddr*)&storage);
        port = getPort((struct sockaddr*)&storage);
        if(r < 0) return 0;
        return (size_t)r;

        

        
        
    }
    size_t NetworkStream::WriteTo(const uint8_t* buff, size_t sz, std::string ip, uint16_t port)
    {

        if(!this->success) return 0;
        struct sockaddr_storage addr;
        memset(&addr, 0, sizeof(addr));

        uint8_t ipBytes[16];
        bool success = IPParse(ip, &addr);
        if(!success) 
        {
            this->success=false;
            if(this->owns)
                NETWORK_CLOSE(this->sock);
            return 0;
        }
       
        SetPort((struct sockaddr*)&addr, port);
        auto sz2 = NETWORK_SENDTO(this->sock,(const char*)buff,sz, 0, (const sockaddr*)&addr, (socklen_t)sizeof(addr));
        if(sz2 < 0) return 0;
        return (size_t)sz2;
    }
    NetworkStream::~NetworkStream()
    {
        if(this->owns && this->success)
            NETWORK_CLOSE(this->sock);
    }
    void NetworkStream::SetNoDelay(bool noDelay)
    {
         
        int noDelay2 = noDelay;
        NETWORK_SETSOCKOPT(this->sock, SOL_SOCKET, TCP_NODELAY, (const char*)&noDelay2,(socklen_t)sizeof(noDelay2));
    }
    
}
#else
namespace Tesses::Framework::Streams {

TcpServer::TcpServer(int32_t sock,bool owns)
{

}
TcpServer::TcpServer(uint16_t port, int32_t backlog)
{

}
TcpServer::TcpServer(std::string ip, uint16_t port, int32_t backlog)
{

}
NetworkStream* TcpServer::GetStream(std::string& ip, uint16_t& port)
{
    return nullptr;
}
TcpServer::~TcpServer()
{

}
bool TcpServer::IsValid()
    {
        return this->valid;
    }
void TcpServer::Close()
{
    
}
bool NetworkStream::EndOfStream() {
    return true;
}
bool NetworkStream::CanRead() {
    return false;
}
bool NetworkStream::CanWrite() {
    return false;
}
    
NetworkStream::NetworkStream(bool ipV6,bool datagram)
{

}
NetworkStream::NetworkStream(std::string ipOrFqdn, uint16_t port, bool datagram,bool broadcast,bool supportIPv6)
{

}
NetworkStream::NetworkStream(int32_t sock, bool owns)
{

}
void NetworkStream::Listen(int32_t backlog)
{

}
void NetworkStream::Bind(std::string ip, uint16_t port)
{

}
void NetworkStream::SetBroadcast(bool bC)
{

}
NetworkStream* NetworkStream::Accept(std::string& ip, uint16_t& port)
{
    return nullptr;
}
size_t NetworkStream::Read(uint8_t* buff, size_t sz)
{
    return 0;
}
size_t NetworkStream::Write(const uint8_t* buff, size_t sz)
{
    return 0;
}
size_t NetworkStream::ReadFrom(uint8_t* buff, size_t sz, std::string& ip, uint16_t& port)
{
    return 0;
}
size_t NetworkStream::WriteTo(const uint8_t* buff, size_t sz, std::string ip, uint16_t port)
{
    return 0;
}
std::vector<std::pair<std::string,std::string>> NetworkStream::GetIPs(bool ipV6)
{
    return {};
}
NetworkStream::~NetworkStream()
{

}
void NetworkStream::SetNoDelay(bool noDelay)
{
    
}

}
#endif
