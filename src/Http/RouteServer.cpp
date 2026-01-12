#include "TessesFramework/Http/RouteServer.hpp"

namespace Tesses::Framework::Http
{
    
    RouteServer::RouteServerRoute::RouteServerRoute(std::string route, std::string method, ServerRequestHandler handler) : method(method), handler(handler)
    {
        auto path = Tesses::Framework::Filesystem::VFSPath::ParseUriPath(route);
        for(auto item : path.path) 
        {
            if(item.size() > 2 && item[0] == '{' && item[item.size()-1] == '}')
            {
                this->parts.emplace_back( item.substr(1,item.size()-2),true);
            }
            else {
                this->parts.emplace_back(item,false);
            }
        }
    }
    bool RouteServer::RouteServerRoute::Equals(Tesses::Framework::Filesystem::VFSPath& path, HttpDictionary& args)
    {
        if(path.path.size() != this->parts.size()) return false;
        
        
        for(size_t i = 0; i < this->parts.size(); i++)
        {
            auto& part = this->parts[i];
            if(part.second)
                args.SetValue(part.first, Tesses::Framework::Http::HttpUtils::UrlPathDecode(path.path[i]));
            else if(part.first != path.path[i]) return false;

        }

        
        return true;
    }

    RouteServer::RouteServer(std::shared_ptr<IHttpServer> root) : root(root)
    {

    }

    void RouteServer::Add(std::string method, std::string pattern, ServerRequestHandler handler)
    {
        this->routes.emplace_back(pattern,method,handler);
    }

    bool RouteServer::Handle(ServerContext& ctx)
    {
        auto pathArgs = ctx.pathArguments;
        auto path = Tesses::Framework::Filesystem::VFSPath::ParseUriPath(ctx.path);
        for(auto& svr : this->routes)
        {
            if(svr.method != ctx.method && !((svr.method == "GET" && ctx.method == "HEAD") || (svr.method == "HEAD" && ctx.method == "GET"))) continue;
            ctx.pathArguments = pathArgs;
            if(svr.Equals(path, ctx.pathArguments) && svr.handler && svr.handler(ctx))
                return true;
            
        }
        ctx.pathArguments = pathArgs;

        if(this->root)
            return this->root->Handle(ctx);
        return false;
    }

     void RouteServer::Get(std::string pattern, ServerRequestHandler handler)
     {
        Add("GET",pattern,handler);
     }
     void RouteServer::Post(std::string pattern, ServerRequestHandler handler)
     {
        Add("POST",pattern,handler);
     }
     void RouteServer::Put(std::string pattern, ServerRequestHandler handler)
     {
        Add("PUT",pattern,handler);
     }
     void RouteServer::Patch(std::string pattern, ServerRequestHandler handler)
     {
        Add("PATCH",pattern,handler);
     }

     void RouteServer::Delete(std::string pattern, ServerRequestHandler handler)
     {
        Add("DELETE",pattern,handler);
     }

     void RouteServer::Trace(std::string pattern, ServerRequestHandler handler)
     {
        Add("TRACE",pattern,handler);
     }
     void RouteServer::Options(std::string pattern, ServerRequestHandler handler)
     {
        Add("OPTIONS",pattern,handler);
     }
}