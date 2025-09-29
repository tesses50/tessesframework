#include "TessesFramework/Mail/Smtp.hpp"
#include "TessesFramework/Crypto/MbedHelpers.hpp"
#include "TessesFramework/Streams/MemoryStream.hpp"
#include "TessesFramework/TextStreams/StreamWriter.hpp"
#include "TessesFramework/Http/HttpUtils.hpp"
namespace Tesses::Framework::Mail
{
    static void SMTP_ATTACHMENT_WRITE(std::string& myStr, std::shared_ptr<Tesses::Framework::Streams::MemoryStream> strm)
    {
        std::string txt = Tesses::Framework::Crypto::Base64_Encode(strm->GetBuffer());
        bool first=true;
        size_t read;
        size_t offset = 0;
        do {
            if(!first) myStr.append("\r\n");
            
            read = std::min<size_t>(72, txt.size()-offset);
            
            if(read > 0)
                myStr.insert(myStr.end(),txt.begin()+offset,txt.begin()+offset+read);
            
            offset+=read;
            first=false;
        } while(read > 0);

    }

    SMTPBody::~SMTPBody()
    {

    }

    SMTPStringBody::SMTPStringBody()
    {
        
    }
    SMTPStringBody::SMTPStringBody(std::string text, std::string mimeType)
    {
        this->text = text;
        this->mimeType=mimeType;
    }
    void SMTPStringBody::Write(std::shared_ptr<Tesses::Framework::Streams::Stream> strm)
    {
        strm->WriteBlock((const uint8_t*)this->text.c_str(),this->text.size());
    }
    SMTPStreamBody::SMTPStreamBody(std::string mimeType,std::shared_ptr<Tesses::Framework::Streams::Stream> strm)
    {
        this->mimeType = mimeType;
        this->stream = strm;
    }
   
    void SMTPStreamBody::Write(std::shared_ptr<Tesses::Framework::Streams::Stream> strm)
    {
        this->stream->Seek(0L,Tesses::Framework::Streams::SeekOrigin::Begin);
        this->stream->CopyTo(strm);
    }
    SMTPStreamBody::~SMTPStreamBody()
    {
    }
    SMTPClient::SMTPClient(std::shared_ptr<Tesses::Framework::Streams::Stream> stream)
    {
        this->strm = stream;
        this->body = nullptr;
    }
    
    void SMTPClient::Send()
    {
        std::string emailHeaders = "EHLO ";
        emailHeaders.append(this->domain);
        emailHeaders.append("\r\nAUTH LOGIN\r\n");
        std::vector<uint8_t> data;
        data.insert(data.begin(), this->username.begin(),this->username.end());

        emailHeaders.append(Tesses::Framework::Crypto::Base64_Encode(data));
        emailHeaders.append("\r\n");
        data.clear();
        data.insert(data.begin(),this->password.begin(),this->password.end());
        emailHeaders.append(Tesses::Framework::Crypto::Base64_Encode(data));
        emailHeaders.append("\r\n");
        emailHeaders.append("MAIL FROM:<");
       
        emailHeaders.append(this->from);
        emailHeaders.append(">\r\n");
        
        emailHeaders.append("RCPT TO:<");
        emailHeaders.append(to);
        emailHeaders.append(">\r\n");
        
        emailHeaders.append("DATA\r\n");
        std::string boundary = "joel&<<94292025209248";
        emailHeaders.append("From: ");
        emailHeaders.append(this->from_name);
        emailHeaders.append(" <");
        emailHeaders.append(this->from);
        emailHeaders.append(">\r\nSubject: ");
        emailHeaders.append(this->subject);
        emailHeaders.append("\r\n");
        emailHeaders.append("Content-Type: multipart/mixed; boundary=");
        emailHeaders.append(boundary);
        emailHeaders.append("\r\n\r\n--");
        emailHeaders.append(boundary);
        emailHeaders.append("\r\nContent-Type: ");
        emailHeaders.append(this->body->mimeType);
        emailHeaders.append("; charset=utf-8\r\n\r\n");
        Tesses::Framework::TextStreams::StreamWriter writer(this->strm);
        writer.Write(emailHeaders);
        this->body->Write(this->strm);
        
        if(this->attachments.empty())
        {
            emailHeaders="\r\n--";
            emailHeaders.append(boundary);
            emailHeaders.append("--\r\n.\r\n");
            writer.Write(emailHeaders);
        }
        else
        {
            for(auto item : this->attachments)
            {
                emailHeaders="\r\n--";
                emailHeaders.append(boundary);
                emailHeaders.append("\r\n");
                emailHeaders.append("Content-Type: ");
                emailHeaders.append(item.second->mimeType);
                emailHeaders.append("; name=\"");
                std::string name = Tesses::Framework::Http::HttpUtils::UrlPathEncode(item.first,true);
                emailHeaders.append(name);
                emailHeaders.append("\"\r\nContent-Transfer-Encoding: base64\r\nContent-Disposition: attachment; filename=\"");
                emailHeaders.append(name);
                emailHeaders.append("\"\r\n\r\n");
                std::shared_ptr<Tesses::Framework::Streams::MemoryStream> ms = std::make_shared<Tesses::Framework::Streams::MemoryStream>(true);
                item.second->Write(ms);
                SMTP_ATTACHMENT_WRITE(emailHeaders,ms);
                //emailHeaders.append(Tesses::Framework::Crypto::Base64_Encode(ms.GetBuffer()));
                writer.Write(emailHeaders);
            }

            emailHeaders="\r\n--";
            emailHeaders.append(boundary);
            emailHeaders.append("--\r\n.\r\n");
            writer.Write(emailHeaders);
        }
    }

    SMTPClient::~SMTPClient()
    {
        
    }
}