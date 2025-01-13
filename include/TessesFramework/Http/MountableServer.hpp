#pragma once
#include "HttpServer.hpp"
#include "../Filesystem/VFSFix.hpp"
#include "../Filesystem/VFS.hpp"

namespace Tesses::Framework::Http
{
    class MountableServer : public IHttpServer
    {
        IHttpServer* root;
        bool owns;
        std::vector<std::pair<std::string,std::pair<bool,IHttpServer*>>> servers;
        std::string Subpath(Filesystem::VFSPath fullPath, Filesystem::VFSPath offsetPath);
        bool StartsWith(Filesystem::VFSPath fullPath, Filesystem::VFSPath offsetPath);
        public:
            MountableServer();
            MountableServer(IHttpServer* root, bool owns);
            MountableServer(IHttpServer& root);
            void Mount(std::string path, IHttpServer* server, bool owns);
            void Mount(std::string path, IHttpServer& server);
            void Unmount(std::string path);
            bool Handle(ServerContext& ctx);
            ~MountableServer();
    };
}