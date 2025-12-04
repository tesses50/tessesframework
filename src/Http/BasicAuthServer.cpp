#include "TessesFramework/Http/BasicAuthServer.hpp"
#include "TessesFramework/Crypto/Crypto.hpp"
#include <iostream>
namespace Tesses::Framework::Http {
    
        BasicAuthServer::BasicAuthServer()
        {

        }
        BasicAuthServer::BasicAuthServer(std::shared_ptr<IHttpServer> server, std::function<bool(std::string username, std::string password)> auth,std::string realm) : server(server), authorization(auth), realm(realm)
        {

        }
        bool BasicAuthServer::Handle(ServerContext& ctx)
        {
            std::string www_authenticate = "Basic realm=\"" + this->realm + "\", charset=\"UTF-8\"";
            std::string user;
            std::string pass;
            if(!GetCreds(ctx,user,pass) || !this->authorization(user,pass)) {
                ctx.responseHeaders.SetValue("WWW-Authenticate",www_authenticate);
                ctx.statusCode = Unauthorized;
                ctx.WithMimeType("text/html").SendText("<h1>Unauthorized</h1>");
                return true;
            }

            if(this->server)
                return this->server->Handle(ctx);
            ctx.statusCode = InternalServerError;
            ctx.WithMimeType("text/html").SendText("<h1>Internal Server Error</h1>\r\n<h3>REASON: Internal server not set on Basic Auth</h3>");
            return true;
            
        }


        bool BasicAuthServer::GetCreds(ServerContext& ctx, std::string& user, std::string& pass)
        {
            std::string auth;
            if(!ctx.requestHeaders.TryGetFirst("Authorization", auth)) return false;
            
            auto security = HttpUtils::SplitString(auth," ",2);
            if(security.size() < 2) return false;
            if(security[0] != "Basic") return false;

            auto decoded = Crypto::Base64_Decode(security[1]);
            
            std::string decoded_str(decoded.begin(),decoded.end());
            security = HttpUtils::SplitString(decoded_str,":",2);
            if(security.size() < 2) return false;
            user = security[0];
            pass = security[1];
            return true;
        }
}