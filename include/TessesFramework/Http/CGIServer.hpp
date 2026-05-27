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
    bool Handle(ServerContext &ctx);
    static bool ServeCGIRequest(ServerContext &ctx, CGIParams &params);
};
} // namespace Tesses::Framework::Http