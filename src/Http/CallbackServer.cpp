#include "TessesFramework/Http/CallbackServer.hpp"

namespace Tesses::Framework::Http
{
    CallbackServer::CallbackServer(std::function<bool(ServerContext&)> cb) : CallbackServer(cb,[]()->void{})
    {

    }
    CallbackServer::CallbackServer(std::function<bool(ServerContext&)> cb,std::function<void()> destroy)
    {
        this->cb = cb;
        this->destroy=destroy;
    }
    bool CallbackServer::Handle(ServerContext& ctx)
    {
        return this->cb(ctx);
    }
    CallbackServer::~CallbackServer()
    {
        this->destroy();
    }
}