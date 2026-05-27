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
#include "VFS.hpp"
#include "VFSFix.hpp"

namespace Tesses::Framework::Filesystem {
class LocalFilesystem : public VFS {
  public:
    std::shared_ptr<Tesses::Framework::Streams::Stream>
    OpenFile(VFSPath path, std::string mode);

    void CreateDirectory(VFSPath path);
    void DeleteDirectory(VFSPath path);

    void DeleteFile(VFSPath path);
    void CreateSymlink(VFSPath existingFile, VFSPath symlinkFile);
    VFSPathEnumerator EnumeratePaths(VFSPath path);
    void CreateHardlink(VFSPath existingFile, VFSPath newName);

    void MoveFile(VFSPath src, VFSPath dest);

    void MoveDirectory(VFSPath src, VFSPath dest);
    VFSPath ReadLink(VFSPath path);
    std::string VFSPathToSystem(VFSPath path);
    VFSPath SystemToVFSPath(std::string path);

    void SetDate(VFSPath path, Date::DateTime lastWrite,
                 Date::DateTime lastAccess);
    bool Stat(VFSPath path, StatData &stat);
    bool StatVFS(VFSPath path, StatVFSData &vfsData);

    void Chmod(VFSPath path, uint32_t mode);
    void Chown(VFSPath path, uint32_t uid, uint32_t gid);

    void Lock(VFSPath path);
    void Unlock(VFSPath path);

    FIFOCreationResult CreateFIFO(VFSPath path, uint32_t mod);

  protected:
    std::shared_ptr<FSWatcher> CreateWatcher(std::shared_ptr<VFS> vfs,
                                             VFSPath path);
};
extern std::shared_ptr<LocalFilesystem> LocalFS;
} // namespace Tesses::Framework::Filesystem
