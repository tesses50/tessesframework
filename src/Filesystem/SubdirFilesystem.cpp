#include "TessesFramework/Filesystem/SubdirFilesystem.hpp"
#include "TessesFramework/Filesystem/LocalFS.hpp"
#include <iostream>
namespace Tesses::Framework::Filesystem
{
    VFSPath SubdirFilesystem::ReadLink(VFSPath path)
    {
        return FromParent(this->parent->ReadLink(ToParent(path)));
    }
    VFSPath SubdirFilesystem::FromParent(VFSPath path)
    {
        // /a/b/c
        // /a/b/c
        VFSPath newPath;
        newPath.relative=false;

        if(path.path.size() >= this->path.path.size())
        {
            newPath.path.reserve(path.path.size()-this->path.path.size());
            for(size_t i = this->path.path.size(); i < path.path.size();i++)
            {
                newPath.path.push_back(path.path[i]);
            }
        }
        return newPath;
    }

    VFSPath SubdirFilesystem::ToParent(VFSPath path)
    {
        return this->path / path.CollapseRelativeParents();
    }
    SubdirFilesystem::SubdirFilesystem(std::shared_ptr<VFS> parent, VFSPath path)
    {
        this->parent = parent;
        if(path.relative && std::dynamic_pointer_cast<LocalFilesystem>(parent) != nullptr)
        {
            Tesses::Framework::Filesystem::LocalFilesystem lfs;
            auto curDir = std::filesystem::current_path();
            auto myPath = lfs.SystemToVFSPath(curDir.string()) / path;
            this->path = myPath.CollapseRelativeParents();
        }
        else
        this->path = path;

    }
    std::shared_ptr<Tesses::Framework::Streams::Stream> SubdirFilesystem::OpenFile(VFSPath path, std::string mode)
    {
        return this->parent->OpenFile(ToParent(path),mode);
    }
    void SubdirFilesystem::CreateDirectory(VFSPath path)
    {
        this->parent->CreateDirectory(ToParent(path));
    }
    void SubdirFilesystem::DeleteDirectory(VFSPath path)
    {
        this->parent->DeleteDirectory(ToParent(path));
    }
    void SubdirFilesystem::DeleteFile(VFSPath path)
    {
        this->parent->DeleteFile(ToParent(path));
    }
    void SubdirFilesystem::Lock(VFSPath path)
    {
        this->parent->Lock(ToParent(path));
    }

    void SubdirFilesystem::Unlock(VFSPath path)
    {
        this->parent->Unlock(ToParent(path));
    }
    void SubdirFilesystem::CreateSymlink(VFSPath existingFile, VFSPath symlinkFile)
    {
        this->parent->CreateSymlink(ToParent(existingFile),ToParent(symlinkFile));
    }

    VFSPathEnumerator SubdirFilesystem::EnumeratePaths(VFSPath path)
    {
        VFSPathEnumerator* enumerator = this->parent->EnumeratePaths(ToParent(path)).MakePointer();

        return VFSPathEnumerator([enumerator,this](VFSPath& path0)->bool{
            if(enumerator->MoveNext())
            {
                path0 = FromParent(enumerator->Current);
                return true;
            }
            return false;
        },[enumerator]()->void{
            delete enumerator;
        });
    }

    void SubdirFilesystem::SetDate(VFSPath path, Date::DateTime lastWrite, Date::DateTime lastAccess)
    {
        this->parent->SetDate(ToParent(path),lastWrite,lastAccess);
    }

    void SubdirFilesystem::CreateHardlink(VFSPath existingFile, VFSPath newName)
    {
        this->parent->CreateHardlink(ToParent(existingFile),ToParent(newName));
    }
    void SubdirFilesystem::MoveFile(VFSPath src, VFSPath dest)
    {
        this->parent->MoveFile(ToParent(src),ToParent(dest));
    }
    void SubdirFilesystem::MoveDirectory(VFSPath src, VFSPath dest)
    {
        this->parent->MoveDirectory(ToParent(src),ToParent(dest));
    }
    std::string SubdirFilesystem::VFSPathToSystem(VFSPath path)
    {
        return this->parent->VFSPathToSystem(ToParent(path));
    }
    VFSPath SubdirFilesystem::SystemToVFSPath(std::string path)
    {
        return FromParent(this->parent->SystemToVFSPath(path));
    }
    void SubdirFilesystem::DeleteDirectoryRecurse(VFSPath path)
    {
        this->parent->DeleteDirectoryRecurse(ToParent(path));
    }


    SubdirFilesystem::~SubdirFilesystem()
    {

    }
    bool SubdirFilesystem::StatVFS(VFSPath path, StatVFSData& vfsData)
    {
        return this->parent->StatVFS(ToParent(path), vfsData);
    }
    bool SubdirFilesystem::Stat(VFSPath path, StatData& data)
    {
        return this->parent->Stat(ToParent(path), data);
    }
    void SubdirFilesystem::Chmod(VFSPath path, uint32_t mode)
    {
        return this->parent->Chmod(ToParent(path), mode);
    }
    void SubdirFilesystem::Chown(VFSPath path, uint32_t uid, uint32_t gid)
    {
        return this->parent->Chown(ToParent(path), uid, gid);
    }


    FIFOCreationResult SubdirFilesystem::CreateFIFO(VFSPath path, uint32_t mod)
    {
        return this->parent->CreateFIFO(path, mod);
    }
    
    SubdirFilesystem::Watcher::Watcher(std::shared_ptr<SubdirFilesystem> vfs, VFSPath path) : FSWatcher(vfs, path)
    {
        this->watcher = FSWatcher::Create(vfs->parent, vfs->ToParent(path));
        this->watcher->event = [vfs,this](FSWatcherEvent & evt)-> void{
            FSWatcherEvent e2=evt;
            if(evt.IsEvent(FSWatcherEventType::Moved))
            {
                e2.dest = vfs->FromParent(e2.dest);
            }
            e2.src = vfs->FromParent(e2.src);

            if(this->event) this->event(e2);
        };
    }

    void SubdirFilesystem::Watcher::SetEnabledImpl(bool enabled)
    {
        this->enabled = enabled;
        this->watcher->events = this->events;
        this->watcher->SetEnabled(enabled);
    }
    SubdirFilesystem::Watcher::~Watcher()
    {
        this->watcher->SetEnabled(false);
    }
    std::shared_ptr<FSWatcher> SubdirFilesystem::CreateWatcher(std::shared_ptr<VFS> vfs, VFSPath path)
    {
        auto sdfs = std::dynamic_pointer_cast<SubdirFilesystem>(vfs);
        if(sdfs)
        {
            return std::make_shared<Watcher>(sdfs,path);
        }
        return VFS::CreateWatcher(vfs,path);
    }

}
