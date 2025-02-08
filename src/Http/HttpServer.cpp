#include "TessesFramework/Http/HttpServer.hpp"
#include "TessesFramework/Streams/FileStream.hpp"
#include "TessesFramework/Streams/MemoryStream.hpp"
#include "TessesFramework/TextStreams/StreamWriter.hpp"
#include "TessesFramework/TextStreams/StreamReader.hpp"
#include "TessesFramework/Http/ContentDisposition.hpp"
#include "TessesFramework/Streams/BufferedStream.hpp"
#include "TessesFramework/Http/HttpStream.hpp"
#include "TessesFramework/Crypto/MbedHelpers.hpp"
#include "TessesFramework/Threading/Mutex.hpp"

#include <iostream>
using FileStream = Tesses::Framework::Streams::FileStream;
using Stream = Tesses::Framework::Streams::Stream;
using SeekOrigin = Tesses::Framework::Streams::SeekOrigin;
using MemoryStream = Tesses::Framework::Streams::MemoryStream;
using StreamReader = Tesses::Framework::TextStreams::StreamReader;
using StreamWriter = Tesses::Framework::TextStreams::StreamWriter;
using TcpServer = Tesses::Framework::Streams::TcpServer;
using NetworkStream = Tesses::Framework::Streams::NetworkStream;
using BufferedStream = Tesses::Framework::Streams::BufferedStream;



namespace Tesses::Framework::Http
{
    class WSServer 
    {
        public:
        Threading::Mutex mtx;
        ServerContext* ctx;
        WebSocketConnection* conn;
        Stream* strm;
        std::atomic_bool hasInit;
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
        
            WSServer(ServerContext* ctx,WebSocketConnection* conn)
            {
                this->ctx = ctx;
                this->conn = conn;
                this->strm = &this->ctx->GetStream();
                this->hasInit=false;

                
            }
            void Start()
            {
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

                while(!strm->EndOfStream())
                {
                
                    uint8_t frame_start[2];
                    if(strm->ReadBlock(frame_start,2) != 2) return;
                    
                    
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
   void ServerContext::ReadStream(Stream* strm)
   {
        if(strm == nullptr) return;
        auto strm2 = this->OpenRequestStream();
        if(strm2 != nullptr)
        {
            strm2->CopyTo(strm);
            delete strm2;
        }
   }
   void ServerContext::ReadStream(Stream& strm)
   {
        ReadStream(&strm);
   }
   std::string ServerContext::ReadString()
   {
        if(strm == nullptr) return {};
        auto strm2 = this->OpenRequestStream();
        if(strm2 != nullptr)
        {
            TextStreams::StreamReader reader(strm2,true);
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
    static bool parseUntillBoundaryEnd(Tesses::Framework::Streams::Stream* src, Tesses::Framework::Streams::Stream* dest, std::string boundary)
    {
        bool hasMore=true;
        uint8_t checkBuffer[boundary.size()];
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
        return hasMore;
    }

    static bool parseSection(ServerContext* ctx, std::string boundary, std::function<Tesses::Framework::Streams::Stream*(std::string mime, std::string filename, std::string name)> cb)
    {
        HttpDictionary req;
        StreamReader reader(ctx->GetStream());
        std::string line;
        while(reader.ReadLine(line))
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
                MemoryStream ms(true);

                bool retVal = parseUntillBoundaryEnd(&ctx->GetStream(),&ms,boundary);
                auto& buff = ms.GetBuffer();

                ctx->queryParams.AddValue(cd1.fieldName, std::string(buff.begin(),buff.end()));

                return retVal;

            }
            else
            {
                auto strm = cb(ct, cd1.filename, cd1.fieldName); 
                bool retVal = parseUntillBoundaryEnd(&ctx->GetStream(),strm,boundary);
                delete strm;
                return retVal;
            }
        }
        return false;
    }

    void ServerContext::ParseFormData(std::function<Tesses::Framework::Streams::Stream*(std::string mime, std::string filename, std::string name)> cb)
    {
        std::string ct;
        if(this->requestHeaders.TryGetFirst("Content-Type",ct))
        {
            if(ct.find("multipart/form-data") != 0)
            {
                std::cout << "Not form data" << std::endl;
                return;
            }
            auto res = ct.find("boundary=");
            if(res == std::string::npos) return;
            ct = "--" + ct.substr(res+9);
        }
        Stream nullStrm;
        parseUntillBoundaryEnd(this->strm,&nullStrm, ct);

        while(parseSection(this, ct, cb));
    }   
            
    HttpServer::HttpServer(uint16_t port, IHttpServer* http, bool owns)
    {
        this->server = new TcpServer(port, 10);
        this->http = http;
        this->owns = owns;
        this->thrd=nullptr;
        this->port = port;
    }
    HttpServer::HttpServer(uint16_t port, IHttpServer& http) : HttpServer(port,&http,false)
    {

    }
    Stream* ServerContext::OpenResponseStream()
    {
        if(sent) return nullptr;
        int64_t length = -1;
        if(!this->responseHeaders.TryGetFirstInt("Content-Length",length))
            length = -1;
        
        if(this->version == "HTTP/1.1" && length == -1)
            this->responseHeaders.SetValue("Transfer-Encoding","chunked");

        this->WriteHeaders();
        return new HttpStream(this->strm,false,length,false,version == "HTTP/1.1");
    }
    Stream* ServerContext::OpenRequestStream()
    {
        int64_t length = -1;
        if(!this->requestHeaders.TryGetFirstInt("Content-Length",length))
            length = -1;
        return new HttpStream(this->strm,false,length,true,version == "HTTP/1.1");
    }
    void HttpServer::StartAccepting()
    {
        fflush(stdout);
        if(http == nullptr) return;
        auto svr=this->server;
        auto http = this->http;
        thrd = new Threading::Thread([svr,http]()->void {
            while(TF_IsRunning())
            {
                std::string ip;
                uint16_t port;
                auto sock =svr->GetStream(ip,port);
                if(sock == nullptr) return;
                Threading::Thread thrd2([sock,http,ip,port]()->void {
                    HttpServer::Process(*sock,*http,ip,port,false);
                    delete sock;
                });
                thrd2.Detach();
            }
        });
        std::cout << "\e[34mInterfaces:\n";
        for(auto _ip : NetworkStream::GetIPs())
        {
            std::cout << "\e[32m";
            std::cout << _ip.first << ": ";
            std::cout << "\e[35mhttp://";
            std::cout << _ip.second << ":" << std::to_string(this->port) << "/\n";
        }
        std::cout << "\e[31mAlmost Ready to Listen\e[39m\n";
    }
    HttpServer::~HttpServer()
    {
        this->server->Close();
        TF_ConnectToSelf(this->port);
        if(this->thrd != nullptr)
        {
            this->thrd->Join();
            delete this->thrd;
        }
        if(this->owns)
            delete http;
        delete this->server;
    }
    IHttpServer::~IHttpServer()
    {
    
    }
    ServerContext::ServerContext(Stream* strm)
    {
        this->statusCode = OK;
        this->strm = strm;
        this->sent = false;
        this->responseHeaders.AddValue("Server","TessesFrameworkWebServer");
    }
    Stream& ServerContext::GetStream()
    {
        return *this->strm;
    }
    void ServerContext::SendBytes(std::vector<uint8_t> buff)
    {
        MemoryStream strm(false);
        strm.GetBuffer() = buff;
        SendStream(strm);
    }
    void ServerContext::SendText(std::string text)
    {
        MemoryStream strm(false);
        auto& buff= strm.GetBuffer();
        buff.insert(buff.end(),text.begin(),text.end());
        SendStream(strm);
    }
    void ServerContext::SendErrorPage(bool showPath)
    {
        if(sent) return;
        std::string errorHtml = showPath ? ("<html><head><title>File " + HttpUtils::HtmlEncode(this->originalPath) + " " + HttpUtils::StatusCodeString(this->statusCode) + "</title></head><body><h1>" + std::to_string((int)this->statusCode) + " " + HttpUtils::StatusCodeString(this->statusCode) + "</h1><h4>" + HttpUtils::HtmlEncode(this->originalPath) + "</h4></body></html>") : "";

        WithMimeType("text/html").SendText(errorHtml);
    }
    void ServerContext::SendStream(Stream* strm)
    {
        if(strm == nullptr) return;
        SendStream(*strm);
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
    void ServerContext::SendStream(Stream& strm)
    {
        if(sent) return;
        if(!strm.CanRead()) throw TextException("Cannot read from stream");
        if(strm.EndOfStream()) throw TextException("End of stream");
        if(strm.CanSeek())
        {
            int64_t len=strm.GetLength();
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
                    strm.Seek(begin,SeekOrigin::Begin);

                    uint8_t buffer[1024];

                    size_t read=0;
                    do {
                        read = sizeof(buffer);
                        myLen = (end - begin)+1;
                        if(myLen < read) read = (size_t)myLen;
                        if(read == 0) break;
                        read = strm.Read(buffer,read);
                        if(read == 0) break;
                        this->strm->WriteBlock(buffer,read);

                        begin += read;
                    } while(read > 0);

                    
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
                    strm.CopyTo(*this->strm);
                }
            }

        }
        else
        {
            auto chunkedStream = this->OpenResponseStream();
            this->strm->CopyTo(chunkedStream);
            delete chunkedStream;
            
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

        StreamWriter writer(this->strm,false);
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
    void HttpServer::Process(Stream& strm, IHttpServer& server, std::string ip, uint16_t port, bool encrypted)
    {
        
        while(true)
        {
            BufferedStream bStrm(strm);
            StreamReader reader(bStrm);
            ServerContext ctx(&bStrm);
            ctx.ip = ip;
            ctx.port = port;
            ctx.encrypted = encrypted;
            try{
            bool firstLine = true;
            std::string line;
            while(reader.ReadLine(line))
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
                len = bStrm.ReadBlock(buffer,len);
                std::string query((const char*)buffer,len);
                delete[] buffer;
                HttpUtils::QueryParamsDecode(ctx.queryParams, query);
            }
            
                if(!server.Handle(ctx)) 
                {
                    ctx.SendNotFound();
                }
            } 
            catch(std::exception& ex)
            {
                ctx.SendException(ex);
            }
            catch(...)
            {
                TextException ex("An unknown error occurred");
                ctx.SendException(ex);   
            }

            if(ctx.version != "HTTP/1.1" ) return;

            std::string connection;
            if(ctx.requestHeaders.TryGetFirst("Connection", connection))
            {
                if(connection != "keep-alive") return;
            }

            if(ctx.responseHeaders.TryGetFirst("Connection", connection))
            {
                if(connection != "keep-alive") return;
            }

            if(bStrm.EndOfStream()) return;
        }
    }

    WebSocketConnection::~WebSocketConnection()
    {

    }
    void ServerContext::StartWebSocketSession(std::function<void(std::function<void(WebSocketMessage&)>,std::function<void()>)> onOpen, std::function<void(WebSocketMessage&)> onReceive, std::function<void(bool)> onClose)
    {
        CallbackWebSocketConnection wsc(onOpen,onReceive,onClose);
        StartWebSocketSession(wsc);
    }
            
    void ServerContext::StartWebSocketSession(WebSocketConnection& connection)
    {
        WSServer svr(this,&connection);
        Threading::Thread thrd([&svr,&connection]()->void{
            try {
            connection.OnOpen([&svr](WebSocketMessage& msg)->void {
                svr.send_msg(&msg);
            },[&svr]()->void {
                std::vector<uint8_t> p = {(uint8_t)'p',(uint8_t)'i',(uint8_t)'n',(uint8_t)'g'};
                svr.ping_send(p);
            });
            }catch(...) {

            }
        });
        
        svr.Start();
        thrd.Join();
    }

    CallbackWebSocketConnection::CallbackWebSocketConnection()
    {

    }
    CallbackWebSocketConnection::CallbackWebSocketConnection(std::function<void(std::function<void(WebSocketMessage&)>,std::function<void()>)> onOpen, std::function<void(WebSocketMessage&)> onReceive, std::function<void(bool)> onClose)
    {
        this->onOpen = onOpen;
        this->onReceive = onReceive;
        this->onClose = onClose;
    }

    void CallbackWebSocketConnection::OnOpen(std::function<void(WebSocketMessage&)> sendMessage, std::function<void()> ping)
    {
        if(this->onOpen)
            this->onOpen(sendMessage,ping);
    }
    void CallbackWebSocketConnection::OnReceive(WebSocketMessage& message)
    {
        if(this->onReceive)
            this->onReceive(message);
    }
    void CallbackWebSocketConnection::OnClose(bool clean)
    {
        if(this->onClose)
            this->onClose(clean);
    }


    WebSocketMessage::WebSocketMessage()
    {
        this->isBinary=false;
        this->data={};
    }
    WebSocketMessage::WebSocketMessage(std::vector<uint8_t> data)
    {
        this->isBinary = true;
        this->data = data;
    }
    WebSocketMessage::WebSocketMessage(const void* data, size_t len)
    {
        this->isBinary=true;
        this->data={};
        this->data.insert(this->data.end(),(uint8_t*)data,((uint8_t*)data)+len);
    }
    WebSocketMessage::WebSocketMessage(std::string message)
    {
        this->isBinary=false;
        this->data={};
        this->data.insert(this->data.end(),message.begin(), message.end());
    }
    std::string WebSocketMessage::ToString()
    {
        std::string str = {};
        str.insert(str.end(),this->data.begin(),this->data.end());
        return str;
    }
}
