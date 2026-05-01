#include "TessesFramework/Filesystem/RelativeFilesystem.hpp"

namespace Tesses::Framework::Filesystem
{
    VFSPath RelativeFilesystem::ToParent(VFSPath path)
    {
        if(path.relative)
        {
            return path.MakeAbsolute(GetWorking());
        }
        else
        {
            return path;
        }
    }

    RelativeFilesystem::RelativeFilesystem(std::shared_ptr<VFS> vfs, VFSPath working) : vfs(vfs), working(working)
    {

    }
    VFSPath RelativeFilesystem::GetWorking()
    {
        mtx.Lock();
        auto p = this->working;
        mtx.Unlock();
        return p;
    }
    void RelativeFilesystem::SetWorking(VFSPath path)
    {
        mtx.Lock();
        this->working=path;
        mtx.Unlock();
    }
    std::shared_ptr<VFS> RelativeFilesystem::GetVFS()
    {
        return vfs;
    }

    std::shared_ptr<Tesses::Framework::Streams::Stream> RelativeFilesystem::OpenFile(VFSPath path, std::string mode)
    {
        return this->vfs->OpenFile(ToParent(path),mode);
    }
    void RelativeFilesystem::CreateDirectory(VFSPath path)
    {
        this->vfs->CreateDirectory(ToParent(path));
    }
    void RelativeFilesystem::DeleteDirectory(VFSPath path)
    {
        this->vfs->DeleteDirectory(ToParent(path));
    }
    void RelativeFilesystem::DeleteFile(VFSPath path)
    {
        this->vfs->DeleteFile(ToParent(path));
    }
    void RelativeFilesystem::CreateSymlink(VFSPath existingFile, VFSPath symlinkFile)
    {
        this->vfs->CreateSymlink(existingFile, ToParent(symlinkFile));
    }
    VFSPathEnumerator RelativeFilesystem::EnumeratePaths(VFSPath path)
    {
        VFSPathEnumerator* enumerator = this->vfs->EnumeratePaths(ToParent(path)).MakePointer();

        return VFSPathEnumerator([enumerator,path,this](VFSPath& path0)->bool{
            if(enumerator->MoveNext())
            {
                path0 = path / enumerator->Current.GetFileName();
                return true;
            }
            return false;
        },[enumerator]()->void{
            delete enumerator;
        });
    }
    void RelativeFilesystem::CreateHardlink(VFSPath existingFile, VFSPath newName)
    {
        if(existingFile.relative || newName.relative)
        {
            auto working = GetWorking();
            if(existingFile.relative)
                existingFile = existingFile.MakeAbsolute(working);
            
            if(newName.relative)
                newName = newName.MakeAbsolute(working);
        }
        vfs->CreateHardlink(existingFile, newName);
    }
    void RelativeFilesystem::MoveFile(VFSPath src, VFSPath dest)
    {
        if(src.relative || dest.relative)
        {
            auto working = GetWorking();
            if(src.relative)
                src = src.MakeAbsolute(working);
            
            if(dest.relative)
                dest = dest.MakeAbsolute(working);
        }
        vfs->MoveFile(src, dest);
    }
    void RelativeFilesystem::MoveDirectory(VFSPath src, VFSPath dest)
    {
        if(src.relative || dest.relative)
        {
            auto working = GetWorking();
            if(src.relative)
                src = src.MakeAbsolute(working);
            
            if(dest.relative)
                dest = dest.MakeAbsolute(working);
        }
        vfs->MoveDirectory(src, dest);
    }
    void RelativeFilesystem::DeleteDirectoryRecurse(VFSPath path)
    {
        vfs->DeleteDirectoryRecurse(ToParent(path));
    }
    VFSPath RelativeFilesystem::ReadLink(VFSPath path)
    {
        return vfs->ReadLink(ToParent(path));
    }
    std::string RelativeFilesystem::VFSPathToSystem(VFSPath path)
    {
        return vfs->VFSPathToSystem(path);
    }
    VFSPath RelativeFilesystem::SystemToVFSPath(std::string path)
    {
        return vfs->SystemToVFSPath(path);
    }
    void RelativeFilesystem::SetDate(VFSPath path, Date::DateTime lastWrite, Date::DateTime lastAccess)
    {
        vfs->SetDate(ToParent(path),lastWrite,lastAccess);
    }
    bool RelativeFilesystem::StatVFS(VFSPath path, StatVFSData& vfsData)
    {
        return vfs->StatVFS(ToParent(path), vfsData);
    }
    bool RelativeFilesystem::Stat(VFSPath path, StatData& data)
    {
        return vfs->Stat(ToParent(path),data);
    }

    void RelativeFilesystem::Chown(VFSPath path, uint32_t uid, uint32_t gid)
    {
        vfs->Chown(ToParent(path),uid,gid);
    }
    void RelativeFilesystem::Chmod(VFSPath path, uint32_t mode)
    {
        vfs->Chmod(ToParent(path), mode);
    }
    FIFOCreationResult RelativeFilesystem::CreateFIFO(VFSPath path, uint32_t mode)
    {
        return vfs->CreateFIFO(ToParent(path),mode);
    }
    void RelativeFilesystem::Lock(VFSPath path)
    {
        vfs->Lock(ToParent(path));
    }
    void RelativeFilesystem::Unlock(VFSPath path)
    {
        vfs->Unlock(ToParent(path));
    }
    

    RelativeFilesystem::Watcher::Watcher(std::shared_ptr<RelativeFilesystem> vfs, VFSPath path) : FSWatcher(vfs, path)
    {
        this->watcher = FSWatcher::Create(vfs->vfs, vfs->ToParent(path));
        this->watcher->event = [vfs,this,path](FSWatcherEvent & evt)-> void{
            if(path.relative)
            {
                auto working = vfs->GetWorking();
                FSWatcherEvent e2=evt;
                if(evt.IsEvent(FSWatcherEventType::Moved))
                {
                    e2.dest = e2.dest.MakeRelative(working);
                }
                e2.src = e2.src.MakeRelative(working);

                if(this->event) this->event(e2);
            }
            else {
                if(this->event)
                    this->event(evt);
            }
        };
    }

    void RelativeFilesystem::Watcher::SetEnabledImpl(bool enabled)
    {
        this->enabled = enabled;
        this->watcher->events = this->events;
        this->watcher->SetEnabled(enabled);
    }
    RelativeFilesystem::Watcher::~Watcher()
    {
        this->watcher->SetEnabled(false);
    }
    std::shared_ptr<FSWatcher> RelativeFilesystem::CreateWatcher(std::shared_ptr<VFS> vfs, VFSPath path)
    {
        auto sdfs = std::dynamic_pointer_cast<RelativeFilesystem>(vfs);
        if(sdfs)
        {
            return std::make_shared<Watcher>(sdfs,path);
        }
        return VFS::CreateWatcher(vfs,path);
    }
}