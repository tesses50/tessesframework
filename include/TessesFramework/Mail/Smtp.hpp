#pragma once
#include "../Streams/Stream.hpp"
namespace Tesses::Framework::Mail 
{
class SMTPBody
{
    public:
        std::string mimeType;
        virtual void Write(std::shared_ptr<Tesses::Framework::Streams::Stream> strm)=0;
        virtual ~SMTPBody();
};

class SMTPStringBody : public SMTPBody
{
    public:
        SMTPStringBody();
        SMTPStringBody(std::string text, std::string mimeType);
        std::string text;
        void Write(std::shared_ptr<Tesses::Framework::Streams::Stream> strm);
};

class SMTPStreamBody : public SMTPBody 
{
    std::shared_ptr<Tesses::Framework::Streams::Stream> stream;

    public:
        SMTPStreamBody(std::string mimeType,std::shared_ptr<Tesses::Framework::Streams::Stream> stream);
        void Write(std::shared_ptr<Tesses::Framework::Streams::Stream> strm);
        ~SMTPStreamBody();
};


class SMTPClient {
    std::shared_ptr<Tesses::Framework::Streams::Stream> strm;
    
    public:
        SMTPClient(std::shared_ptr<Tesses::Framework::Streams::Stream> strm);
        std::string domain;
        std::string username;
        std::string password;
        std::string from;
        std::string from_name;
        std::string to;
        std::string subject;
        std::shared_ptr<SMTPBody> body;
        std::vector<std::pair<std::string,std::shared_ptr<SMTPBody>>> attachments;
        void Send();
        ~SMTPClient();
};

}