#include "TessesFramework/BitTorrent/TorrentStream.hpp"

namespace Tesses::Framework::BitTorrent
{
    TorrentFileEntry::TorrentFileEntry()
    {

    }
    TorrentFileEntry::TorrentFileEntry(Tesses::Framework::Filesystem::VFSPath path, int64_t length): path(path), length(length)
    {}

    ReadWriteAt::~ReadWriteAt(){}

    TorrentFileStream::TorrentFileStream(std::shared_ptr<Tesses::Framework::Filesystem::VFS> vfs, Tesses::Framework::Filesystem::VFSPath path,uint64_t length) : vfs(vfs), path(path), length(length)
    {
    }
    bool TorrentFileStream::ReadBlockAt(uint64_t offset, uint8_t* data, size_t len)
    {
        if(!vfs->FileExists(path)) return false;
        auto strm = vfs->OpenFile(path,"rb");
        strm->Seek((int64_t)offset,Streams::SeekOrigin::Begin);
        strm->ReadBlock(data,len);
        return true;
    }
    void TorrentFileStream::WriteBlockAt(uint64_t offset, const uint8_t* data, size_t len)
    {
        len = (size_t)std::min((int64_t)len, (int64_t)this->length-(int64_t)offset);
        if(len == 0) return;

        mtx.Lock();
        auto strm = vfs->OpenFile(path,"wb");
        strm->Seek((int64_t)offset,Streams::SeekOrigin::Begin);
        strm->WriteBlock(data,len);
        mtx.Unlock();
    }

     TorrentDirectoryStream::TorrentDirectoryStream(std::shared_ptr<Tesses::Framework::Filesystem::VFS> vfs, Tesses::Framework::Filesystem::VFSPath path,std::vector<TorrentFileEntry> entries) : vfs(vfs), path(path), entries(entries)
     {
        if(!vfs->DirectoryExists(path))
            this->vfs->CreateDirectory(path);
        this->mtxs.resize(entries.size());
     }

     // From https://www.seanjoflynn.com/research/bittorrent.html , which is licensed under MIT based on code repo
     bool TorrentDirectoryStream::ReadBlockAt(uint64_t offset, uint8_t* data, size_t len)
     {
        uint64_t currentOffset = 0;
        uint64_t end = offset + len;
        for(size_t i = 0; i < this->entries.size(); i++)
        {
            if(offset < currentOffset && end < currentOffset) {
                currentOffset += this->entries[i].length;
                continue;
            }

            if(offset > currentOffset + this->entries[i].length && end > currentOffset + this->entries[i].length ){
                currentOffset += this->entries[i].length;
                continue;
            }
            
            auto path = this->path / this->entries[i].path;
            if(!vfs->FileExists(path)) return false;

            int64_t fstart = std::max((int64_t)0,(int64_t)offset - (int64_t)currentOffset);
            int64_t fend = std::min((int64_t)end - (int64_t)currentOffset, (int64_t)this->entries[i].length);
            int flength = (int)(fend - fstart);
            int bstart = std::max((int)0,(int)((int64_t)currentOffset - (int64_t)offset));
            auto strm = vfs->OpenFile(path,"rb");
            strm->Seek(fstart, Streams::SeekOrigin::Begin);
            strm->ReadBlock(data+bstart,flength);



            currentOffset += this->entries[i].length;


        }
        return true;
     }

     // From https://www.seanjoflynn.com/research/bittorrent.html , which is licensed under MIT based on code repo
     void TorrentDirectoryStream::WriteBlockAt(uint64_t offset, const uint8_t* data, size_t len)
     {
        uint64_t currentOffset = 0;
        uint64_t end = offset + len;
        for(size_t i = 0; i < this->entries.size(); i++)
        {
            if(offset < currentOffset && end < currentOffset) {
                currentOffset += this->entries[i].length;
                continue;
            }

            if(offset > currentOffset + this->entries[i].length && end > currentOffset + this->entries[i].length ){
                currentOffset += this->entries[i].length;
                continue;
            }
            
            auto path = this->path / this->entries[i].path;
            auto parent = path.GetParent();
            if(!vfs->DirectoryExists(parent))
                vfs->CreateDirectory(parent);

            int64_t fstart = std::max((int64_t)0,(int64_t)offset - (int64_t)currentOffset);
            int64_t fend = std::min((int64_t)end - (int64_t)currentOffset, (int64_t)this->entries[i].length);
            int flength = (int)(fend - fstart);
            int bstart = std::max((int)0,(int)((int64_t)currentOffset - (int64_t)offset));
            
            this->mtxs[i].Lock();
            {
                auto strm = vfs->OpenFile(path,"wb");
                strm->Seek(fstart, Streams::SeekOrigin::Begin);
                strm->WriteBlock(data+bstart,flength);
            }
            this->mtxs[i].Unlock();


            currentOffset += this->entries[i].length;


        }
     }
}