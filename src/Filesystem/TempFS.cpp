#include "TessesFramework/Threading/Mutex.hpp"
#include "TessesFramework/Filesystem/TempFS.hpp"
#include "TessesFramework/Filesystem/LocalFS.hpp"
#include "TessesFramework/Filesystem/SubdirFilesystem.hpp"
#include "TessesFramework/Platform/Environment.hpp"
namespace Tesses::Framework::Filesystem {
    Tesses::Framework::Threading::Mutex umtx;
    int64_t uidx=0;
    void UniqueString(std::string& text)
    {   
        umtx.Lock();
        text += std::to_string((int64_t)time(NULL));
        text += "_";
        text += std::to_string(uidx);
        
        uidx++;

        umtx.Unlock();
    }

    TempFS::TempFS(bool deleteOnDestroy) : TempFS(std::make_shared<SubdirFilesystem>(LocalFS, Platform::Environment::SpecialFolders::GetTemp()), deleteOnDestroy)
    {

    }
    TempFS::TempFS(std::shared_ptr<VFS> vfs,bool deleteOnDestroy)
    {
        this->parent = vfs;
        this->deleteOnDestroy=deleteOnDestroy;
        this->tmp_str = "tf_tmp_";
        UniqueString(this->tmp_str);
        VFSPath p;
        p.relative = false;
        p.path.push_back(this->tmp_str);
        this->parent->CreateDirectory(p);
        this->vfs = std::make_shared<SubdirFilesystem>(this->parent,p);
    }

    std::string TempFS::TempDirectoryName()
    {
        return this->tmp_str;
    }
    std::shared_ptr<Tesses::Framework::Streams::Stream> TempFS::OpenFile(VFSPath path, std::string mode)
    {
        if(this->vfs == nullptr) return nullptr;
        return this->vfs->OpenFile(path,mode);
    }
    void TempFS::CreateDirectory(VFSPath path)
    {
        if(this->vfs == nullptr) return;
        this->vfs->CreateDirectory(path);
    }
    void TempFS::DeleteDirectory(VFSPath path)
    {
        if(this->vfs == nullptr) return;
        this->vfs->DeleteDirectory(path);
    }
    bool TempFS::SpecialFileExists(VFSPath path)
    {

        if(this->vfs == nullptr) return false;
        return this->vfs->SpecialFileExists(path);
    }
    bool TempFS::FileExists(VFSPath path)
    {

        if(this->vfs == nullptr) return false;
        return this->vfs->FileExists(path);
    }
    bool TempFS::RegularFileExists(VFSPath path)
    {

        if(this->vfs == nullptr) return false;
        return this->vfs->RegularFileExists(path);
    }
    bool TempFS::SymlinkExists(VFSPath path)
    {

        if(this->vfs == nullptr) return false;
        return this->vfs->SymlinkExists(path);
    }
    bool TempFS::CharacterDeviceExists(VFSPath path)
    {

        if(this->vfs == nullptr) return false;
        return this->vfs->CharacterDeviceExists(path);
    }
    bool TempFS::BlockDeviceExists(VFSPath path)
    {
        if(this->vfs == nullptr) return false;
        return this->vfs->BlockDeviceExists(path);
    }
    bool TempFS::SocketFileExists(VFSPath path)
    {
        if(this->vfs == nullptr) return false;
        return this->vfs->SocketFileExists(path);
    }
    bool TempFS::FIFOFileExists(VFSPath path)
    {
        if(this->vfs == nullptr) return false;
        return this->vfs->FIFOFileExists(path);
    }
    bool TempFS::DirectoryExists(VFSPath path)
    {
        if(this->vfs == nullptr) return false;
        return this->vfs->DirectoryExists(path);
    }
    void TempFS::DeleteFile(VFSPath path)
    {
        if(this->vfs == nullptr) return;
        this->vfs->DeleteFile(path);
    }
    void TempFS::Lock(VFSPath path)
    {
        if(this->vfs == nullptr) return;
        this->vfs->Lock(path);
    }
    void TempFS::Unlock(VFSPath path)
    {
        if(this->vfs == nullptr) return;
        this->vfs->Unlock(path);
    }
    void TempFS::CreateSymlink(VFSPath existingFile, VFSPath symlinkFile)
    {
        if(this->vfs == nullptr) return;
        this->vfs->CreateSymlink(existingFile, symlinkFile);
    }
    VFSPathEnumerator TempFS::EnumeratePaths(VFSPath path)
    {
        
        if(this->vfs == nullptr) return VFSPathEnumerator();

        return this->vfs->EnumeratePaths(path);
    }
    void TempFS::CreateHardlink(VFSPath existingFile, VFSPath newName)
    {

        if(this->vfs == nullptr) return;
        this->vfs->CreateHardlink(existingFile,newName);
    }
    void TempFS::MoveFile(VFSPath src, VFSPath dest)
    {

        if(this->vfs == nullptr) return;
        this->vfs->MoveFile(src,dest);
    }
    void TempFS::MoveDirectory(VFSPath src, VFSPath dest)
    {
        if(this->vfs == nullptr) return;
        this->vfs->MoveDirectory(src,dest);
    }
    void TempFS::DeleteDirectoryRecurse(VFSPath path)
    {

        if(this->vfs == nullptr) return;
        this->vfs->DeleteDirectoryRecurse(path);
    }
    VFSPath TempFS::ReadLink(VFSPath path)
    {

        if(this->vfs == nullptr) return VFSPath();
        return this->vfs->ReadLink(path);
    }
    std::string TempFS::VFSPathToSystem(VFSPath path)
    {

        if(this->vfs == nullptr) return "";
        return this->vfs->VFSPathToSystem(path);
    }
    VFSPath TempFS::SystemToVFSPath(std::string path)
    {

        if(this->vfs == nullptr) return VFSPath();
        return this->vfs->SystemToVFSPath(path);
    }
            
    void TempFS::GetDate(VFSPath path, Date::DateTime& lastWrite, Date::DateTime& lastAccess)
    {

        if(this->vfs == nullptr) return;
        this->vfs->GetDate(path,lastWrite,lastAccess);
    }
    void TempFS::SetDate(VFSPath path, Date::DateTime lastWrite, Date::DateTime lastAccess)
    {

        if(this->vfs == nullptr) return;
        this->vfs->SetDate(path,lastWrite,lastAccess);
    }
    bool TempFS::StatVFS(VFSPath path, StatVFSData& vfsData)
    {
        
        if(this->vfs == nullptr) return false;
        return this->vfs->StatVFS(path, vfsData);
    }

    void TempFS::Chmod(VFSPath path, uint32_t mode)
    {

        if(this->vfs == nullptr) return;
        this->vfs->Chmod(path,mode);
    }
    void TempFS::Close()
    {

        VFSPath p;
        p.relative = false;
        p.path.push_back(this->tmp_str);
        this->vfs = nullptr;
        if(this->deleteOnDestroy && this->parent->DirectoryExists(p))
        this->parent->DeleteDirectoryRecurse(p);
    }
    TempFS::~TempFS()
    {
        Close();
    }
    
}