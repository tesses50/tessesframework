#include "TessesFramework/Filesystem/FSHelpers.hpp"
#include "TessesFramework/TextStreams/StreamReader.hpp"
#include "TessesFramework/TextStreams/StreamWriter.hpp"

namespace Tesses::Framework::Filesystem::Helpers
{
    void ReadAllText(std::shared_ptr<VFS> vfs, VFSPath path, std::string& text)
    {
        auto file = vfs->OpenFile(path,"rb");
        if(file->CanRead())
        {
            TextStreams::StreamReader reader(file);
            reader.ReadToEnd(text);
        }
    }
    void ReadAllLines(std::shared_ptr<VFS> vfs, VFSPath path, std::vector<std::string>& lines)
    {
        auto file = vfs->OpenFile(path,"rb");
        if(file->CanRead())
        {
            TextStreams::StreamReader reader(file);
            reader.ReadAllLines(lines);
        }
    }
    void ReadAllBytes(std::shared_ptr<VFS> vfs, VFSPath path, std::vector<uint8_t>& array)
    {
        auto file = vfs->OpenFile(path,"rb");
        if(file->CanRead())
        {
            if(file->CanSeek())
            {
                size_t length = (size_t)file->GetLength();
                array.resize(length);
                file->ReadBlock(array.data(), array.size());             
            }
            else
            {
                size_t totalSize = 0;
                size_t read = 0;
                do {
                    array.resize(totalSize+1024);
                    read = file->ReadBlock(array.data()+totalSize,1024);
                    totalSize += read;
                } while(read != 0);

                array.resize(totalSize);
            }
        }
    }
    std::string ReadAllText(std::shared_ptr<VFS> vfs, VFSPath path)
    {
        std::string text;
        ReadAllText(vfs,path,text);
        return text;
    }
    std::vector<std::string> ReadAllLines(std::shared_ptr<VFS> vfs, VFSPath path)
    {
        std::vector<std::string> lines;
        ReadAllLines(vfs,path,lines);
        return lines;
    }
    std::vector<uint8_t> ReadAllBytes(std::shared_ptr<VFS> vfs, VFSPath path)
    {
        std::vector<uint8_t> bytes;
        ReadAllBytes(vfs,path,bytes);
        return bytes;
    }
    void WriteAllText(std::shared_ptr<VFS> vfs, VFSPath path, const std::string& text)
    {
        auto file = vfs->OpenFile(path,"wb");
        if(file->CanWrite())
        {
            TextStreams::StreamWriter writer(file);
            writer.Write(text);
        }
    }
    void WriteAllLines(std::shared_ptr<VFS> vfs, VFSPath path, const std::vector<std::string>& parts)
    {
        auto file = vfs->OpenFile(path,"wb");
        if(file->CanWrite())
        {
            TextStreams::StreamWriter writer(file);
            for(auto& line : parts)
            {
                writer.WriteLine(line);
            }
        }
    }
    void WriteAllBytes(std::shared_ptr<VFS> vfs, VFSPath path, const std::vector<uint8_t>& bytes)
    {
        auto file = vfs->OpenFile(path,"wb");
        if(file->CanWrite())
        {
            file->WriteBlock(bytes.data(),bytes.size());
        }
    }

    void CopyFile(std::shared_ptr<VFS> vfsSrc, VFSPath pathSrc, std::shared_ptr<VFS> vfsDest, VFSPath pathDest, bool overwrite)
    {
        if(!overwrite && vfsDest->FileExists(pathDest)) return;
        if(!vfsSrc->FileExists(pathSrc)) return;
        auto src=vfsSrc->OpenFile(pathSrc,"wb");
        auto dest = vfsDest->OpenFile(pathDest,"wb");
        if(src->CanRead() && dest->CanWrite())
        src->CopyTo(dest);
    }

    void CopyStreamProgress(std::shared_ptr<Streams::Stream> src,std::shared_ptr<Streams::Stream> dest, std::function<void(int64_t offset, int64_t length)> progress)
    {
        int64_t length=0;
        try {
            length = src->GetLength();   
        } catch(...) {
            length=0;
        }
        if(length == 0) length = (int64_t)1<<62; // a big number so its always 0% if the stream does not have a length

        int64_t offset = 0;
        size_t read = 0;
        std::vector<uint8_t> data(4096);
        
        
        do {
            read = src->ReadBlock(data.data(),data.size());
            dest->WriteBlock(data.data(),read);
            offset += (int64_t)read;
            if(read != 0)
                progress(offset,length);
        } while(read != 0);

        if(offset > 0)
        progress(offset,offset);
    }
    void CopyFile(std::shared_ptr<VFS> vfsSrc, VFSPath pathSrc, std::shared_ptr<VFS> vfsDest, VFSPath pathDest, std::function<void(int64_t offset, int64_t length)> progress, bool overwrite)
    {
        if(!overwrite && vfsDest->FileExists(pathDest)) return;
        if(!vfsSrc->FileExists(pathSrc)) return;
        auto src=vfsSrc->OpenFile(pathSrc,"wb");
        auto dest = vfsDest->OpenFile(pathDest,"wb");
        if(src->CanRead() && dest->CanWrite())
        CopyStreamProgress(src,dest,progress);
    }
    void CopyDirectory(std::shared_ptr<VFS> vfsSrc, VFSPath pathSrc, std::shared_ptr<VFS> vfsDest, VFSPath pathDest,bool overwrite)
    {


        if(vfsSrc->DirectoryExists(pathSrc))
        {
            vfsDest->CreateDirectory(pathDest);
            for(auto& srcPath : vfsSrc->EnumeratePaths(pathSrc))
            {
                if(vfsSrc->DirectoryExists(srcPath))
                {
                    CopyDirectory(vfsSrc,srcPath,vfsDest,pathDest / srcPath.GetFileName());
                }
                if(vfsSrc->FileExists(srcPath))
                {
                    CopyFile(vfsSrc,srcPath,vfsDest,pathDest / srcPath.GetFileName(),overwrite);
                }
            }
        }
    }
    void CopyDirectory(std::shared_ptr<VFS> vfsSrc, VFSPath pathSrc, std::shared_ptr<VFS> vfsDest, VFSPath pathDest, std::function<void(int64_t offset, int64_t length, VFSPath currentFile)> progress, bool overwrite)
    {
        if(vfsSrc->DirectoryExists(pathSrc))
        {
            vfsDest->CreateDirectory(pathDest);
            for(auto& srcPath : vfsSrc->EnumeratePaths(pathSrc))
            {
                if(vfsSrc->DirectoryExists(srcPath))
                {
                    CopyDirectory(vfsSrc,srcPath,vfsDest,pathDest / srcPath.GetFileName(),progress,overwrite);
                }
                if(vfsSrc->FileExists(srcPath))
                {
                    CopyFile(vfsSrc,srcPath,vfsDest,pathDest / srcPath.GetFileName(),[progress,srcPath](int64_t offset, int64_t length)->void {
                        progress(offset,length,srcPath);
                    },overwrite);
                }
            }
        }
    }

}