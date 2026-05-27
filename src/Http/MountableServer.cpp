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

#include "TessesFramework/Http/MountableServer.hpp"

namespace Tesses::Framework::Http {
std::string MountableServer::Subpath(Filesystem::VFSPath fullPath,
                                     Filesystem::VFSPath offsetPath) {
    if (fullPath.path.size() < offsetPath.path.size())
        return {}; // this shouldn't happen but here just in case
    Filesystem::VFSPath p;
    p.relative = false;

    for (size_t i = offsetPath.path.size(); i < fullPath.path.size(); i++) {
        p.path.push_back(fullPath.path[i]);
    }
    return p.ToString();
}
bool MountableServer::StartsWith(Filesystem::VFSPath fullPath,
                                 Filesystem::VFSPath offsetPath) {
    if (fullPath.path.size() < offsetPath.path.size())
        return false;
    for (size_t i = 0; i < offsetPath.path.size(); i++) {
        if (fullPath.path[i] != offsetPath.path[i])
            return false;
    }
    return true;
}
MountableServer::MountableServer() : MountableServer(nullptr) {}
MountableServer::MountableServer(std::shared_ptr<IHttpServer> root) {
    this->root = root;
}

void MountableServer::Mount(std::string path,
                            std::shared_ptr<IHttpServer> server) {
    this->servers.insert(
        this->servers.begin(),
        std::pair<std::string, std::shared_ptr<IHttpServer>>(path, server));
}
void MountableServer::Unmount(std::string path) {
    for (auto i = this->servers.begin(); i != this->servers.end(); i++) {
        auto &item = *i;
        if (item.first == path) {
            this->servers.erase(i);
            return;
        }
    }
}
bool MountableServer::Handle(ServerContext &ctx) {
    std::string oldPath = ctx.path;
    for (auto item : this->servers) {
        if (StartsWith(oldPath, item.first)) {
            ctx.path = Subpath(oldPath, item.first);
            if (item.second->Handle(ctx)) {
                ctx.path = oldPath;
                return true;
            }
            ctx.path = oldPath;
            break;
        }
    }
    ctx.path = oldPath;
    if (this->root && this->root->Handle(ctx))
        return true;
    return false;
}
MountableServer::~MountableServer() {}
} // namespace Tesses::Framework::Http
