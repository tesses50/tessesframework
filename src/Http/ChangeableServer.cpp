#include "TessesFramework/Http/ChangeableServer.hpp"

namespace Tesses::Framework::Http {
ChangeableServer::ChangeableServer() : ChangeableServer(nullptr)
{

}
ChangeableServer::ChangeableServer(std::shared_ptr<IHttpServer> original)
{
    this->server = original;
}

bool ChangeableServer::Handle(ServerContext& ctx)
{
    if(this->server) this->server->Handle(ctx);
    return false;
}
ChangeableServer::~ChangeableServer()
{

}
}