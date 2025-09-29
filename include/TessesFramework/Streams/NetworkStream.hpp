#pragma once
#include "Stream.hpp"

namespace Tesses::Framework::Streams
{
    class NetworkStream;
   
    class TcpServer {
        int32_t sock;
        bool owns;
        bool valid;
        public:
            TcpServer(int32_t backlog);
            TcpServer(int32_t sock,bool owns);
            TcpServer(uint16_t port, int32_t backlog);
            TcpServer(std::string ip, uint16_t port, int32_t backlog);
            TcpServer(std::string unixPath,int32_t backlog);
            std::shared_ptr<NetworkStream> GetStream(std::string& ip, uint16_t& port);
            uint16_t GetPort();
            ~TcpServer();
            bool IsValid();
            void Close();
    };  
    enum class SocketType {
        ST_IPv4_TCP,
        ST_IPv4_UDP,
        ST_IPv6_TCP,
        ST_IPv6_UDP,
        ST_UNIX
    };
    class NetworkStream : public Stream {
        int32_t sock;
        bool owns;
        bool success;
        bool endOfStream;
        public:
        bool EndOfStream();
        bool CanRead();
        bool CanWrite();
        NetworkStream(SocketType type);
        NetworkStream(std::string unixPath,bool isServer);
        NetworkStream(std::string ipOrFqdn, uint16_t port, bool datagram,bool broadcast,bool supportIPv6);
        NetworkStream(int32_t sock, bool owns);
        uint16_t GetPort();
        void Listen(int32_t backlog);
        void Bind(std::string ip, uint16_t port);
        void SetBroadcast(bool bC);
        std::shared_ptr<NetworkStream> Accept(std::string& ip, uint16_t& port);
        size_t Read(uint8_t* buff, size_t sz);
        size_t Write(const uint8_t* buff, size_t sz);
        size_t ReadFrom(uint8_t* buff, size_t sz, std::string& ip, uint16_t& port);
        size_t WriteTo(const uint8_t* buff, size_t sz, std::string ip, uint16_t port);
        static std::vector<std::pair<std::string,std::string>> GetIPs(bool ipV6=false);
        ~NetworkStream();
        void SetNoDelay(bool noDelay);
    };
}