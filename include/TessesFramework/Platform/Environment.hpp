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
// clang-format off
#include "TessesFramework/Filesystem/LocalFS.hpp"
#include "TessesFramework/Filesystem/VFSFix.hpp"
// clang-format on
#include <optional>
namespace Tesses::Framework::Platform::Environment {
extern const char EnvPathSeperator;

struct PortableAppConfig {

    std::optional<Tesses::Framework::Filesystem::VFSPath> desktop;
    std::optional<Tesses::Framework::Filesystem::VFSPath> documents;
    std::optional<Tesses::Framework::Filesystem::VFSPath> music;
    std::optional<Tesses::Framework::Filesystem::VFSPath> pictures;
    std::optional<Tesses::Framework::Filesystem::VFSPath> videos;
    std::optional<Tesses::Framework::Filesystem::VFSPath> downloads;
    std::optional<Tesses::Framework::Filesystem::VFSPath> user;
    std::optional<Tesses::Framework::Filesystem::VFSPath> config;
    std::optional<Tesses::Framework::Filesystem::VFSPath> state;
    std::optional<Tesses::Framework::Filesystem::VFSPath> data;
    std::optional<Tesses::Framework::Filesystem::VFSPath> cache;
    std::optional<Tesses::Framework::Filesystem::VFSPath> temp;
};

extern PortableAppConfig portable_config;

namespace SpecialFolders {
Tesses::Framework::Filesystem::VFSPath GetTemp();
Tesses::Framework::Filesystem::VFSPath GetHomeFolder();
Tesses::Framework::Filesystem::VFSPath GetDownloads();
Tesses::Framework::Filesystem::VFSPath GetMusic();
Tesses::Framework::Filesystem::VFSPath GetPictures();
Tesses::Framework::Filesystem::VFSPath GetVideos();
Tesses::Framework::Filesystem::VFSPath GetDocuments();
Tesses::Framework::Filesystem::VFSPath GetConfig();
Tesses::Framework::Filesystem::VFSPath GetDesktop();
Tesses::Framework::Filesystem::VFSPath GetState();
Tesses::Framework::Filesystem::VFSPath GetCache();
Tesses::Framework::Filesystem::VFSPath GetData();
} // namespace SpecialFolders

std::string GetPlatform();

Tesses::Framework::Filesystem::VFSPath
GetRealExecutablePath(Tesses::Framework::Filesystem::VFSPath realPath);

std::optional<std::string> GetVariable(std::string name);
void SetVariable(std::string name, std::optional<std::string> var);

void GetEnvironmentVariables(
    std::vector<std::pair<std::string, std::string>> &env);
} // namespace Tesses::Framework::Platform::Environment
