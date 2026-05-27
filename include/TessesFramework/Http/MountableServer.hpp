/*
    TessesFramework a library to make C++ easier for me, used in CrossLang:
   https://git.tesses.org/tesses50/crosslang Copyright (C) 2026 Mike Nolan

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once
#include "../Filesystem/VFS.hpp"
#include "../Filesystem/VFSFix.hpp"
#include "HttpServer.hpp"

namespace Tesses::Framework::Http {
class MountableServer : public IHttpServer {
    std::shared_ptr<IHttpServer> root;
    std::vector<std::pair<std::string, std::shared_ptr<IHttpServer>>> servers;
    std::string Subpath(Filesystem::VFSPath fullPath,
                        Filesystem::VFSPath offsetPath);
    bool StartsWith(Filesystem::VFSPath fullPath,
                    Filesystem::VFSPath offsetPath);

  public:
    MountableServer();
    MountableServer(std::shared_ptr<IHttpServer> root);
    void Mount(std::string path, std::shared_ptr<IHttpServer> server);
    void Unmount(std::string path);
    bool Handle(ServerContext &ctx);
    ~MountableServer();
};
} // namespace Tesses::Framework::Http