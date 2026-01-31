#include "TessesFramework/Filesystem/VFS.hpp"
#include "TessesFramework/Http/HttpUtils.hpp"
#include "TessesFramework/Filesystem/LocalFS.hpp"
namespace Tesses::Framework::Filesystem
{
    VFSPathEnumeratorItterator::VFSPathEnumeratorItterator()
    {
        this->enumerator=nullptr;
    }
    VFSPathEnumeratorItterator::VFSPathEnumeratorItterator(VFSPathEnumerator* enumerator)
    {
        this->enumerator = enumerator;
    }
    VFSPathEnumeratorItterator& VFSPathEnumeratorItterator::operator++(int)
    {
        enumerator->MoveNext();
        return *this;
    }
    VFSPathEnumeratorItterator& VFSPathEnumeratorItterator::operator++()
    {
        enumerator->MoveNext();
        return *this;
    }
    
    
    VFSPath& VFSPathEnumeratorItterator::operator*()
    {
        std::filesystem::directory_iterator i;
        
        if(enumerator != nullptr)
            return enumerator->Current;
        return this->e;
    }
    VFSPath* VFSPathEnumeratorItterator::operator->()
    {
        if(enumerator != nullptr)
            return &enumerator->Current;
        return nullptr;
    }
    bool VFSPathEnumeratorItterator::operator!=(VFSPathEnumeratorItterator right)
    {
        if(enumerator == right.enumerator)
        {
            return false;
        }
        if(right.enumerator == nullptr)
        {
            auto r = !enumerator->IsDone();
           
            return r;
        }
        return true;
    }
    bool VFSPathEnumeratorItterator::operator==(VFSPathEnumeratorItterator right)
    {
        if(enumerator  == right.enumerator)
        {
            return true;
        }
        if(right.enumerator == nullptr)
            return enumerator->IsDone();
        return false;
    }
    VFSPathEnumerator::VFSPathEnumerator()
    {
        data = nullptr;
    }
    VFSPathEnumerator* VFSPathEnumerator::MakePointer()
    {
        VFSPathEnumerator* enumerator = new VFSPathEnumerator();
        enumerator->Current = Current;
        enumerator->data = data;
        return enumerator;
    }
    VFSPathEnumerator::VFSPathEnumerator(std::function<bool(VFSPath&)> moveNext,  std::function<void()> destroy)
    {
        data = std::make_shared<VFSPathEnumeratorData>(moveNext,destroy);
    }
    bool VFSPathEnumerator::MoveNext()
    {
        if(this->data)
        {
            auto r = data->moveNext(Current);
            if(!r) data->eof=true;
            return r;
        }
        return false;
    }
    bool VFSPathEnumerator::IsDone()
    {
        
        if(this->data)
        {
            return data->eof;
        }
        return true;
    }

    VFSPathEnumeratorItterator VFSPathEnumerator::begin()
    {
        MoveNext();
        VFSPathEnumeratorItterator ittr(this);
        return ittr;
    }

    VFSPathEnumeratorItterator VFSPathEnumerator::end()
    {
        return VFSPathEnumeratorItterator();
    }
     VFSPath operator/(VFSPath p, VFSPath p2)
    {
        return VFSPath(p,p2);
    }
    VFSPath operator/(VFSPath p, std::string p2)
    {
        return VFSPath(p,p2);
    }
    VFSPath operator/(std::string p, VFSPath p2)
    {
        return VFSPath(p,p2);
    }
    VFSPath operator+(VFSPath p, VFSPath p2)
    {
        VFSPath pout;
        pout.relative = p.relative;
        if(p.path.size()>1)
        {
            pout.path.insert(pout.path.end(),p.path.begin(),p.path.end()-1);
        }
        std::string mid={};
        if(!p.path.empty())
        {
            mid.append(p.path.back());
        }
        
        if(!p2.path.empty())
        {
            mid.append(p2.path.front());
        }
        pout.path.push_back(mid);

        if(p2.path.size()>1)
        {
            pout.path.insert(pout.path.end(),p2.path.begin()+1,p2.path.end());
        }

        return pout;
    }
    VFSPath operator+(VFSPath p, std::string p2)
    {
        return p + VFSPath(p2);
    }
    VFSPath operator+(std::string p, VFSPath p2)
    {
        return VFSPath(p) + p2;
    }
    bool operator==(VFSPath p,VFSPath p2)
    {
        if(p.relative != p2.relative) return false;
        if(p.path.size() != p2.path.size()) return false;
        for(size_t i = 0; i < p.path.size(); i++)
            if(p.path[i] != p2.path[i]) return false;
        return true;
    }
    bool operator!=(VFSPath p,VFSPath p2)
    {
        if(p.relative != p2.relative) return true;
        if(p.path.size() != p2.path.size()) return true;
        for(size_t i = 0; i < p.path.size(); i++)
            if(p.path[i] != p2.path[i]) return true;
        return false;
    
    }
    bool operator==(std::string p,VFSPath p2)
    {
        return VFSPath(p) == p2;
    }
    bool operator!=(std::string p,VFSPath p2)
    {
        return VFSPath(p) != p2;
    }
    bool operator==(VFSPath p,std::string p2)
    {
        return p == VFSPath(p2);
    }
    bool operator!=(VFSPath p,std::string p2)
    {
        return p != VFSPath(p2);
    }
    VFSPath VFS::ReadLink(VFSPath path)
    {
        return VFSPath("/");
    }
    VFSPath VFSPath::GetAbsoluteCurrentDirectory()
    {
        auto p = std::filesystem::current_path();
        return LocalFS->SystemToVFSPath(p.string());
    }
    void VFSPath::SetAbsoluteCurrentDirectory(VFSPath path)
    {
        auto res = LocalFS->VFSPathToSystem(path);
        std::filesystem::path mpath=res;
        std::filesystem::current_path(mpath);
    }
    VFSPath VFSPath::MakeAbsolute() const
    {
        return MakeAbsolute(GetAbsoluteCurrentDirectory());
    }
    VFSPath VFSPath::MakeAbsolute(VFSPath curDir) const
    {
        if (!this->relative) return *this;
        VFSPath p2 = curDir / *this;
        return p2.CollapseRelativeParents();
    }
    VFSPath VFSPath::MakeRelative() const
    {
        return MakeRelative(GetAbsoluteCurrentDirectory());
    }
    VFSPath VFSPath::MakeRelative(VFSPath toMakeRelativeTo) const
    {

        if(this->relative) return *this;
        
        
        
        size_t i;
        size_t len = std::min(toMakeRelativeTo.path.size(),this->path.size());
        for(i = 0; i < len; i++)
        {
            if(this->path[i] != toMakeRelativeTo.path[i]) break;
        }

        if(i == this->path.size()-1 && i == toMakeRelativeTo.path.size()-1)
        {
            std::vector<std::string> paths{this->path[this->path.size()-1]};
            VFSPath path(paths);
            path.relative = true;
            return path;
        }

        std::vector<std::string> parts(this->path.begin()+i, this->path.end());
        
        if(i < toMakeRelativeTo.path.size())
        {
            for(; i < toMakeRelativeTo.path.size();i++)
            {
                parts.insert(parts.begin(),"..");
            }
        }
        VFSPath p2(parts);
        p2.relative = true;
        return p2;
    }
    VFSPath VFSPath::CollapseRelativeParents() const
    {
        std::vector<std::string> parts;

        for(auto item : this->path)
        {
            if(item == "..")
            {
                if(!parts.empty())
                {
                    parts.erase(parts.end()-1);
                }
            }
            else if(item == ".") 
            {
                //do nothing but don't emit this
            }
            else
            {
                parts.push_back(item);
            }
        }
        VFSPath newpath;
        newpath.relative = relative;
        newpath.path = parts;
        return newpath;
    }
    VFSPath VFSPath::CurrentDirectoryAsRelative()
    {
        VFSPath path;
        path.relative=true;
        return path;
    }
    
    std::vector<std::string> VFSPath::SplitPath(std::string path)
    {
        std::vector<std::string> parts;
        std::string curPath = {};
        for(auto c : path)
        {
            if(c == '/')
            {
                if(!curPath.empty())
                {
                    parts.push_back(curPath);
                    curPath = {};
                }
            }
            #if defined(WIN32)
            else if(c == '\\')
            {
                if(!curPath.empty())
                {
                    parts.push_back(curPath);
                    curPath = {};
                }
            }
            #endif
            else
            {
                curPath.push_back(c);
            }
        }
        if(!curPath.empty())
        {
            parts.push_back(curPath);
            curPath = {};
        }

        return parts;
    }
    VFSPath::VFSPath()
    {
        this->relative=false;
    }
    VFSPath::VFSPath(std::vector<std::string> p)
    {
        this->path = p;
    }

    VFSPath VFSPath::ParseUriPath(std::string path)
    {
        std::string builder = {};
        VFSPath vpath;
        vpath.relative=true;

        if(!path.empty() && path[0] == '/') vpath.relative=false;

        for(auto item : path)
        {
            if(item == '/')
            {
                if(!builder.empty())
                {
                    vpath.path.push_back(builder);
                    builder.clear();
                }
            }
            else {
                builder.push_back(item);
            }
        }
        if(!builder.empty())
        {
            vpath.path.push_back(builder);
        }
        return vpath;
    }

    bool VFSPath::HasExtension() const
    {
        if(this->path.empty()) return false;
        auto& str = this->path.back();
        auto index = str.find_last_of('.');
        if(index == std::string::npos) return false;
        return true;
    }
    std::string VFSPath::GetExtension() const
    {
        if(this->path.empty()) return {};
        auto& str = this->path.back();
        auto index = str.find_last_of('.');
        if(index == std::string::npos) return {};
        return str.substr(index);
    }
    void VFSPath::ChangeExtension(std::string ext)
    {
        if(this->path.empty()) return;
        auto& str = this->path.back();
        auto index = str.find_last_of('.');
        if(index != std::string::npos)
        {
            str = str.substr(0,index);
        }
        if(ext.empty()) return;
        if(ext[0] != '.') 
        {
            str += '.';
            str += ext;
        }
        else
        {
            str += ext;
        }
    }
    void VFSPath::RemoveExtension()
    {
        ChangeExtension({});
    }
    
    VFSPath::VFSPath(std::string str)
    {
        this->path = SplitPath(str);
        this->relative=true;
        if(!str.empty())
        {
            if(str.front() == '/') this->relative=false;
            #if defined(_WIN32)
            if(str.front() == '\\') this->relative=false;
            #endif
            if(!this->path.empty())
            {
                auto firstPartPath = this->path.front();

                if(!firstPartPath.empty() && firstPartPath.back() == ':') this->relative=false;
            }
        }
    }
    VFSPath::VFSPath(VFSPath p1, VFSPath p2)
    {
        this->relative = p1.relative;
        this->path.insert(this->path.end(),p1.path.begin(),p1.path.end());
        this->path.insert(this->path.end(),p2.path.begin(),p2.path.end());
    }   
    VFSPath::VFSPath(VFSPath p1, std::string subpath) : VFSPath(p1, VFSPath(subpath))
    {

    }
    VFSPath VFSPath::GetParent() const
    {
        std::vector<std::string> paths;
        if(this->path.empty()) return VFSPath();
        if(!this->relative && this->path.size() == 1 && !this->path[0].empty() && this->path[0].back() == ':') return *this;

        paths.insert(paths.begin(), this->path.begin(), this->path.end()-1);
        auto res= VFSPath(paths);
        res.relative = this->relative;
        return res;
    }

    std::string VFSPath::GetFileName() const
    {
        if(this->path.empty()) return "";
        return this->path.back();
    }

    std::string VFSPath::ToString() const
    {
        if(this->path.empty() && !this->relative) return "/";
        if(!this->relative && this->path.size() == 1 && !this->path[0].empty() && this->path[0].back() == ':') return this->path[0] + "/";
        bool first=true;
        std::string p = {};
        for(auto item : this->path)
        {
            if(!(first && !item.empty() && item.back()==':') && !(first && this->relative))
                p.push_back('/');
            p.append(item);
            first=false;
        }
        return p;
    }

    VFS::~VFS()
    {

    }

    bool VFS::FIFOFileExists(VFSPath path) {return false;}
    bool VFS::SocketFileExists(VFSPath path) {return false;}
    bool VFS::CharacterDeviceExists(VFSPath path) {return false;}
    bool VFS::BlockDeviceExists(VFSPath path) {return false;}
    bool VFS::SymlinkExists(VFSPath path) {return false;}
    void VFS::MoveDirectory(VFSPath src, VFSPath dest)
    {
        
        for(auto item : EnumeratePaths(src))
        {
            if(DirectoryExists(item))
            {
                MoveDirectory(item, VFSPath(dest, item.GetFileName()));
            }
            else
            {
                MoveFile(item, VFSPath(dest, item.GetFileName()));
            }
        }

        DeleteDirectory(src); 
    }
    void VFS::CreateSymlink(VFSPath existingFile, VFSPath symlinkFile)
    {

    }
    void VFS::CreateHardlink(VFSPath existingFile, VFSPath newName)
    {
        
    }
    
    bool VFS::SpecialFileExists(VFSPath path)
    {
        return SymlinkExists(path) || BlockDeviceExists(path) || CharacterDeviceExists(path) || SocketFileExists(path) || FIFOFileExists(path);
    }
    bool VFS::FileExists(VFSPath path)
    {
        return RegularFileExists(path) || SpecialFileExists(path);
    }

    void VFS::DeleteDirectoryRecurse(VFSPath path)
    {
        if(!DirectoryExists(path)) return;
       
        for(auto item : EnumeratePaths(path))
        {
            if(DirectoryExists(item))
            {
                DeleteDirectoryRecurse(item);
            }
            else
            {
                DeleteFile(item);
            }
        }
        DeleteDirectory(path);
    }
    void VFS::GetDate(VFSPath path, Date::DateTime& lastWrite, Date::DateTime& lastAccess)
    {

    }
    void VFS::SetDate(VFSPath path, Date::DateTime lastWrite, Date::DateTime lastAccess)
    {

    }
    bool VFS::StatVFS(VFSPath path, StatVFSData& data)
    {
        data.BlockSize = 512;
        data.Blocks=10000000000000;
        data.BlocksFree=10000000000000;
        data.BlocksAvailable=10000000000000;
        data.AvailableInodes=10000000000000;
        data.TotalInodes=10000000000000;
        data.FreeInodes=10000000000000;
        data.FragmentSize=512;
        data.Id = 85138;
        data.Flags = 0;
        data.MaxNameLength=255;
        return true;
    }
    void VFS::Chmod(VFSPath path, uint32_t mode) {

    }
    void VFS::Close() {
        
    }
    void VFS::Lock(VFSPath path)
    {

    }
    void VFS::Unlock(VFSPath path)
    {
        
    }



    std::shared_ptr<FSWatcher> VFS::CreateWatcher(std::shared_ptr<VFS> vfs,VFSPath path)
    {
        return std::make_shared<FSWatcher>(vfs,path);
    }
    void FSWatcher::SetEnabled(bool enabled)
    {
        if(this->enabled == enabled) return;
        this->enabled = enabled;
        this->SetEnabledImpl(enabled);
    }

    bool FSWatcher::GetEnabled()
    {
        return this->enabled;
    }

    void FSWatcher::SetEnabledImpl(bool enabled)
    {

    }

    std::shared_ptr<VFS> FSWatcher::GetFilesystem()
    {
        return this->vfs;
    }
    const VFSPath& FSWatcher::GetPath()
    {
        return this->path;
    }

    FSWatcher::FSWatcher(std::shared_ptr<VFS> vfs, VFSPath path): vfs(vfs), path(path)
    {

    }
            
    std::shared_ptr<FSWatcher> FSWatcher::Create(std::shared_ptr<VFS> vfs, VFSPath path)
    {
        return vfs->CreateWatcher(vfs,path);
    }

    bool FSWatcherEvent::IsEvent(FSWatcherEventType e)
    {
        if(e == FSWatcherEventType::All) return this->type != FSWatcherEventType::None;
        if(e == FSWatcherEventType::Moved) return ((uint32_t)this->type & (uint32_t)FSWatcherEventType::Moved) == (uint32_t)FSWatcherEventType::Moved;
        if(e == FSWatcherEventType::Closed) return ((uint32_t)this->type & (uint32_t)FSWatcherEventType::Closed) != 0;
        return (uint32_t)this->type & (uint32_t)e;
    }

    std::string FSWatcherEvent::ToString()
    {
        if(IsEvent(FSWatcherEventType::Moved))
        {
            return (this->isDir ? "Moved directory " : "Moved file ") + this->src.ToString() + " -> " + this->dest.ToString();
        }
        else if(IsEvent(FSWatcherEventType::MoveOld))
        {
            return (this->isDir ? "Move source directory " : "Move source file ") + this->src.ToString();
        }
        else if(IsEvent(FSWatcherEventType::MoveNew))
        {
            return (this->isDir ? "Move destination directory " : "Move destination file ") + this->src.ToString();
        }
        else if(IsEvent(FSWatcherEventType::Accessed))
        {
            return (this->isDir ? "Accessed directory " : "Accessed file ") + this->src.ToString();
        }
        else if(IsEvent(FSWatcherEventType::AttributeChanged))
        {
            return (this->isDir ? "Changed attr on directory " : "Changed attr on file ") + this->src.ToString();
        }
        else if(IsEvent(FSWatcherEventType::Writen))
        {
            return (this->isDir ? "Finished changing directory " : "Finished writing to file ") + this->src.ToString();
        }
        else if(IsEvent(FSWatcherEventType::Read))
        {
            return (this->isDir ? "Finished reading directory " : "Finished reading from file ") + this->src.ToString();
        }
        else if(IsEvent(FSWatcherEventType::Created))
        {
            return (this->isDir ? "Created directory " : "Created file ") + this->src.ToString();
        }
        else if(IsEvent(FSWatcherEventType::Deleted))
        {
            return (this->isDir ? "Deleted directory " : "Deleted file ") + this->src.ToString();
        }
        else if(IsEvent(FSWatcherEventType::WatchEntryDeleted))
        {

            return (this->isDir ? "Deleted watched directory " : "Deleted watched file ") + this->src.ToString();
        }
        else if(IsEvent(FSWatcherEventType::Modified))
        {
            return (this->isDir ? "Modified directory " : "Modified file ") + this->src.ToString();
        }
        else if(IsEvent(FSWatcherEventType::WatchEntryMoved))
        {
            return (this->isDir ? "Moved watched directory " : "Moved watched file ") + this->src.ToString();
        }
        else if(IsEvent(FSWatcherEventType::Opened))
        {
            return (this->isDir ? "Opened directory " : "Opened file ") + this->src.ToString();
        }

        return "";
    }
}   