#include "TessesFramework/Http/HttpServer.hpp"
#include "TessesFramework/Streams/FileStream.hpp"
#include "TessesFramework/Streams/MemoryStream.hpp"
#include "TessesFramework/TextStreams/StreamWriter.hpp"
#include "TessesFramework/TextStreams/StreamReader.hpp"
#include "TessesFramework/Http/ContentDisposition.hpp"
#include "TessesFramework/Streams/BufferedStream.hpp"
#include "TessesFramework/Http/HttpStream.hpp"
#include "TessesFramework/Crypto/Crypto.hpp"
#include "TessesFramework/Threading/Mutex.hpp"
#include "TessesFramework/Common.hpp"
#include "TessesFramework/TextStreams/StdIOWriter.hpp"
#include "TessesFramework/Filesystem/VFSFix.hpp"
#include "TessesFramework/Filesystem/VFS.hpp"

#include <iostream>
using FileStream = Tesses::Framework::Streams::FileStream;
using Stream = Tesses::Framework::Streams::Stream;
using SeekOrigin = Tesses::Framework::Streams::SeekOrigin;
using MemoryStream = Tesses::Framework::Streams::MemoryStream;
using TcpServer = Tesses::Framework::Streams::TcpServer;
using NetworkStream = Tesses::Framework::Streams::NetworkStream;
using BufferedStream = Tesses::Framework::Streams::BufferedStream;

using namespace Tesses::Framework::TextStreams;

namespace Tesses::Framework::Http
{
    class WSServer 
    {
        public:
        Threading::Mutex mtx;
        ServerContext* ctx;
        std::shared_ptr<WebSocketConnection> conn;
        std::shared_ptr<Stream> strm;
        std::atomic_bool hasInit;
        std::atomic<bool> closed;
        void close()
        {
            mtx.Lock();
            closed=true;
            uint8_t finField = 0b10000000 ;
            uint8_t firstByte= finField | 0x8;
            strm->WriteByte(firstByte);
            strm->WriteByte(0);
           
            this->strm = nullptr;
            mtx.Unlock(); 
            this->conn->OnClose(true);
        }
        void write_len_bytes(uint64_t len)
        {
            if(len < 126)
            {
                strm->WriteByte((uint8_t)len);
            }
            else if(len < 65535)
            {
                uint8_t b[3];
                b[0] = 126;
                b[1] = (uint8_t)(len >> 8);
                b[2] = (uint8_t)len;

                strm->WriteBlock(b,sizeof(b));
            }
            else {

                uint8_t b[9];
                b[0] = 127;

                b[1] = (uint8_t)(len >> 56);
                b[2] = (uint8_t)(len >> 48);
                b[3] = (uint8_t)(len >> 40);
                b[4] = (uint8_t)(len >> 32);
                b[5] = (uint8_t)(len >> 24);
                b[6] = (uint8_t)(len >> 16);
                b[7] = (uint8_t)(len >> 8);
                b[8] = (uint8_t)len;

                strm->WriteBlock(b,sizeof(b));
            }
        }
        uint64_t get_long()
        {
            uint8_t buff[8];
            if(strm->ReadBlock(buff,sizeof(buff)) != sizeof(buff)) return 0;
            
            uint64_t v = 0;
            v |= (uint64_t)buff[0] << 56;
            v |= (uint64_t)buff[1] << 48;
            v |= (uint64_t)buff[2] << 40;
            v |= (uint64_t)buff[3] << 32;
            v |= (uint64_t)buff[4] << 24;
            v |= (uint64_t)buff[5] << 16;
            v |= (uint64_t)buff[6] << 8;
            v |= (uint64_t)buff[7];
            return v;
        }
        uint16_t get_short()
        {
            uint8_t buff[2];
            if(strm->ReadBlock(buff,sizeof(buff)) != sizeof(buff)) return 0;
            
            uint16_t v = 0;
            v |= (uint16_t)buff[0] << 8;
            v |= (uint16_t)buff[1];
            return v;
        }
        void send_msg(WebSocketMessage* msg)
        {
            while(!hasInit);
            mtx.Lock();
          
            uint8_t opcode = msg->isBinary ? 0x2 : 0x1;

            size_t lengthLastByte = msg->data.size() % 4096;
            size_t fullPackets = (msg->data.size() - lengthLastByte) / 4096;
            size_t noPackets = lengthLastByte > 0 ? fullPackets+1 : fullPackets;
            size_t offset = 0;
            for(size_t i = 0; i < noPackets; i++)
            {
                bool fin = i == noPackets - 1;
                uint8_t finField =  fin ?  0b10000000 : 0;
                uint8_t opcode2 = i == 0 ? opcode : 0;
                uint8_t firstByte = finField | (opcode2 & 0xF);
                
                size_t len = std::min((size_t)4096,msg->data.size()- offset);
                
                strm->WriteByte(firstByte);
                write_len_bytes((uint64_t)len);
                strm->WriteBlock(msg->data.data() + offset,len);
                offset += len;
            }   
            mtx.Unlock();
        }
        void ping_send(std::vector<uint8_t>& pData)
        {
            
            mtx.Lock();
          
            uint8_t finField = 0b10000000 ;
            uint8_t firstByte= finField | 0x9;
            strm->WriteByte(firstByte);
            write_len_bytes((uint64_t)pData.size());
            strm->WriteBlock(pData.data(),pData.size());
            mtx.Unlock();
        }
        void pong_send(std::vector<uint8_t>& pData)
        {
            mtx.Lock();
           
            uint8_t finField = 0b10000000 ;
            uint8_t firstByte= finField | 0xA;
            strm->WriteByte(firstByte);
            write_len_bytes((uint64_t)pData.size());
            strm->WriteBlock(pData.data(),pData.size());
            mtx.Unlock();
        }
        bool read_packet(uint8_t len,std::vector<uint8_t>& data)
        {
           
            uint8_t realLen=len & 127;
            bool masked=(len & 0b10000000) > 0;
            uint64_t reallen2 = realLen >= 126 ? realLen > 126 ? get_long() : get_short() : realLen;
            uint8_t mask[4];
            if(masked)
            {
                if(strm->ReadBlock(mask,sizeof(mask)) != sizeof(mask)) return false;
            }
            size_t offset = data.size();
            data.resize(offset+(size_t)reallen2);
            if(data.size() < ((uint64_t)offset+reallen2)) return false;
            if(strm->ReadBlock(data.data()+offset,(size_t)reallen2) != (size_t)reallen2) return false;
            if(masked)
            {
                for(size_t i = 0; i < (size_t)reallen2; i++)
                {
                    data[i+offset] ^= mask[i%4];
                }
            }
            return true;
        }
        
            WSServer(ServerContext* ctx,std::shared_ptr<WebSocketConnection> conn)
            {
                this->ctx = ctx;
                this->conn = conn;
                this->strm = this->ctx->GetStream();
                this->hasInit=false;

                
            }
            void Start()
            {
                this->closed=false;
                std::string key;
                if(ctx->requestHeaders.TryGetFirst("Sec-WebSocket-Key", key) && !key.empty())
                {
                    key.append("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
                    auto res = Crypto::Sha1::ComputeHash((const uint8_t*)key.c_str(),key.size());
                    
                    if(res.empty()) return;
                    key = Crypto::Base64_Encode(res);

                }else {

                    return;
                }
                
                if(!ctx->requestHeaders.AnyEquals("Upgrade","websocket"))
                {

                    return;
                }
                if(!ctx->requestHeaders.AnyEquals("Sec-WebSocket-Version","13"))
                {

                    return;
                }
                ctx->statusCode = StatusCode::SwitchingProtocols;
                ctx->responseHeaders.SetValue("Connection","Upgrade");
                ctx->responseHeaders.SetValue("Upgrade","websocket");
                ctx->responseHeaders.SetValue("Sec-WebSocket-Accept",key);
        
                ctx->WriteHeaders();
                bool hasMessage =false;

                WebSocketMessage message;
                message.isBinary=false;
                message.data={};
                hasInit=true;

                while( !strm->EndOfStream())
                {
                
                    uint8_t frame_start[2];
                    if( strm->ReadBlock(frame_start,2) != 2) return;
                    
                    
                    uint8_t opcode = frame_start[0] & 0xF;
                    bool fin = (frame_start[0] & 0b10000000) > 0;
                    switch(opcode)
                    {
                        case 0x0:
                            if(!hasMessage) break;
                            read_packet(frame_start[1], message.data);
                        break;
                        case 0x1:
                        case 0x2:
                            hasMessage=true;
                            message.data = {};
                            message.isBinary = opcode == 0x2;
                            
                            read_packet(frame_start[1], message.data);
                            break;
                        case 0x8:

                        if(!this->closed) this->close();
                        this->conn->OnClose(true);
                        return;
                        case 0x9:
                        {
                            std::vector<uint8_t> b;
                            read_packet(frame_start[1],b);
                            pong_send(b);
                        }
                        break;
                        case 0xA:
                        {
                            std::vector<uint8_t> b;
                            read_packet(frame_start[1],b);
                        }
                    }
                    if(fin && hasMessage)
                    {
                        hasMessage=false;
                        this->conn->OnReceive(message);
                        message.data={};
                    }
                }
                this->conn->OnClose(false);
            }
    }; 
    /*
    static int _header_field(multipart_parser* p, const char *at, size_t length)
    {
        UploadData* data = static_cast<UploadData*>(multipart_parser_get_data(p));
        data->currentHeaderKey = std::string(at,length);
        return 0;
    }
    static int _header_value(multipart_parser* p, const char *at, size_t length)
    {
        UploadData* data = static_cast<UploadData*>(multipart_parser_get_data(p));
        data->currentHeaders.AddValue(data->currentHeaderKey, std::string(at,length));
        std::cout << data->currentHeaderKey << ": " <<  std::string(at,length) << std::endl;
        return 0;
    }
    static int _part_begin(multipart_parser* p)
    {
        UploadData* data = static_cast<UploadData*>(multipart_parser_get_data(p));
        std::string cd0;
        ContentDisposition cd1;
        std::string ct;
        if(!data->currentHeaders.TryGetFirst("Content-Type",ct))
            ct = "application/octet-stream";
        if(data->currentHeaders.TryGetFirst("Content-Disposition", cd0) && ContentDisposition::TryParse(cd0,cd1))
        {
            if(cd1.filename.empty())
            {
                data->isFile=false;
                data->key = cd1.fieldName;
                data->currentBody = new MemoryStream(true);
            }
            else
            {
                data->isFile = true;
                data->currentBody = data->cb(ct, cd1.filename, cd1.fieldName);
            }
        }
        return 0;
    }
    static int _part_end(multipart_parser* p)
    {
        UploadData* data = static_cast<UploadData*>(multipart_parser_get_data(p));
        if(data->currentBody == nullptr) return 0;
        if(data->isFile)
        {
            delete data->currentBody;
            data->currentBody = nullptr;
        }
        else
        {
            MemoryStream* ms = dynamic_cast<MemoryStream*>(data->currentBody);
            if(ms != nullptr)
            {
                ms->Seek(0, SeekOrigin::Begin);
                auto& buff = ms->GetBuffer();
                data->ctx->queryParams.AddValue(data->key, std::string(buff.begin(),buff.end()));
            }
            delete data->currentBody;
            data->currentBody=nullptr;
        }
        data->currentHeaders.Clear();
        return 0;
    }*/
    std::string ServerContext::GetUrlWithQuery()
   {
        if(this->queryParams.kvp.empty()) return this->path;
        return this->path + "?" + HttpUtils::QueryParamsEncode(this->queryParams);
   }
   std::string ServerContext::GetOriginalPathWithQuery()
   {
        if(this->queryParams.kvp.empty()) return this->originalPath;
        return this->originalPath + "?" + HttpUtils::QueryParamsEncode(this->queryParams);
   }
   void ServerContext::ReadStream(std::shared_ptr<Stream> strm)
   {
        if(strm == nullptr) return;
        auto strm2 = this->OpenRequestStream();
        if(strm2 != nullptr)
        {
            strm2->CopyTo(strm);
        }
   }
   
   std::string ServerContext::ReadString()
   {
        if(strm == nullptr) return {};
        auto strm2 = this->OpenRequestStream();
        if(strm2 != nullptr)
        {
            TextStreams::StreamReader reader(strm2);
            return reader.ReadToEnd();
        }
        return {};
   }
    bool ServerContext::NeedToParseFormData()
    {
        std::string ct;
        if(this->requestHeaders.TryGetFirst("Content-Type",ct))
        {
            if(ct.find("multipart/form-data") == 0)
            {
                return true;
            }
        }
        return false;
    }
    static bool parseUntillBoundaryEnd(std::shared_ptr<Tesses::Framework::Streams::Stream> src, std::shared_ptr<Tesses::Framework::Streams::Stream> dest, std::string boundary)
    {
        bool hasMore=true;
        uint8_t* checkBuffer = new uint8_t[boundary.size()];
        
        int b;
        size_t i = 0;
        size_t i2 = 0;

        uint8_t buffer[1024];
        size_t offsetInMem = 0;

        while((b = src->ReadByte()) != -1)
        {
            if(i == boundary.size())
            {
                if (b == '-')
                {
                    i2++;
                    if(i2 == 2) hasMore=false;
                }
                if (b == '\n') break;
                else if(b != '-')
                    i2 = 0;
                continue;
            }
            i2=0;

            if (b == boundary[i]) //start filling the check buffer
            {
                checkBuffer[i] = (uint8_t)b;
                i++;
            }
            else
            {
                size_t idx = 0;
                while (idx < i) //write the buffer data to stream
                {

                if(offsetInMem >= sizeof(buffer))
                {
                    dest->Write(buffer, sizeof(buffer));
                    offsetInMem=0;
                    
                }

                    buffer[offsetInMem++] = checkBuffer[idx];
                        
                        idx++;
                }

                i = 0;
                
                if(offsetInMem >= sizeof(buffer))
                {
                    dest->Write(buffer, sizeof(buffer));
                    offsetInMem=0;
                    
                }

                buffer[offsetInMem++] = (uint8_t)b;
            }
        }

        if(offsetInMem > 0)
        {
            dest->Write(buffer,offsetInMem);
        }
        delete[] checkBuffer;
        
        return hasMore;
    }

    static bool parseSection(ServerContext* ctx, std::string boundary, std::function<std::shared_ptr<Tesses::Framework::Streams::Stream>(std::string mime, std::string filename, std::string name)> cb)
    {
        HttpDictionary req;
        StreamReader reader(ctx->GetStream());
        std::string line;
        while(reader.ReadLineHttp(line))
        {
            auto v = HttpUtils::SplitString(line,": ", 2);    
            if(v.size() == 2)
                req.AddValue(v[0],v[1]);
            line.clear();
        }

        std::string cd0;
        ContentDisposition cd1;
        std::string ct;
       
        if(!req.TryGetFirst("Content-Type",ct))
            ct = "application/octet-stream";
        if(req.TryGetFirst("Content-Disposition", cd0) && ContentDisposition::TryParse(cd0,cd1))
        {
            if(cd1.filename.empty())
            {
                std::shared_ptr<MemoryStream> ms=std::make_shared<MemoryStream>(true);

                bool retVal = parseUntillBoundaryEnd(ctx->GetStream(),ms,boundary);
                auto& buff = ms->GetBuffer();

                ctx->queryParams.AddValue(cd1.fieldName, std::string(buff.begin(),buff.end()));

                return retVal;

            }
            else
            {
                auto strm = cb(ct, cd1.filename, cd1.fieldName); 
                if(strm == nullptr) strm = std::make_shared<Stream>();
                bool retVal = parseUntillBoundaryEnd(ctx->GetStream(),strm,boundary);
               
                return retVal;
            }
        }
        return false;
    }

    void ServerContext::ParseFormData(std::function<std::shared_ptr<Tesses::Framework::Streams::Stream>(std::string mime, std::string filename, std::string name)> cb)
    {
        std::string ct;
        if(this->requestHeaders.TryGetFirst("Content-Type",ct))
        {
            if(ct.find("multipart/form-data") != 0)
            {
                throw std::runtime_error("Not form data");
            }
            auto res = ct.find("boundary=");
            if(res == std::string::npos) return;
            ct = "--" + ct.substr(res+9);
        }
        std::shared_ptr<Stream> nullStrm=std::make_shared<Stream>();
        parseUntillBoundaryEnd(this->strm,nullStrm, ct);

        while(parseSection(this, ct, cb));
    }   

    HttpServer::HttpServer(std::shared_ptr<Tesses::Framework::Streams::TcpServer> tcpServer, std::shared_ptr<IHttpServer> http, bool showIPs)
    {
        this->server = tcpServer;
        this->http = http;

        this->showIPs = showIPs;
        this->thrd=nullptr;
        this->showARTL = showIPs;
    }
   
            
    HttpServer::HttpServer(uint16_t port, std::shared_ptr<IHttpServer> http, bool showIPs) : HttpServer(std::make_shared<TcpServer>(port,10),http,showIPs)
    {
        
    }
    
    HttpServer::HttpServer(std::string unixPath, std::shared_ptr<IHttpServer> http) : HttpServer(std::make_shared<TcpServer>(unixPath,10),http,false)
    {
        this->showARTL=true;
    }
   

    uint16_t HttpServer::GetPort()
    {
        if(server != nullptr)
        return server->GetPort();
        return 0;
    }
    std::shared_ptr<Stream> ServerContext::OpenResponseStream()
    {
        if(sent) return nullptr;
        int64_t length = -1;
        if(!this->responseHeaders.TryGetFirstInt("Content-Length",length))
            length = -1;
        
        if(this->version == "HTTP/1.1" && length == -1)
            this->responseHeaders.SetValue("Transfer-Encoding","chunked");

        this->WriteHeaders();
        auto strm = std::make_shared<HttpStream>(this->strm,length,false,version == "HTTP/1.1");
        if(method == "HEAD")
        {
            strm->Close();
        }
        return strm;
    }
    std::shared_ptr<Stream> ServerContext::OpenRequestStream()
    {
        int64_t length = -1;
        if(!this->requestHeaders.TryGetFirstInt("Content-Length",length))
            length = -1;
        return  std::make_shared<HttpStream>(this->strm,length,true,version == "HTTP/1.1");
    }
    void HttpServer::StartAccepting()
    {
        fflush(stdout);
        if(http == nullptr || server == nullptr) return;
        auto svr=this->server;
        auto serverPort = this->server->GetPort();
        auto http = this->http;
        TF_LOG("Before Creating Thread");
        thrd = new Threading::Thread([svr,http,serverPort]()->void {
            while(TF_IsRunning())
            {
                TF_LOG("after TF_IsRunning");
                std::string ip;
                uint16_t port;
                auto sock =svr->GetStream(ip,port);
                
                TF_LOG("New Host IP: " + ip + ":" + std::to_string(port));
                
                if(sock == nullptr) 
                {
                    std::cout << "STREAM ERROR" << std::endl;
                    return;
                }
                TF_LOG("Before entering socket thread");
                Threading::Thread thrd2([sock,http,ip,port,serverPort]()->void {
                    TF_LOG("In thread to process");
                    HttpServer::Process(sock,http,ip,port,serverPort,false);
                    TF_LOG("In thread after process");
                    
                });
                TF_LOG("Before attach");
                thrd2.Detach();
                TF_LOG("After attach");
            }
        });
        if(this->showIPs)
        {
            TF_LOG("Before printing interfaces");
        
            StdOut() << "\x1B[34mInterfaces:" << NewLine();
            for(auto _ip : NetworkStream::GetIPs())
            {
                StdOut() << "\x1B[32m"
                << _ip.first << ": "
                << "\x1B[35mhttp://"
                << _ip.second << ":" << (uint64_t)this->GetPort() << "/" << NewLine();
            }
            
            
        }
        if(this->showARTL)
        {
            if(!svr->IsValid()) std::cout << "\x1B[31mError, we failed to bind or something\x1B[39m\n" << std::endl;
            StdOut() << "\x1B[31mAlmost Ready to Listen\x1B[39m" << NewLine();
        }
        
        TF_LOG("After printing interfaces");
    }
    HttpServer::~HttpServer()
    {
        auto port = this->GetPort();
        this->server->Close();
        TF_ConnectToSelf(port);
        if(this->thrd != nullptr)
        {
            this->thrd->Join();
            delete this->thrd;
        }
        
    }
    IHttpServer::~IHttpServer()
    {
    
    }
    ServerContext::ServerContext(std::shared_ptr<Stream> strm)
    {
        this->statusCode = OK;
        this->strm = strm;
        this->sent = false;
        this->queryParams.SetCaseSensitive(true);
        this->responseHeaders.AddValue("Server","TessesFrameworkWebServer");
    }
    std::shared_ptr<Stream> ServerContext::GetStream()
    {
        return this->strm;
    }
    void ServerContext::SendBytes(std::vector<uint8_t> buff)
    {
        std::shared_ptr<MemoryStream> strm=std::make_shared<MemoryStream>(false);
        strm->GetBuffer() = buff;
        SendStream(strm);
    }
    ServerContext& ServerContext::WithLastModified(Date::DateTime dt)
    {
        this->responseHeaders.SetValue("Last-Modified",dt);
        return *this;
    }

    void ServerContext::SendText(std::string text)
    {
        std::shared_ptr<MemoryStream> strm=std::make_shared<MemoryStream>(false);
        
        auto& buff= strm->GetBuffer();
        buff.insert(buff.end(),text.begin(),text.end());
        SendStream(strm);
    }
    void ServerContext::SendErrorPage(bool showPath)
    {
        if(sent) return;
        std::string errorHtml = showPath ? ("<html><head><title>File " + HttpUtils::HtmlEncode(this->originalPath) + " " + HttpUtils::StatusCodeString(this->statusCode) + "</title></head><body><h1>" + std::to_string((int)this->statusCode) + " " + HttpUtils::StatusCodeString(this->statusCode) + "</h1><h4>" + HttpUtils::HtmlEncode(this->originalPath) + "</h4></body></html>") : "";

        WithMimeType("text/html").SendText(errorHtml);
    }
    
    ServerContext::~ServerContext()
    {
        for(auto item : this->data)
        {
            delete item.second;
        }
    }
    ServerContextData::~ServerContextData()
    {
        
    }
    void ServerContext::SendStream(std::shared_ptr<Stream> strm)
    {
        if(sent) return;
        if(!strm->CanRead()) throw std::runtime_error("Cannot read from stream");
        if(strm->EndOfStream()) throw std::runtime_error("End of stream");
        if(strm->CanSeek())
        {
            int64_t len=strm->GetLength();
            std::string range={};
            if(this->requestHeaders.TryGetFirst("Range",range))
            {
                auto res = HttpUtils::SplitString(range,"=",2);
                if(res.size() == 2)
                {
                    if(res[0] != "bytes")
                    {
                        this->statusCode = BadRequest;
                        this->WriteHeaders();
                        return;
                    }
                    res = HttpUtils::SplitString(res[1],", ",2);
                    if(res.size() != 1) 
                    {
                        this->statusCode = BadRequest;
                        this->WriteHeaders();
                        return;
                    }
                    auto dash=HttpUtils::SplitString(res[0],"-",2);
                    int64_t begin = 0;
                    int64_t end = -1;

                    if(dash.size() == 1 && res[0].find_first_of('-') != std::string::npos)
                    {
                        //NUMBER-
                        begin = std::stoll(dash[0]);
                    }
                    else if(dash.size() == 2)
                    {
                        //NUMBER-NUMBER
                        //or
                        //-NUMBER

                        if(dash[0].size() > 0)
                        {
                            //NUMBER-NUMBER
                            begin = std::stoll(dash[0]);
                            end = std::stoll(dash[1]);
                        }
                        else
                        {
                            //-NUMBER
                            end = std::stoll(dash[1]);
                        }
                    }
                    else
                    {
                        this->statusCode = BadRequest;
                        this->WriteHeaders();
                        return;
                    }

                    if(end == -1)
                    {
                        end = len-1; 
                    }

                    if(end > len-1)
                    {
                        this->statusCode = RangeNotSatisfiable;
                        this->WithSingleHeader("Content-Range","bytes */" + std::to_string(len));
                        this->WriteHeaders();
                        return;
                    }

                    if(begin >= end) {
                        this->statusCode = RangeNotSatisfiable;
                        this->WithSingleHeader("Content-Range","bytes */" + std::to_string(len));
                        this->WriteHeaders();
                        return;
                    }

                    int64_t myLen = (end - begin)+1;

                    this->WithSingleHeader("Accept-Ranges","bytes");
                    this->WithSingleHeader("Content-Length",std::to_string(myLen));
                    this->WithSingleHeader("Content-Range","bytes " + std::to_string(begin) + "-" + std::to_string(end) + "/" + std::to_string(len));
                    this->statusCode = PartialContent;
                    this->WriteHeaders();
                    if(this->method != "HEAD")
                    {
                        strm->Seek(begin,SeekOrigin::Begin);

                        uint8_t buffer[1024];

                        size_t read=0;
                   
                        do {
                            read = sizeof(buffer);
                            myLen = (end - begin)+1;
                            if(myLen < read) read = (size_t)myLen;
                            if(read == 0) break;
                            read = strm->Read(buffer,read);
                            if(read == 0) break;
                            this->strm->WriteBlock(buffer,read);

                            begin += read;
                        } while(read > 0 && !this->strm->EndOfStream());
                    }
                    
                }
                else
                {
                    this->statusCode = BadRequest;
                    this->SendErrorPage(true);
                    return;
                }
            }
            else
            {
                if(len > -1)
                {
                    this->WithSingleHeader("Accept-Range","bytes");
                    this->WithSingleHeader("Content-Length",std::to_string(len));
                    this->WriteHeaders();
                    if(this->method != "HEAD")
                    strm->CopyTo(this->strm);
                }
            }

        }
        else
        {
            
            auto chunkedStream = this->OpenResponseStream();

            if(method != "HEAD")
            strm->CopyTo(chunkedStream);
            
            
        }
    }
    ServerContext& ServerContext::WithHeader(std::string key, std::string value)
    {
        this->responseHeaders.AddValue(key, value);
        return *this;    
    }
    ServerContext& ServerContext::WithSingleHeader(std::string key, std::string value)
    {
        this->responseHeaders.SetValue(key, value);
        return *this;
    }
    ServerContext& ServerContext::WithMimeType(std::string mime)
    {
        this->responseHeaders.SetValue("Content-Type",mime);
        return *this;
    }
    ServerContext& ServerContext::WithContentDisposition(std::string filename, bool isInline)
    {
        ContentDisposition cd;
        cd.type = isInline ? "inline" : "attachment";
        cd.filename = filename;

        //std::string cd;
        //cd = (isInline ? "inline; filename*=UTF-8''" : "attachment; filename*=UTF-8''") + HttpUtils::UrlPathEncode(filename);
        this->responseHeaders.SetValue("Content-Disposition",cd.ToString());
        return *this;
    }
    ServerContext& ServerContext::WithLocationHeader(std::string url)
    {
        return WithSingleHeader("Location", this->MakeAbsolute(url));
    }
    ServerContext& ServerContext::WithLocationHeader(std::string url,StatusCode sc)
    {
        this->statusCode = sc;
        return WithSingleHeader("Location", this->MakeAbsolute(url));
    }
    void ServerContext::SendRedirect(std::string url)
    {
        WithLocationHeader(url);
        this->WriteHeaders();
    }
    void ServerContext::SendRedirect(std::string url,StatusCode sc)
    {
        WithLocationHeader(url,sc);
        this->WriteHeaders();
    }

    void ServerContext::SendNotFound()
    {
        if(sent) return;
        statusCode = StatusCode::NotFound;
        SendErrorPage(true);
    }
    void ServerContext::SendBadRequest()
    {
        if(sent) return;
        statusCode = StatusCode::BadRequest;
        SendErrorPage(false);
    }
    void ServerContext::SendException(std::exception& ex)
    {
        /*<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Internal Server Error at /</title>
</head>
<body>
    <h1>Internal Server Error at /</h1>
    <p>what():  std::exception</p>
</body>
</html>*/
       this->WithMimeType("text/html").SendText("<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Internal Server Error at " + HttpUtils::HtmlEncode(this->originalPath) + "</title></head><body><h1>Internal Server Error at " + HttpUtils::HtmlEncode(this->originalPath) + "</h1><p>what(): " + HttpUtils::HtmlEncode(ex.what()) + "</p></body></html>");
    }
    ServerContext& ServerContext::WriteHeaders()
    {
        if(this->sent) return *this;
        this->sent = true;

        StreamWriter writer(this->strm);
        writer.newline = "\r\n";
        writer.WriteLine("HTTP/1.1 " + std::to_string((int)statusCode) + " " + HttpUtils::StatusCodeString(statusCode));
        for(auto& hdr : responseHeaders.kvp)
        {
            auto& key = hdr.first;
            for(auto& val : hdr.second)
            {
                writer.WriteLine(key + ": " + val);
            }
        }
        writer.WriteLine();
        
        return *this;
    }
    void HttpServer::Process(std::shared_ptr<Stream> strm, std::shared_ptr<IHttpServer> server, std::string ip, uint16_t port,uint16_t serverPort, bool encrypted)
    {
        TF_LOG("In process");
        while(true)
        {
            std::shared_ptr<BufferedStream> bStrm = std::make_shared<BufferedStream>(strm);
            StreamReader reader(bStrm);
            ServerContext ctx(bStrm);
            ctx.ip = ip;
            ctx.port = port;
            ctx.encrypted = encrypted;
            ctx.serverPort = serverPort;
            try{
            bool firstLine = true;
            std::string line;
            while(reader.ReadLineHttp(line))
            {
                 if(firstLine)
                {
                    auto v = HttpUtils::SplitString(line, " ", 3);
                    if(v.size() != 3) {
                        ctx.statusCode = BadRequest;
                        ctx.WithMimeType("text/plain").SendText("First line is not 3 elements");
                        return;
                    }
                    ctx.method = v[0];
                    auto pp = HttpUtils::SplitString(v[1],"?", 2);
                    pp.resize(2);

                    ctx.originalPath = pp[0];
                    ctx.path = ctx.originalPath;

                    TF_LOG(ctx.method + " with path " + ctx.path);

                    auto queryPart = pp[1];
                    if(!queryPart.empty())
                    {
                        HttpUtils::QueryParamsDecode(ctx.queryParams, queryPart);
                    }

                    ctx.version = v[2];

                }
                else
                {
                    auto v = HttpUtils::SplitString(line,": ", 2);
                    if(v.size() != 2) {
                        ctx.statusCode = BadRequest;
                        ctx.WithMimeType("text/plain").SendText("Header line is not 2 elements");
                        return;
                    }
                  
                    ctx.requestHeaders.AddValue(v[0],v[1]);
                    
                }
                line.clear();
                firstLine=false;
            }

            if(firstLine) return;

            std::string type;
            int64_t length;

            if(ctx.requestHeaders.TryGetFirst("Content-Type",type) && type == "application/x-www-form-urlencoded" && ctx.requestHeaders.TryGetFirstInt("Content-Length",length))
            {
                size_t len = (size_t)length;
                uint8_t* buffer = new uint8_t[len];
                len = bStrm->ReadBlock(buffer,len);
                std::string query((const char*)buffer,len);
                delete[] buffer;
                HttpUtils::QueryParamsDecode(ctx.queryParams, query);
            }
            
                if(!server->Handle(ctx)) 
                {
                    ctx.SendNotFound();
                }
            } 

            catch(std::exception& ex)
            {
                ctx.SendException(ex);
            }
            catch(std::string& ex)
            {
                std::runtime_error re(ex);
                ctx.SendException(re);
            }
            catch(const char* ex)
            {
                std::runtime_error re(ex);
                ctx.SendException(re);
            }
            catch(...)
            {
                std::runtime_error ex("An unknown error occurred");
                ctx.SendException(ex);   
            }

            if(ctx.version != "HTTP/1.1" ) return;

            std::string connection;
            if(ctx.requestHeaders.TryGetFirst("Connection", connection))
            {
                if(HttpUtils::ToLower(connection) != "keep-alive") return;
            }

            if(ctx.responseHeaders.TryGetFirst("Connection", connection))
            {
                if(HttpUtils::ToLower(connection) != "keep-alive") return;
            }

            if(bStrm->EndOfStream()) {
                return;
            }
        }
    }

    WebSocketConnection::~WebSocketConnection()
    {

    }
    void ServerContext::StartWebSocketSession(std::function<void(std::function<void(WebSocketMessage&)>,std::function<void()>,std::function<void()>)> onOpen, std::function<void(WebSocketMessage&)> onReceive, std::function<void(bool)> onClose)
    {
        std::shared_ptr<CallbackWebSocketConnection> wsc = std::make_shared<CallbackWebSocketConnection>(onOpen,onReceive,onClose);
        StartWebSocketSession(wsc);
    }
            
    void ServerContext::StartWebSocketSession(std::shared_ptr<WebSocketConnection> connection)
    {
        WSServer svr(this,connection);
        Threading::Thread thrd([&svr,&connection]()->void{
            try {
            connection->OnOpen([&svr](WebSocketMessage& msg)->void {
                svr.send_msg(&msg);
            },[&svr]()->void {
                std::vector<uint8_t> p = {(uint8_t)'p',(uint8_t)'i',(uint8_t)'n',(uint8_t)'g'};
                svr.ping_send(p);
            },[&svr]()->void{svr.close();});
            }catch(...) {

            }
        });
        
        svr.Start();
        thrd.Join();
    }
    std::string ServerContext::GetServerRoot()
    {
        if(this->originalPath == this->path) return "/";
        Tesses::Framework::Filesystem::VFSPath originalPath=this->originalPath;

        Tesses::Framework::Filesystem::VFSPath path=this->path;
        if(originalPath.path.size() <= path.path.size()) return "/";

        originalPath.path.resize(originalPath.path.size() - path.path.size());
        return originalPath.ToString();
    }
    std::string ServerContext::MakeAbsolute(std::string path)
    {
        if(path.find("http://") == 0 || path.find("https://") == 0 || path.find("/") == 0) return path;
        Tesses::Framework::Filesystem::VFSPath path2 = GetServerRoot();
        path2 = path2 / path;
        return path2.CollapseRelativeParents().ToString();
    }


}

