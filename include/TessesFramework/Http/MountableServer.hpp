#pragma once
#include "HttpServer.hpp"
#include "../Filesystem/VFSFix.hpp"
#include "../Filesystem/VFS.hpp"

namespace Tesses::Framework::Http
{
    class MountableServer : public IHttpServer
    {
        std::shared_ptr<IHttpServer> root;
        std::vector<std::pair<std::string,std::shared_ptr<IHttpServer>>> servers;
        std::string Subpath(Filesystem::VFSPath fullPath, Filesystem::VFSPath offsetPath);
        bool StartsWith(Filesystem::VFSPath fullPath, Filesystem::VFSPath offsetPath);
        public:
            MountableServer();
            MountableServer(std::shared_ptr<IHttpServer> root);
            void Mount(std::string path, std::shared_ptr<IHttpServer> server);
            void Unmount(std::string path);
            bool Handle(ServerContext& ctx);
            ~MountableServer();
    };
}