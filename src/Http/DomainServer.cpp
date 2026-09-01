#include "TessesFramework/Http/DomainServer.hpp"

namespace Tesses::Framework::Http {
DomainServer::DomainServer() {}
DomainServer::DomainServer(std::shared_ptr<IHttpServer> root) : root(root) {}
void DomainServer::Set(std::string domain,
                       std::shared_ptr<IHttpServer> server) {
    mtx.Lock();
    this->servers[domain] = server;
    mtx.Unlock();
}
void DomainServer::Unset(std::string domain) {
    mtx.Lock();
    this->servers.erase(domain);
    mtx.Unlock();
}
void DomainServer::Clear() {
    mtx.Lock();
    this->servers.clear();
    mtx.Unlock();
}
bool DomainServer::Handle(ServerContext &ctx) {
    std::string host;

    std::shared_ptr<IHttpServer> server = nullptr;
    if (ctx.requestHeaders.TryGetFirst("Host", host)) {

        mtx.Lock();
        if (this->servers.count(host) > 0)
            server = this->servers[host];
        mtx.Unlock();
    }

    if (server)
        return server->Handle(ctx);
    if (this->root)
        return this->root->Handle(ctx);
    return false;
}
} // namespace Tesses::Framework::Http