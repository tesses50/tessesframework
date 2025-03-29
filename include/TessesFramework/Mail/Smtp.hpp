#pragma once
#include "../Streams/Stream.hpp"
namespace Tesses::Framework::Mail 
{
class SMTPBody
{
    public:
        std::string mimeType;
        virtual void Write(Tesses::Framework::Streams::Stream* strm)=0;
        virtual ~SMTPBody();
};

class SMTPStringBody : public SMTPBody
{
    public:
        SMTPStringBody();
        SMTPStringBody(std::string text, std::string mimeType);
        std::string text;
        void Write(Tesses::Framework::Streams::Stream* strm);
};

class SMTPStreamBody : public SMTPBody 
{
    Tesses::Framework::Streams::Stream* stream;
    bool owns;
    public:
        SMTPStreamBody(std::string mimeType,Tesses::Framework::Streams::Stream& stream);
        SMTPStreamBody(std::string mimeType,Tesses::Framework::Streams::Stream* stream, bool owns);
        void Write(Tesses::Framework::Streams::Stream* strm);
        ~SMTPStreamBody();
};


class SMTPClient {
    Tesses::Framework::Streams::Stream* strm;
    bool owns;
    public:
        SMTPClient(Tesses::Framework::Streams::Stream* stream,bool owns=true);
        SMTPClient(Tesses::Framework::Streams::Stream& strm);
        std::string domain;
        std::string username;
        std::string password;
        std::string from;
        std::string from_name;
        std::string to;
        std::string subject;
        SMTPBody* body;
        std::vector<std::pair<std::string,SMTPBody*>> attachments;
        void Send();
        ~SMTPClient();
};

}