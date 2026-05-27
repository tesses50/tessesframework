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

#include "TessesFramework/Http/ChangeableServer.hpp"

namespace Tesses::Framework::Http {
ChangeableServer::ChangeableServer() : ChangeableServer(nullptr) {}
ChangeableServer::ChangeableServer(std::shared_ptr<IHttpServer> original) {
    this->server = original;
}

bool ChangeableServer::Handle(ServerContext &ctx) {
    if (this->server)
        this->server->Handle(ctx);
    return false;
}
ChangeableServer::~ChangeableServer() {}
} // namespace Tesses::Framework::Http