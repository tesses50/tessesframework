#include "TessesFramework/Filesystem/MountableFilesystem.hpp"
#include "TessesFramework/Filesystem/NullFilesystem.hpp"
#include <iostream>
namespace Tesses::Framework::Filesystem
{
    MountableFilesystem::MountableFilesystem() : MountableFilesystem(new NullFilesystem(),true)
    {

    }
    MountableFilesystem::MountableFilesystem(VFS* root, bool owns)
    {
        this->root = root;
        this->owns = owns;
    }
    MountableDirectory::~MountableDirectory()
    {
        if(this->owns && this->vfs) delete this->vfs;
        for(auto dir : this->dirs) delete dir;
    }

    MountableFilesystem::~MountableFilesystem()
    {
        if(this->owns && this->root) delete root;
        for(auto item : this->directories) delete item;
    }


    void MountableFilesystem::GetFS(VFSPath srcPath, VFSPath& destRoot, VFSPath& destPath, VFS*& vfs) 
    {
        if(srcPath.path.empty()) return;
        for(auto item : this->directories)
        {
            if(srcPath.path.front() == item->name)
            {
                if(item->vfs != nullptr)
                {
                    vfs = item->vfs;
                    VFSPath srcPath1(std::vector(srcPath.path.begin()+1,srcPath.path.end()));
                    srcPath1.relative=false;
                    destPath = srcPath1;
                    destRoot = VFSPath(VFSPath(),item->name);
                    
                    
                }
                VFSPath srcPath2(std::vector(srcPath.path.begin()+1,srcPath.path.end()));
                srcPath2.relative=false;
                item->GetFS(srcPath2,VFSPath(VFSPath(),item->name), destRoot,destPath,vfs);
                return;
            }
        }
    }

  
    
    void MountableDirectory::GetFS(VFSPath srcPath, VFSPath curDir, VFSPath& destRoot, VFSPath& destPath, VFS*& vfs) 
    {
        if(srcPath.path.empty()) return;
        for(auto item : this->dirs)
        {
            if(!srcPath.path.empty() && srcPath.path.front() == item->name)
            {
                if(item->vfs != nullptr)
                {
                    vfs = item->vfs;
                    
                    VFSPath srcPath1(std::vector(srcPath.path.begin()+1,srcPath.path.end()));

                    srcPath1.relative=false;
                    destPath = srcPath1;
                    destRoot = curDir;
                    
                }
                VFSPath srcPath2(std::vector(srcPath.path.begin()+1,srcPath.path.end()));
                srcPath2.relative=false;
                item->GetFS(srcPath2,VFSPath(curDir,item->name), destRoot,destPath,vfs);
                return;
            }
        }
    }

    VFSPath MountableFilesystem::ReadLink(VFSPath path)
    {
        path = path.CollapseRelativeParents();
        VFSPath destRoot;
        VFSPath destPath = path;
        VFS* vfs = root;
        
        GetFS(path, destRoot, destPath, vfs);
        if(vfs != nullptr)
            return VFSPath(destRoot,vfs->ReadLink(destPath));
        return VFSPath();
    }

    Tesses::Framework::Streams::Stream* MountableFilesystem::OpenFile(VFSPath path, std::string mode)
    {
        path = path.CollapseRelativeParents();
        VFSPath destRoot;
        VFSPath destPath = path;
        VFS* vfs = root;
        
        GetFS(path, destRoot, destPath, vfs);

        if(vfs != nullptr)
            return vfs->OpenFile(destPath,mode);
        return nullptr;
    }
    void MountableFilesystem::CreateDirectory(VFSPath path)
    {
        path = path.CollapseRelativeParents();
       
        VFSPath destRoot;
        VFSPath destPath = path;
        VFS* vfs = root;
        
        GetFS(path, destRoot, destPath, vfs);

        if(destPath.path.empty()) return;

        if(vfs != nullptr)
            vfs->CreateDirectory(destPath);
        
    }
    

    void MountableFilesystem::DeleteDirectory(VFSPath path)
    {
         path = path.CollapseRelativeParents();
       
        VFSPath destRoot;
        VFSPath destPath = path;
        VFS* vfs = root;
        
        GetFS(path, destRoot, destPath, vfs);

        if(destPath.path.empty()) return;

        if(vfs != nullptr)
            vfs->DeleteDirectory(destPath);
        
    }

    bool MountableFilesystem::SpecialFileExists(VFSPath path)
    {
        path = path.CollapseRelativeParents();
       
        VFSPath destRoot;
        VFSPath destPath = path;
        VFS* vfs = root;
        
        GetFS(path, destRoot, destPath, vfs);

        if(vfs != nullptr)
            return vfs->SpecialFileExists(destPath);
        return false;
    }

    bool MountableFilesystem::FileExists(VFSPath path)
    {
        path = path.CollapseRelativeParents();
       
        VFSPath destRoot;
        VFSPath destPath = path;
        VFS* vfs = root;
        
        GetFS(path, destRoot, destPath, vfs);

        if(vfs != nullptr)
            return vfs->FileExists(destPath);
        return false;
    }
    bool MountableFilesystem::RegularFileExists(VFSPath path)
    {
        path = path.CollapseRelativeParents();
       
        VFSPath destRoot;
        VFSPath destPath = path;
        VFS* vfs = root;
        
        GetFS(path, destRoot, destPath, vfs);

        if(vfs != nullptr)
            return vfs->RegularFileExists(destPath);
        return false;
    }

    bool MountableFilesystem::SymlinkExists(VFSPath path)
    {
        path = path.CollapseRelativeParents();
       
        VFSPath destRoot;
        VFSPath destPath = path;
        VFS* vfs = root;
        
        GetFS(path, destRoot, destPath, vfs);

        if(vfs != nullptr)
            return vfs->SymlinkExists(destPath);
        return false;
    }
    bool MountableFilesystem::CharacterDeviceExists(VFSPath path)
    {
        path = path.CollapseRelativeParents();
       
        VFSPath destRoot;
        VFSPath destPath = path;
        VFS* vfs = root;
        
        GetFS(path, destRoot, destPath, vfs);

        if(vfs != nullptr)
            return vfs->CharacterDeviceExists(destPath);
        return false;
    }
    bool MountableFilesystem::BlockDeviceExists(VFSPath path)
    {
        path = path.CollapseRelativeParents();
       
        VFSPath destRoot;
        VFSPath destPath = path;
        VFS* vfs = root;
        
        GetFS(path, destRoot, destPath, vfs);

        if(vfs != nullptr)
            return vfs->BlockDeviceExists(destPath);
        return false;
    }

    bool MountableFilesystem::SocketFileExists(VFSPath path)
    {
        path = path.CollapseRelativeParents();
       
        VFSPath destRoot;
        VFSPath destPath = path;
        VFS* vfs = root;
        
        GetFS(path, destRoot, destPath, vfs);

        if(vfs != nullptr)
            return vfs->SocketFileExists(destPath);
        return false;
    }


    bool MountableFilesystem::FIFOFileExists(VFSPath path)
    {
        path = path.CollapseRelativeParents();
       
        VFSPath destRoot;
        VFSPath destPath = path;
        VFS* vfs = root;
        
        GetFS(path, destRoot, destPath, vfs);

        if(vfs != nullptr)
            return vfs->FIFOFileExists(destPath);
        return false;
    }



    bool MountableFilesystem::DirectoryExists(VFSPath path)
    {
        path = path.CollapseRelativeParents();
       
        VFSPath destRoot;
        VFSPath destPath = path;
        VFS* vfs = root;
        
        GetFS(path, destRoot, destPath, vfs);
        if(destPath.path.empty()) return true;

        if(vfs != nullptr)
            return vfs->DirectoryExists(destPath);
        return false;
    }
    void MountableFilesystem::DeleteFile(VFSPath path)
    {
        path = path.CollapseRelativeParents();
       
        VFSPath destRoot;
        VFSPath destPath = path;
        VFS* vfs = root;
        
        GetFS(path, destRoot, destPath, vfs);

        if(vfs != nullptr)
            vfs->DeleteFile(destPath);
       
    }
      void MountableFilesystem::GetDate(VFSPath path, time_t& lastWrite, time_t& lastAccess)
    {

        path = path.CollapseRelativeParents();
       
        VFSPath destRoot;
        VFSPath destPath = path;
        VFS* vfs = root;
        
        GetFS(path, destRoot, destPath, vfs);

        if(vfs != nullptr)
            vfs->GetDate(destPath,lastWrite,lastAccess);
    }
    void MountableFilesystem::SetDate(VFSPath path, time_t lastWrite, time_t lastAccess)
    {

        path = path.CollapseRelativeParents();
       
        VFSPath destRoot;
        VFSPath destPath = path;
        VFS* vfs = root;
        
        GetFS(path, destRoot, destPath, vfs);

        if(vfs != nullptr)
            vfs->SetDate(destPath,lastWrite,lastAccess);
    }
    void MountableFilesystem::CreateSymlink(VFSPath existingFile, VFSPath symlinkFile)
    {
        existingFile = existingFile.CollapseRelativeParents();
        symlinkFile = existingFile.CollapseRelativeParents();
       
        VFSPath existingDestRoot;
        VFSPath existingDestPath = existingFile;
        VFS* existingVFS = root;
        VFSPath symlinkDestRoot;
        VFSPath symlinkDestPath = symlinkFile;
        VFS* symlinkVFS = root;
        
        GetFS(existingFile, existingDestRoot, existingDestPath, existingVFS);
        GetFS(symlinkFile, symlinkDestRoot, symlinkDestPath, symlinkVFS);
        
        if(existingVFS != nullptr && existingVFS == symlinkVFS)
            existingVFS->CreateSymlink(existingDestPath, symlinkDestPath);
    }

    void MountableFilesystem::MoveDirectory(VFSPath src, VFSPath dest)
    {
        src = src.CollapseRelativeParents();
        dest = dest.CollapseRelativeParents();
       
        VFSPath srcDestRoot;
        VFSPath srcDestPath = src;
        VFS* srcVFS = root;
        VFSPath destDestRoot;
        VFSPath destDestPath = dest;
        VFS* destVFS = root;
        
        GetFS(src, srcDestRoot, srcDestPath, srcVFS);
        GetFS(dest, destDestRoot, destDestPath, destVFS);
        
        if(srcVFS != nullptr && srcVFS == destVFS)
            srcVFS->MoveDirectory(srcDestPath, destDestPath);
    }
    void MountableFilesystem::MoveFile(VFSPath src, VFSPath dest)
    {
        src = src.CollapseRelativeParents();
        dest = dest.CollapseRelativeParents();
       
        VFSPath srcDestRoot;
        VFSPath srcDestPath = src;
        VFS* srcVFS = root;
        VFSPath destDestRoot;
        VFSPath destDestPath = dest;
        VFS* destVFS = root;
        
        GetFS(src, srcDestRoot, srcDestPath, srcVFS);
        GetFS(dest, destDestRoot, destDestPath, destVFS);
        
        if(srcVFS != nullptr && srcVFS == destVFS)
            srcVFS->MoveFile(srcDestPath, destDestPath);
    }
    void MountableFilesystem::CreateHardlink(VFSPath existingFile, VFSPath newName)
    {
        existingFile = existingFile.CollapseRelativeParents();
        newName = existingFile.CollapseRelativeParents();
       
        VFSPath existingDestRoot;
        VFSPath existingDestPath = existingFile;
        VFS* existingVFS = root;
        VFSPath newNameRoot;
        VFSPath newNamePath = newName;
        VFS* newNameVFS = root;
        
        GetFS(existingFile, existingDestRoot, existingDestPath, existingVFS);
        GetFS(newName, newNameRoot, newNamePath, newNameVFS);
        
        if(existingVFS != nullptr && existingVFS == newNameVFS)
            existingVFS->CreateHardlink(existingDestPath, newNamePath);
    }
    class MountableEnumerationState {
        public:
            VFSPathEnumerator* enumerator;
            std::vector<MountableDirectory*> dirs;
            size_t index;
    };
    VFSPathEnumerator MountableFilesystem::EnumeratePaths(VFSPath path)
    {
        
        path = path.CollapseRelativeParents();
        bool mydirs = path.path.empty();
        std::vector<MountableDirectory*>* dirs = &this->directories;
        
        if(!path.path.empty())
        for(auto p : path.path)
        {
            mydirs=true;
            bool hasSet=false;
        
            for(auto itm : *dirs)
            {
                if(itm->name == p)
                {
                    hasSet=true;
                    dirs = &itm->dirs;
                    break;
                }
            }

            if(!hasSet)
            {
                mydirs=false;
                break;
            }
        }
       
        VFSPath destRoot;
        VFSPath destPath = path;
        VFS* vfs = root;
        
        GetFS(path, destRoot, destPath, vfs);
        

        MountableEnumerationState* state = new MountableEnumerationState();
        state->dirs = *dirs;
        state->index = 0;
        if(vfs->DirectoryExists(destPath) || !mydirs)
        state->enumerator = vfs->EnumeratePaths(destPath).MakePointer();
        else
        state->enumerator = nullptr;

        return VFSPathEnumerator([state,path](VFSPath& path0)->bool{
            
            while(state->enumerator != nullptr && state->enumerator->MoveNext())
            {
                auto fname = state->enumerator->Current.GetFileName();
               
                bool mustContinue=false;
                for(auto item : state->dirs)
                {
                    if(item->name == fname)
                    {
                        mustContinue=true;
                        break;
                    }
                }
                if(mustContinue) continue;
                path0 = path / fname;
                return true;
            }
            if(state->enumerator != nullptr)
            {
                delete state->enumerator;
                state->enumerator = nullptr;
            }
            if(state->index < state->dirs.size())
            {
                path0 =  path / state->dirs[state->index++]->name;
                return true;
            }
            return false;
        },[state]()->void{
            if(state->enumerator) delete state->enumerator;
            delete state;
        });
    }
    
    void MountableFilesystem::Mount(VFSPath path, VFS* fs, bool owns)
    {
         path = path.CollapseRelativeParents();
       
        if(path.path.empty())
        {
            if(owns) delete fs;
            return;
        }

        auto* fsLs = &this->directories;
        bool needToCreate=true;
        for(auto index = path.path.begin(); index < path.path.end()-1; index++)
        {
            needToCreate=true;
            for(auto item : *fsLs)
            {
                if(item->name == *index)
                {
                    needToCreate=false;
                    fsLs = &(item->dirs);
                    break;
                }
            }
            if(needToCreate)
            {
                MountableDirectory* dir = new MountableDirectory();
                dir->name = *index;
                dir->owns=false;
                dir->vfs=NULL;
                
                fsLs->push_back(dir);
                fsLs = &(dir->dirs);
            }
        }
        
        needToCreate=true;
        std::string lastDir = path.GetFileName();

        for(auto item : *fsLs)
        {
            if(item->name == lastDir)
            {
                needToCreate=false;
                if(item->owns && item->vfs) delete item->vfs;
                item->vfs = fs;
                break;
            }
        }

        if(needToCreate)
        {
             MountableDirectory* dir = new MountableDirectory();
                dir->name = lastDir;
                dir->owns=owns;
                dir->vfs=fs;
                fsLs->push_back(dir);
        }

    }

    static bool myumount(MountableDirectory* dir,VFSPath path)
    {
        if(path.path.empty()) 
        {
            if(dir->owns && dir->vfs) delete dir->vfs;
            dir->vfs = nullptr;
        }
        
        if(dir->dirs.empty())
        {
            delete dir;
            return true;
        }

        for(auto index = dir->dirs.begin(); index < dir->dirs.end(); index++)
        {
            auto item = *index;
            if(!path.path.empty() && path.path.front() == item->name)
            {
                VFSPath srcPath2(std::vector(path.path.begin()+1,path.path.end()));

                
                
                if(myumount(item,srcPath2))
                {
                    dir->dirs.erase(index);
                }

                if(dir->dirs.empty())
                {
                    delete dir;
                    return true;
                }
                return false;
            }
        }
        return false;
    }

    void MountableFilesystem::Unmount(VFSPath path)
    {
        path = path.CollapseRelativeParents();
       
        for(auto index = this->directories.begin(); index < this->directories.end(); index++)
        {
            auto item = *index;
            if(!path.path.empty() && path.path.front() == item->name)
            {
                 VFSPath srcPath2(std::vector(path.path.begin()+1,path.path.end()));
                
                if(myumount(item,srcPath2))
                {
                    this->directories.erase(index);
                }
                return;
            }
        }
    }

    std::string MountableFilesystem::VFSPathToSystem(VFSPath path)
    {
        return path.ToString();
    }
    VFSPath MountableFilesystem::SystemToVFSPath(std::string path)
    {
        return VFSPath(path);
    }
}