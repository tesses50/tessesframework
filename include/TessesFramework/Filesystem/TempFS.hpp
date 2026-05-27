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
#include "../Common.hpp"
#include "VFS.hpp"
#include "VFSFix.hpp"
namespace Tesses::Framework::Filesystem {
void UniqueString(std::string &text);

class TempFS : public VFS {
    std::shared_ptr<VFS> vfs;
    bool deleteOnDestroy;
    std::shared_ptr<VFS> parent;
    std::string tmp_str;

  public:
    std::string TempDirectoryName();
    TempFS(bool deleteOnDestroy = true);
    TempFS(std::shared_ptr<VFS> vfs, bool deleteOnDestroy = true);

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
    void DeleteDirectoryRecurse(VFSPath path);
    VFSPath ReadLink(VFSPath path);
    std::string VFSPathToSystem(VFSPath path);
    VFSPath SystemToVFSPath(std::string path);

    void SetDate(VFSPath path, Date::DateTime lastWrite,
                 Date::DateTime lastAccess);
    bool StatVFS(VFSPath path, StatVFSData &vfsData);
    bool Stat(VFSPath path, StatData &data);

    void Chmod(VFSPath path, uint32_t mode);

    void Chown(VFSPath path, uint32_t uid, uint32_t gid);
    FIFOCreationResult CreateFIFO(VFSPath path, uint32_t mode);
    void Close();

    void Lock(VFSPath path);
    void Unlock(VFSPath path);
    ~TempFS();

  protected:
    std::shared_ptr<FSWatcher> CreateWatcher(std::shared_ptr<VFS> vfs,
                                             VFSPath path);
};
} // namespace Tesses::Framework::Filesystem
