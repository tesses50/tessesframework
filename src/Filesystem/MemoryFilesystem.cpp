#include "TessesFramework/Filesystem/MemoryFilesystem.hpp"
#include <iostream>

namespace Tesses::Framework::Filesystem
{
    MemoryFilesystemStream::MemoryFilesystemStream(std::shared_ptr<Tesses::Framework::Threading::Mutex> mtx, std::shared_ptr<MemoryFileData> data,bool canRead, bool canWrite, bool canSeek)
    {
        this->mtx = mtx;
        this->data = data;
        this->canRead = canRead;
        this->canWrite = canWrite;
        this->canSeek = canSeek;
        this->pos=0;
    }
    size_t MemoryFilesystemStream::Read(uint8_t* buff, size_t sz)
    {
        if(!this->canRead) return 0;
        if(this->pos >= this->data->file.size()) return 0;
       
        size_t toRead = std::min(sz, (size_t)(this->data->file.size()-this->pos));
       
        memcpy(buff, this->data->file.data() + this->pos, toRead);

        this->pos += toRead;
        return toRead;
    }
    size_t MemoryFilesystemStream::Write(const uint8_t* buff, size_t sz)
    {
        if(!this->canWrite) return 0;
        if(this->canSeek)
        {

            if(this->pos > this->data->file.size())
            {
                this->data->file.resize(this->pos+sz);
            }
            this->data->file.insert(this->data->file.begin()+this->pos, buff, buff+sz);
            this->pos+=sz;
             
        } 
        else 
        {
            this->data->file.insert(this->data->file.end(), buff, buff+sz);
        }
        return sz;
    }
    bool MemoryFilesystemStream::CanRead()
    {
        return canRead;
    }
    bool MemoryFilesystemStream::CanWrite()
    {
        return canWrite;
    }
    bool MemoryFilesystemStream::CanSeek()
    {
        return canSeek;
    }
    int64_t MemoryFilesystemStream::GetPosition()
    {
        if(!this->canSeek) return (int64_t)this->data->file.size();
        return pos;
    }
    void MemoryFilesystemStream::Flush()
    {
        //its already flushed
    }
    void MemoryFilesystemStream::Seek(int64_t pos, Streams::SeekOrigin whence)
    {
        if(canSeek) return;
         switch(whence)
        {
            case Streams::SeekOrigin::Begin:
                this->pos = (size_t)pos;
                break;
            case Streams::SeekOrigin::Current:
                this->pos += (size_t)pos;
                break;
            case Streams::SeekOrigin::End:
                this->pos = (size_t)(this->data->file.size() + pos);
                break;
        }
    }
    MemoryFilesystemStream::~MemoryFilesystemStream()
    {
        mtx->Lock();
        if(this->canWrite) this->data->canAccess=true;
        this->data->readers--;
        mtx->Unlock();
    }   

    MemoryEntry* MemoryFilesystem::GetEntry(VFSPath path,bool followSymlink)
    {
        if(path.relative) return nullptr;
        if(path.path.empty()) return &this->root;
        
        auto entry = GetEntry(path.GetParent(),true);
        if(entry == nullptr) return nullptr;

        auto dir = dynamic_cast<MemoryDirectory*>(entry);
        if(dir != nullptr)
        {
            for(auto item : dir->entries)
            {
                if(item->name == path.GetFileName())
                {
                    auto link = dynamic_cast<MemorySymlink*>(item);
                    if(followSymlink && link != nullptr)
                    {
                        item = GetEntry(link->linkedTo,true);
                    }
                    return item;
                }
            }
        }
        return nullptr;
    }

    std::shared_ptr<Streams::Stream> MemoryFilesystem::OpenFile(VFSPath path, std::string mode)
    {
        bool canRead=false;
        bool canWrite=false;
        bool canSeek=true;
        bool mustExist=false;
        bool truncate=false;


        if(mode.size() >= 1)
        {
            if(mode[0] == 'r') 
            {
                canRead = true;
                mustExist=true;
            }
            else if(mode[0] == 'w')
            {
                canWrite = true;
                truncate=true;
            }
            else if(mode[0] == 'a')
            {
                canSeek = false;
                canWrite = true;
            }
        }

        if(((mode.size() >= 2  && mode[1] == '+') || (mode.size() >= 2 && mode[1] == 'b' && mode[2] == '+')))
        {
            canRead = true;
            canWrite = true;
        }

        mtx->Lock();
        if(mustExist)
        {
            auto file = GetEntry(path,true);

            if(file == nullptr) 
            {
                mtx->Unlock();
                return nullptr;
            }

            auto f = dynamic_cast<MemoryFile*>(file);
            if(f == nullptr) 
            {
                mtx->Unlock();
                return nullptr;
            }
            if(!f->data->canAccess) 
            {
                mtx->Unlock();
                return nullptr;
            }
            if(canWrite && f->data->readers > 0)
            {
                mtx->Unlock();
                return nullptr;
            }
            f->data->readers++;
            if(canWrite) f->data->canAccess=false;

            mtx->Unlock();
            return std::make_shared<MemoryFilesystemStream>(mtx,f->data,canRead,canWrite,canSeek);
        }
        else
        {
            auto file = dynamic_cast<MemoryFile*>(GetEntry(path,true));
            

            if(file != nullptr)
            {
                if(!file->data->canAccess) 
                {
                    mtx->Unlock();
                    return nullptr;
                }
                if(file->data->readers > 0) 
                {
                    mtx->Unlock();
                    return nullptr;
                }
                file->data->canAccess=false;
                file->data->readers++;
                if(truncate) {file->data->file.clear(); file->data->lastWrite=Date::DateTime::NowUTC();}
                mtx->Unlock();
                return std::make_shared<MemoryFilesystemStream>(mtx,file->data,canRead,canWrite,canSeek);
                
            }


            auto dir = GetEntry(path.GetParent(),true);
        
            if(dir == nullptr) 
            {
                mtx->Unlock();
                return nullptr;
            }

            auto myDir = dynamic_cast<MemoryDirectory*>(dir);
        

            if(myDir == nullptr)
            {
                mtx->Unlock();
                return nullptr;
            }

            MemoryFile* thefile=nullptr;
            
            for(auto f : myDir->entries)
            {
                if(f->name == path.GetFileName())
                {
                    auto symlink = dynamic_cast<MemorySymlink*>(f);
                    while(symlink != nullptr)
                    {
                        auto ent = GetEntry(symlink->name,false);
                        auto sym = dynamic_cast<MemorySymlink*>(f);
                        if(sym != nullptr)
                            symlink = sym;
                        else
                        {
                            auto myDir0 = dynamic_cast<MemoryDirectory*>(GetEntry(symlink->linkedTo.GetParent(),true));
                            if(myDir0 != nullptr)
                            {
                                for(auto f2 : myDir0->entries)
                                {
                                    if(f2->name == symlink->linkedTo.GetFileName())
                                    {
                                        auto myFile = dynamic_cast<MemoryFile*>(f2);
                                        if(myFile != nullptr)
                                        {
                                            thefile = myFile;
                                            break;
                                        }
                                        else
                                        {
                                            mtx->Unlock();
                                            return nullptr;
                                        }
                                    }
                                    
                                }
                                myDir = myDir0;
                            }
                            else
                            {
                                mtx->Unlock();
                                return nullptr;
                            }
                            break;
                        }
                    }
                    break;
                }
            }
            if(thefile == nullptr)
            {
                
                MemoryFile* f = new MemoryFile();
                f->name = path.GetFileName();
                f->data = std::make_shared<MemoryFileData>();
                f->data->canAccess=false;
                f->data->readers++;

                myDir->entries.push_back(f);

                mtx->Unlock();
                return std::make_shared<MemoryFilesystemStream>(mtx,f->data,canRead,canWrite,canSeek);
            }
            if(thefile != nullptr)
            {
                if(!thefile->data->canAccess) 
                {
                    mtx->Unlock();
                    return nullptr;
                }
                if(thefile->data->readers > 0) 
                {
                    mtx->Unlock();
                    return nullptr;
                }
                thefile->data->canAccess=false;
                thefile->data->readers++;
                if(truncate) {thefile->data->file.clear(); thefile->data->lastWrite=Date::DateTime::NowUTC();}
                mtx->Unlock();
                return std::make_shared<MemoryFilesystemStream>(mtx,thefile->data,canRead,canWrite,canSeek);
            }
        }

        mtx->Unlock();
        return nullptr;
    }

    void MemoryFilesystem::CreateDirectory(VFSPath path)
    {
        if(path.relative) return;
        if(path.path.empty()) return;
        mtx->Lock();
        MemoryDirectory* dir=&root;
        for(auto part : path.path)
        {
            bool have=false;
            for(auto dirent : dir->entries)
            {
                if(dirent->name == part)
                {
                    auto symlink = dynamic_cast<MemorySymlink*>(dirent);
                    if(symlink != nullptr)
                    {
                        dirent = GetEntry(symlink->linkedTo,true);
                    }
                    auto dirdirent = dynamic_cast<MemoryDirectory*>(dirent);
                    if(dirdirent != nullptr)
                    {
                        dir = dirdirent;
                        have=true;
                        break;
                    }
                    else
                    {
                        mtx->Unlock();
                        return;
                    }
                }
            }
            if(!have)
            {
                MemoryDirectory* dir2 = new MemoryDirectory();
                dir2->name = part;

                dir2->lastWrite=Date::DateTime::NowUTC();
                
                dir->entries.push_back(dir2);
                dir->lastWrite=Date::DateTime::NowUTC();
                
                dir=dir2;
            }
        }
        mtx->Unlock();
    }

    void MemoryFilesystem::DeleteFile(VFSPath path)
    {
        if(path.relative || path.path.empty()) return;
        mtx->Lock();
        MemoryDirectory* dir=&root;
        if(path.path.size() > 1)
        {
            dir = dynamic_cast<MemoryDirectory*>(GetEntry(path.GetParent(),true));
        }
        if(dir == nullptr)
        {
            mtx->Unlock();
            return;
        }
        std::string fname = path.GetFileName();
        for(auto index = dir->entries.begin(); index < dir->entries.end(); index++)
        {
            auto item = *index;
            if(item->name == fname)
            {
                auto p = dynamic_cast<MemoryDirectory*>(item);
                if(p == nullptr)
                {
                    delete item;
                    dir->entries.erase(index);
                    
                    dir->lastWrite=Date::DateTime::NowUTC();
                }
                break;
            }
        }
        mtx->Unlock();
    }

    bool MemoryFilesystem::RegularFileExists(VFSPath path)
    {

        if(path.relative) return false;
        if(path.path.empty()) return false;
        mtx->Lock();
        auto f = GetEntry(path,false);
        mtx->Unlock();
        return dynamic_cast<MemoryFile*>(f) != nullptr;
    }
    bool MemoryFilesystem::SymlinkExists(VFSPath path)
    {

        if(path.relative) return false;
        if(path.path.empty()) return false;
        mtx->Lock();
        auto f = GetEntry(path,false);
        mtx->Unlock();
        return dynamic_cast<MemorySymlink*>(f) != nullptr;
    }
    bool MemoryFilesystem::DirectoryExists(VFSPath path)
    {
        if(path.relative) return false;
        if(path.path.empty()) return true;
        mtx->Lock();
        auto f = GetEntry(path,false);
        mtx->Unlock();
        return dynamic_cast<MemoryDirectory*>(f) != nullptr;
    }
    
    void MemoryFilesystem::DeleteDirectory(VFSPath path)
    {
        if(path.relative || path.path.empty()) return;
        mtx->Lock();
        MemoryDirectory* dir=&root;
        if(path.path.size() > 1)
        {
            dir = dynamic_cast<MemoryDirectory*>(GetEntry(path.GetParent(),true));
        }
        if(dir == nullptr)
        {
            mtx->Unlock();
            return;
        }
        std::string fname = path.GetFileName();
        for(auto index = dir->entries.begin(); index < dir->entries.end(); index++)
        {
            auto item = *index;
            if(item->name == fname)
            {
                auto p = dynamic_cast<MemoryDirectory*>(item);
                if(p != nullptr)
                {
                    delete item;
                    dir->entries.erase(index);
                    dir->lastWrite=Date::DateTime::NowUTC();
                }
                break;
            }
        }
        mtx->Unlock();
    }

    void MemoryFilesystem::CreateSymlink(VFSPath existingFile, VFSPath symlinkFile)
    {
        if(existingFile.relative || symlinkFile.relative || symlinkFile.path.empty()) return;
        mtx->Lock();
        MemoryDirectory* dir=&root;
        if(symlinkFile.path.size() > 1)
        {
            dir = dynamic_cast<MemoryDirectory*>(GetEntry(symlinkFile.GetParent(),true));
        }
        if(dir == nullptr)
        {
            mtx->Unlock();
            return;
        }
        std::string fname = symlinkFile.GetFileName();
        for(auto index = dir->entries.begin(); index < dir->entries.end(); index++)
        {
            auto item = *index;
            if(item->name == fname)
            {
                auto p = dynamic_cast<MemorySymlink*>(item);
                if(p != nullptr)
                {
                    p->linkedTo = existingFile;
                    p->lastWrite = Date::DateTime::NowUTC();
                }
                mtx->Unlock();
                return;
            }
        }
        MemorySymlink* symlink = new MemorySymlink();
        symlink->name = fname;
        symlink->linkedTo = existingFile;
        symlink->lastWrite = Date::DateTime::NowUTC();
        dir->entries.push_back(symlink);
        dir->lastWrite = Date::DateTime::NowUTC();

        mtx->Unlock();
    }
    VFSPathEnumerator MemoryFilesystem::EnumeratePaths(VFSPath path)
    {
        std::pair<size_t,std::vector<std::string>>* paths=new std::pair<size_t,std::vector<std::string>>();
        paths->first=0;
        mtx->Lock();
        auto dir = dynamic_cast<MemoryDirectory*>(GetEntry(path,true));
        if(dir != nullptr)
        {
            for(auto item : dir->entries) paths->second.push_back(item->name);
        }
        mtx->Unlock();

        return VFSPathEnumerator([paths,path](VFSPath& _path)->bool{
            if(paths->first < paths->second.size())
            {
                _path = path /  paths->second[paths->first++];
                return true;
            }
            return false;
        },[paths]()->void{
            delete paths;
        });
    }

    
    void MemoryFilesystem::CreateHardlink(VFSPath existingFile, VFSPath newName)
    {
        mtx->Lock();
        auto existing = dynamic_cast<MemoryFile*>(GetEntry(existingFile,true));
        if(existing == nullptr) 
        {
            mtx->Unlock();
            return;
        }
          MemoryDirectory* dir=&root;
        if(newName.path.size() > 1)
        {
            dir = dynamic_cast<MemoryDirectory*>(GetEntry(newName.GetParent(),true));
        }
        if(dir == nullptr)
        {
            mtx->Unlock();
            return;
        }

        std::string fname = newName.GetFileName();
        for(auto index = dir->entries.begin(); index < dir->entries.end(); index++)
        {
            auto item = *index;
            if(item->name == fname)
            {
               
                mtx->Unlock();
                return;
            }
        }

        MemoryFile* memFile = new MemoryFile();
        memFile->name = fname;
        memFile->data = existing->data;
        dir->entries.push_back(memFile);
        dir->lastWrite=Date::DateTime::NowUTC();
        
        mtx->Unlock();
    }
    void MemoryFilesystem::MoveFile(VFSPath src, VFSPath dest)
    {
        DeleteFile(dest);
        CreateHardlink(src,dest);
        DeleteFile(src);
    }
    void MemoryFilesystem::MoveDirectory(VFSPath src, VFSPath dest)
    {
        CreateDirectory(dest);
        for(auto ent : EnumeratePaths(src))
        {
            VFSPath destPath = dest / ent.GetFileName();
            if(FileExists(ent)) MoveFile(ent,destPath);
            if(DirectoryExists(ent)) MoveDirectory(ent,destPath);

        }
        DeleteDirectory(src);
    }

    VFSPath MemoryFilesystem::ReadLink(VFSPath p)
    {
        mtx->Lock();
        VFSPath p2;
        auto symlink = dynamic_cast<MemorySymlink*>(GetEntry(p,false));
        if(symlink != nullptr)
        {
            p2 = symlink->linkedTo;
        }
        mtx->Unlock();
        return p2;
    }
    std::string MemoryFilesystem::VFSPathToSystem(VFSPath path)
    {
        return path.ToString();
    }
    VFSPath MemoryFilesystem::SystemToVFSPath(std::string path)
    {
        return path;
    }
    void MemoryFilesystem::GetDate(VFSPath path, Date::DateTime& lastWrite, Date::DateTime& lastAccess)
    {
        
        mtx->Lock();
        auto node = GetEntry(path,false);
        auto dir = dynamic_cast<MemoryDirectory*>(node);

        auto file = dynamic_cast<MemoryFile*>(node);

        auto sym = dynamic_cast<MemorySymlink*>(node);

        if(dir != nullptr) lastWrite = dir->lastWrite;
        if(file != nullptr) lastWrite = file->data->lastWrite;
        if(sym != nullptr) lastWrite = sym->lastWrite;
        mtx->Unlock();
        lastAccess = lastWrite;
    }
    void MemoryFilesystem::SetDate(VFSPath path, Date::DateTime lastWrite, Date::DateTime lastAccess)
    {
        mtx->Lock();
        auto node = GetEntry(path,false);
        auto dir = dynamic_cast<MemoryDirectory*>(node);

        auto file = dynamic_cast<MemoryFile*>(node);

        auto sym = dynamic_cast<MemorySymlink*>(node);

        if(dir != nullptr) dir->lastWrite = lastWrite;
        if(file != nullptr) file->data->lastWrite = lastWrite;
        if(sym != nullptr) sym->lastWrite = lastWrite;
        mtx->Unlock();
    }
    MemoryFilesystem::~MemoryFilesystem()
    {

    }
    MemoryFilesystem::MemoryFilesystem()
    {
        mtx = std::make_shared<Threading::Mutex>();
    }

    MemoryEntry::~MemoryEntry()
    {

    }
    MemoryFile::~MemoryFile()
    {

    }
    MemoryDirectory::MemoryDirectory()
    {
        this->lastWrite = Date::DateTime::NowUTC();
    }
    MemoryDirectory::~MemoryDirectory()
    {
        for(auto item : this->entries) delete item;
    }
    MemoryFileData::MemoryFileData()
    {
        this->lastWrite = Date::DateTime::NowUTC();
        this->canAccess=true;
        this->readers=0;
        
    }
}
