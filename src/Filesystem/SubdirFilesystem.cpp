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
    bool SubdirFilesystem::RegularFileExists(VFSPath path)
    {
        return this->parent->RegularFileExists(ToParent(path));
    }
    bool SubdirFilesystem::SymlinkExists(VFSPath path)
    {
        return this->parent->SymlinkExists(ToParent(path));
    }
    bool SubdirFilesystem::CharacterDeviceExists(VFSPath path)
    {
        return this->parent->CharacterDeviceExists(ToParent(path));
    }
    bool SubdirFilesystem::BlockDeviceExists(VFSPath path)
    {
        return this->parent->BlockDeviceExists(ToParent(path));
    }
    bool SubdirFilesystem::SocketFileExists(VFSPath path)
    {
        return this->parent->SocketFileExists(ToParent(path));
    }
    bool SubdirFilesystem::FIFOFileExists(VFSPath path)
    {
        return this->parent->FIFOFileExists(ToParent(path));
    }
    bool SubdirFilesystem::DirectoryExists(VFSPath path)
    {
        return this->parent->DirectoryExists(ToParent(path));
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

        return VFSPathEnumerator([enumerator,path,this](VFSPath& path0)->bool{
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
    void SubdirFilesystem::GetDate(VFSPath path, Date::DateTime& lastWrite, Date::DateTime& lastAccess)
    {
        this->parent->GetDate(ToParent(path),lastWrite,lastAccess);
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
    bool SubdirFilesystem::SpecialFileExists(VFSPath path)
    {
        return this->parent->SpecialFileExists(ToParent(path));
    }
    bool SubdirFilesystem::FileExists(VFSPath path)
    {
        return this->parent->FileExists(ToParent(path));
    }
            
    SubdirFilesystem::~SubdirFilesystem()
    {
     
    }
    bool SubdirFilesystem::StatVFS(VFSPath path, StatVFSData& vfsData)
    {
        return this->parent->StatVFS(ToParent(path), vfsData);
    }

    void SubdirFilesystem::Chmod(VFSPath path, uint32_t mode)
    {
        return this->parent->Chmod(ToParent(path), mode);
    }

}
