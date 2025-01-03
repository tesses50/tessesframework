#include "TessesFramework/Filesystem/VFS.hpp"
#include "TessesFramework/Http/HttpUtils.hpp"
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
    VFSPath VFS::ReadLink(VFSPath path)
    {
        return VFSPath("/");
    }
    VFSPath VFSPath::CollapseRelativeParents()
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
    VFSPath VFSPath::RelativeCurrentDirectory()
    {
        VFSPath path;
        path.relative=true;
        return path;
    }
    
    std::vector<std::string> VFSPath::SplitPath(std::string path,bool nativePath)
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
            else if(c == '\\' && nativePath)
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
    VFSPath::VFSPath(std::string str)
    {
        this->path = SplitPath(str);
        this->relative=true;
        if(!str.empty())
        {
            if(str.front() == '/') this->relative=false;
            if(!this->path.empty())
            {
                auto firstPartPath = this->path.front();

                if(!firstPartPath.empty() && firstPartPath.back() == '/') this->relative=false;
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
    VFSPath VFSPath::GetParent()
    {
        std::vector<std::string> paths;
        if(this->path.empty()) return VFSPath();
        paths.insert(paths.begin(), this->path.begin(), this->path.end()-1);
        auto res= VFSPath(paths);
        res.relative = this->relative;
        return res;
    }

    std::string VFSPath::GetFileName()
    {
        if(this->path.empty()) return "";
        return this->path.back();
    }

    std::string VFSPath::ToString()
    {
        if(this->path.empty() && !this->relative) return "/";
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
    void VFS::GetDate(VFSPath path, time_t& lastWrite, time_t& lastAccess)
    {

    }
    void VFS::SetDate(VFSPath path, time_t lastWrite, time_t lastAccess)
    {

    }
   
}