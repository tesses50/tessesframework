#include "TessesFramework/Http/FileServer.hpp"
#include "TessesFramework/Filesystem/LocalFS.hpp"
#include "TessesFramework/Filesystem/SubdirFilesystem.hpp"
#include <iostream>
#include "TessesFramework/Common.hpp"
using LocalFilesystem = Tesses::Framework::Filesystem::LocalFilesystem;
using SubdirFilesystem = Tesses::Framework::Filesystem::SubdirFilesystem;
using VFSPath = Tesses::Framework::Filesystem::VFSPath;
using VFS = Tesses::Framework::Filesystem::VFS;
namespace Tesses::Framework::Http
{
    FileServer::FileServer(std::filesystem::path path,bool allowListing,bool spa) : FileServer(path,allowListing,spa,{"index.html","default.html","index.htm","default.htm"})
    {

    }
    FileServer::FileServer(std::filesystem::path path,bool allowListing, bool spa, std::vector<std::string> defaultNames)
    {
        LocalFilesystem* lfs=new LocalFilesystem();
        SubdirFilesystem* sdfs=new SubdirFilesystem(lfs,lfs->SystemToVFSPath(path.string()),true);
        this->vfs = sdfs;
        this->spa = spa;

        this->ownsVFS=true;
        this->allowListing = allowListing;
        this->defaultNames = defaultNames;
    }
    FileServer::FileServer(Tesses::Framework::Filesystem::VFS* fs, bool owns, bool allowListing,bool spa) : FileServer(fs,owns,allowListing,spa,{"index.html","default.html","index.htm","default.htm"})
    {
        
    }
    FileServer::FileServer(Tesses::Framework::Filesystem::VFS* fs, bool owns, bool allowListing, bool spa, std::vector<std::string> defaultNames)
    {
        this->vfs = fs;
        this->ownsVFS = owns;
        this->allowListing = allowListing;
        this->defaultNames = defaultNames;
        this->spa = spa;
    }
    bool FileServer::SendFile(ServerContext& ctx,VFSPath path)
    {
        TF_LOG("File: " + path.ToString());
        auto strm = this->vfs->OpenFile(path,"rb");
        bool retVal = false;
        if(strm != nullptr)
        {
            Date::DateTime lw,la;
            this->vfs->GetDate(path,lw,la);
            ctx.WithLastModified(lw).WithMimeType(HttpUtils::MimeType(path.GetFileName())).SendStream(strm);
            retVal = true;
           
        }

        delete strm;
        return retVal;
    }

    bool FileServer::Handle(ServerContext& ctx)
    {
        auto path = HttpUtils::UrlPathDecode(ctx.path);
        
        if(this->vfs->DirectoryExists(path))
        {
            TF_LOG("Directory exists");
            for(auto f : defaultNames)
            {
                VFSPath p=path;
                p = p / f;
                TF_LOG("Trying " + p.ToString());
                TF_LOG("Before file exists");
                TF_LOG(this->vfs->FileExists(p)?"File Exists" : "File Does Not Exist");
                TF_LOG("After file exists");
                if(this->vfs->FileExists(p))
                    return SendFile(ctx,p);
            }
            if(this->allowListing)
            {
                std::string p = HttpUtils::HtmlEncode(ctx.originalPath);
                std::string html = "<!DOCTYPE html><html><head><title>Index of ";
                html.append(p);
                html.append("</title></head><body><h1>Index of ");
                html.append(p);
                html.append("</h1><hr><pre><a href=\"../\">../</a>\r\n");
                

                for(auto item : vfs->EnumeratePaths(path))
                {
                    if(vfs->DirectoryExists(item))
                    {
                        //is dir
                        std::string path = item.GetFileName();
                        html.append("<a href=\"");
                        html.append(HttpUtils::UrlPathEncode(path) + "/");
                        html.append("\">");
                        html.append(HttpUtils::HtmlEncode(path) + "/");
                        html.append("</a>\r\n");
                    }
                    else
                    {
                        //is file
                        std::string path = item.GetFileName();
                        html.append("<a href=\"");
                        html.append(HttpUtils::UrlPathEncode(path));
                        html.append("\">");
                        html.append(HttpUtils::HtmlEncode(path));
                        html.append("</a>\r\n");
                    }
                }

                html.append("</pre><hr></body></html>");
                
                ctx.WithMimeType("text/html").SendText(html);
                return true;
            }
        }
        else if(this->vfs->FileExists(path))
        {
            return SendFile(ctx,path);
        }
        else if(this->spa)
        {
            for(auto f : defaultNames)
            {
                VFSPath p(f);
                p.relative=false;
                if(this->vfs->FileExists(p))
                    return SendFile(ctx,p);
            }
        }
        return false;
    }
    FileServer::~FileServer()
    {
        if(this->ownsVFS)
            delete this->vfs;
    }
}