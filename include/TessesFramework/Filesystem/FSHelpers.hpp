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
#include "TempFS.hpp"
#include "VFSFix.hpp"

namespace Tesses::Framework::Filesystem::Helpers {
void ReadAllText(std::shared_ptr<VFS> vfs, VFSPath path, std::string &text);
void ReadAllLines(std::shared_ptr<VFS> vfs, VFSPath path,
                  std::vector<std::string> &lines);
void ReadAllBytes(std::shared_ptr<VFS> vfs, VFSPath path,
                  std::vector<uint8_t> &array);
std::string ReadAllText(std::shared_ptr<VFS> vfs, VFSPath path);
std::vector<std::string> ReadAllLines(std::shared_ptr<VFS> vfs, VFSPath path);
std::vector<uint8_t> ReadAllBytes(std::shared_ptr<VFS> vfs, VFSPath path);
void WriteAllText(std::shared_ptr<VFS> vfs, VFSPath path,
                  const std::string &text);
void WriteAllLines(std::shared_ptr<VFS> vfs, VFSPath path,
                   const std::vector<std::string> &parts);
void WriteAllBytes(std::shared_ptr<VFS> vfs, VFSPath path,
                   const std::vector<uint8_t> &bytes);

void CopyStreamProgress(
    std::shared_ptr<Streams::Stream> src, std::shared_ptr<Streams::Stream> dest,
    std::function<void(int64_t offset, int64_t length)> progress);

void CopyFile(std::shared_ptr<VFS> vfsSrc, VFSPath pathSrc,
              std::shared_ptr<VFS> vfsDest, VFSPath pathDest,
              bool overwrite = true);
void CopyFile(std::shared_ptr<VFS> vfsSrc, VFSPath pathSrc,
              std::shared_ptr<VFS> vfsDest, VFSPath pathDest,
              std::function<void(int64_t offset, int64_t length)> progress,
              bool overwrite = true);
void CopyDirectory(std::shared_ptr<VFS> vfsSrc, VFSPath pathSrc,
                   std::shared_ptr<VFS> vfsDest, VFSPath pathDest,
                   bool overwrite = true);
void CopyDirectory(
    std::shared_ptr<VFS> vfsSrc, VFSPath pathSrc, std::shared_ptr<VFS> vfsDest,
    VFSPath pathDest,
    std::function<void(int64_t offset, int64_t length, VFSPath currentFile)>
        progress,
    bool overwrite = true);

inline std::shared_ptr<TempFS> CreateTempFS(bool deleteOnDestroy = true) {
    return std::make_shared<TempFS>(deleteOnDestroy);
}
inline std::shared_ptr<TempFS> CreateTempFS(std::shared_ptr<VFS> vfs,
                                            bool deleteOnDestroy = true) {
    return std::make_shared<TempFS>(vfs, deleteOnDestroy);
}

} // namespace Tesses::Framework::Filesystem::Helpers