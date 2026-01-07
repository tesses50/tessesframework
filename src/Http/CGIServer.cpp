#include "TessesFramework/Http/CGIServer.hpp"
#include "TessesFramework/Filesystem/LocalFS.hpp"
#include "TessesFramework/Platform/Process.hpp"
#include "TessesFramework/Http/BasicAuthServer.hpp"
#include "TessesFramework/TextStreams/StreamReader.hpp"
#include <iostream>
namespace Tesses::Framework::Http {
    CGIServer::CGIServer(Tesses::Framework::Filesystem::VFSPath dir)
    {
        this->dir = dir;
    }
    bool CGIServer::Handle(ServerContext& ctx)
    {
        Tesses::Framework::Filesystem::VFSPath execPath = ctx.path;
        execPath.relative=true;
        CGIParams params;
        params.document_root = this->document_root ? *this->document_root : this->dir;
        params.adminEmail = this->adminEmail;
        params.workingDirectory = this->workingDirectory;
        params.program = this->dir / execPath.CollapseRelativeParents();
       
        return ServeCGIRequest(ctx,params);
    }
    bool CGIServer::ServeCGIRequest(ServerContext& ctx, CGIParams& params)
    {
        using namespace Tesses::Framework::Filesystem;
        auto program = params.program.MakeAbsolute();
        if(!LocalFS->FileExists(program)) return false;
        Tesses::Framework::Platform::Process p;

        Tesses::Framework::Filesystem::VFSPath p0=ctx.originalPath;
        
        p.env.emplace_back("SCRIPT_FILENAME",LocalFS->VFSPathToSystem(program));
        p.env.emplace_back("SCRIPT_NAME",p0.CollapseRelativeParents().ToString());
        if(ctx.encrypted)
            p.env.emplace_back("HTTPS","on");
        
        std::string query;
        for(auto& item : ctx.queryParams.kvp)
        {
            for(auto& val : item.second)
            {
                if(!query.empty()) query += "&";

                query += HttpUtils::UrlEncode(item.first);
                query += "=";
                query += HttpUtils::UrlEncode(val);
            }
        }
        p.env.emplace_back("QUERY_STRING",query);
        p.env.emplace_back("REQUEST_URI",ctx.GetOriginalPathWithQuery());
        p.env.emplace_back("REQUEST_METHOD",ctx.method);
        p.env.emplace_back("REMOTE_HOST",ctx.ip);
        p.env.emplace_back("REMOTE_ADDR",ctx.ip);
        p.env.emplace_back("REMOTE_PORT",std::to_string(ctx.port));
        std::string user;
        std::string pass;
        if(BasicAuthServer::GetCreds(ctx,user,pass))
            p.env.emplace_back("REMOTE_USER",user);
        p.env.emplace_back("SERVER_SOFTWARE","TessesFrameworkWebServer");
        p.env.emplace_back("SERVER_PORT",std::to_string(ctx.serverPort));
        p.env.emplace_back("GATEWAY_INTERFACE","CGI/1.1");
        p.env.emplace_back("SERVER_PROTOCOL",ctx.version);
        
        if(params.document_root)
            p.env.emplace_back("DOCUMENT_ROOT",params.document_root->ToString());
        if(params.adminEmail)
            p.env.emplace_back("SERVER_ADMIN",*params.adminEmail);

        for(auto& hdr : ctx.requestHeaders.kvp)
        {
            std::string hdr_name = HttpUtils::ToUpper(hdr.first);
            if(hdr_name == "CONTENT-LENGTH")
            {
                if(!hdr.second.empty())
                p.env.emplace_back("CONTENT_LENGTH",hdr.second.front());
            }
            else if(hdr_name == "CONTENT-TYPE")
            {
                if(!hdr.second.empty())
                p.env.emplace_back("CONTENT_LENGTH",hdr.second.front());
            }
            else {
                
                if(!hdr.second.empty())
                p.env.emplace_back("HTTP_"+hdr.first,hdr.second.front());
            }
        }
        p.redirectStdIn=true;
        p.redirectStdOut=true;
        p.name = program.ToString();

        
        if(params.workingDirectory)
        {
            p.workingDirectory = params.workingDirectory->MakeAbsolute().ToString();
        }

        if(p.Start())
        {
            auto strm = p.GetStdinStream();
            if(ctx.method != "GET") ctx.ReadStream(strm);
            p.CloseStdInNow();
            auto stout = p.GetStdoutStream();
            Tesses::Framework::TextStreams::StreamReader reader(stout);
            std::string line;
            while(reader.ReadLineHttp(line))
            {
                auto v = HttpUtils::SplitString(line,": ", 2);
                if(v.size() == 2)
                {
                    if(HttpUtils::ToLower(v[0]) == "status")
                    {
                        auto v2 = HttpUtils::SplitString(v[1]," ",2);
                        if(v2.empty())
                        {
                            ctx.statusCode = StatusCode::InternalServerError;
                            throw std::runtime_error("Status response is empty");
                        }
                        ctx.statusCode= (StatusCode)std::stoi(v2[0]);
                    }
                    else {
                        ctx.responseHeaders.AddValue(v[0],v[1]);
                    }
                }
                else throw std::runtime_error("Corrupted header: " + line);
                line.clear();
            }
            
            ctx.SendStream(stout);
            return true;
        }
        return false;
    }
}