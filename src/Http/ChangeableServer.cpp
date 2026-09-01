/*
    TessesFramework a library to make C++ easier for me, used in CrossLang:
    https://git.tesses.org/tesses50/crosslang

    Copyright (C) 2026 Mike Nolan
    SPDX-License-Identifier: GPL-3.0-or-later WITH TessesFramework-Exception-1.0

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

#include "TessesFramework/Http/ChangeableServer.hpp"

namespace Tesses::Framework::Http {
ChangeableServer::ChangeableServer() : ChangeableServer(nullptr) {}
ChangeableServer::ChangeableServer(std::shared_ptr<IHttpServer> original) {
    this->server = original;
}

std::shared_ptr<IHttpServer> ChangeableServer::GetServer() {
    mtx.Lock();
    auto server = this->server;
    mtx.Unlock();
    return server;
}
void ChangeableServer::SetServer(std::shared_ptr<IHttpServer> server) {
    mtx.Lock();
    this->server = server;
    mtx.Unlock();
}
bool ChangeableServer::Handle(ServerContext &ctx) {
    auto server = GetServer();

    if (server)
        return server->Handle(ctx);
    return false;
}
ChangeableServer::~ChangeableServer() {}
} // namespace Tesses::Framework::Http