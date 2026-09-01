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

#pragma once
#include "../Filesystem/VFS.hpp"
#include "../Filesystem/VFSFix.hpp"
#include "HttpServer.hpp"

namespace Tesses::Framework::Http {
class DomainServer : public IHttpServer {
    std::shared_ptr<IHttpServer> root;
    std::map<std::string, std::shared_ptr<IHttpServer>> servers;
    Tesses::Framework::Threading::Mutex mtx;

  public:
    DomainServer();
    DomainServer(std::shared_ptr<IHttpServer> root);
    void Set(std::string domain, std::shared_ptr<IHttpServer> server);
    void Unset(std::string domain);
    void Clear();
    bool Handle(ServerContext &ctx);
};
} // namespace Tesses::Framework::Http