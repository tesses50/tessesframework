#pragma once
#include "../Filesystem/VFS.hpp"
#include "../Filesystem/VFSFix.hpp"
#include "HttpServer.hpp"
#include <optional>
namespace Tesses::Framework::Http {
    struct CGIParams {
        std::optional<Tesses::Framework::Filesystem::VFSPath> document_root;
        Tesses::Framework::Filesystem::VFSPath program;
        std::optional<std::string> adminEmail;
        std::optional<Tesses::Framework::Filesystem::VFSPath> workingDirectory;
    };
    class CGIServer : public Tesses::Framework::Http::IHttpServer {
        Tesses::Framework::Filesystem::VFSPath dir;
        public:
            std::optional<Tesses::Framework::Filesystem::VFSPath> document_root;
            std::optional<std::string> adminEmail;
            std::optional<Tesses::Framework::Filesystem::VFSPath> workingDirectory;
            CGIServer(Tesses::Framework::Filesystem::VFSPath dir);
            bool Handle(ServerContext& ctx);
            static bool ServeCGIRequest(ServerContext& ctx, CGIParams& params);
    };
}