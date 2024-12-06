#include "TessesFramework/Filesystem/VFS.hpp"
#include "TessesFramework/Http/HttpUtils.hpp"
namespace Tesses::Framework::Filesystem
{
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
        return VFSPath(paths);
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
        std::vector<VFSPath> paths;
        GetPaths(src, paths);

        for(auto item : paths)
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
        std::vector<VFSPath> paths;
        GetPaths(path, paths);

        for(auto item : paths)
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
}