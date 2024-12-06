#pragma once
#include "../Filesystem/VFS.hpp"
#include "HttpServer.hpp"

namespace Tesses::Framework::Http
{
    class FileServer : public IHttpServer
    {
        Tesses::Framework::Filesystem::VFS* vfs;
        bool ownsVFS;
    

        bool SendFile(ServerContext& ctx,Tesses::Framework::Filesystem::VFSPath path);
        public:
            bool allowListing;
            bool spa;
            std::vector<std::string> defaultNames;
            FileServer(std::filesystem::path path,bool allowListing,bool spa);
            FileServer(std::filesystem::path path,bool allowListing, bool spa, std::vector<std::string> defaultNames);
            FileServer(Tesses::Framework::Filesystem::VFS* fs, bool owns, bool allowListing, bool spa);
            FileServer(Tesses::Framework::Filesystem::VFS* fs, bool owns, bool allowListing, bool spa, std::vector<std::string> defaultNames);
            bool Handle(ServerContext& ctx);
            ~FileServer();
    };
}