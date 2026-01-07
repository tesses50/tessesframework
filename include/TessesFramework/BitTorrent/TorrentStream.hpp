#pragma once

#include "TessesFramework/Http/HttpClient.hpp"
#include "TessesFramework/Streams/NetworkStream.hpp"
namespace Tesses::Framework::BitTorrent
{
    class TorrentFileEntry {
        public:
            TorrentFileEntry();
            TorrentFileEntry(Tesses::Framework::Filesystem::VFSPath path, int64_t length);
            Tesses::Framework::Filesystem::VFSPath path;
            int64_t length=0;
    };
    class ReadWriteAt {
        public:
            virtual bool ReadBlockAt(uint64_t offset, uint8_t* data, size_t len)=0;
            virtual void WriteBlockAt(uint64_t offset, const uint8_t* data, size_t len)=0;
            virtual ~ReadWriteAt();

    };

    class TorrentFileStream : public ReadWriteAt
    {
        Tesses::Framework::Threading::Mutex mtx;
        public:
            std::shared_ptr<Tesses::Framework::Filesystem::VFS> vfs;
            Tesses::Framework::Filesystem::VFSPath path;
            uint64_t length;
            TorrentFileStream(std::shared_ptr<Tesses::Framework::Filesystem::VFS> vfs, Tesses::Framework::Filesystem::VFSPath path, uint64_t length);
            bool ReadBlockAt(uint64_t offset, uint8_t* data, size_t len);
            void WriteBlockAt(uint64_t offset, const uint8_t* data, size_t len);

    };
    class TorrentDirectoryStream : public ReadWriteAt
    {

        std::vector<Tesses::Framework::Threading::Mutex> mtxs;
        public:
            std::shared_ptr<Tesses::Framework::Filesystem::VFS> vfs;
            Tesses::Framework::Filesystem::VFSPath path;
            std::vector<TorrentFileEntry> entries;
            TorrentDirectoryStream(std::shared_ptr<Tesses::Framework::Filesystem::VFS> vfs, Tesses::Framework::Filesystem::VFSPath path,std::vector<TorrentFileEntry> entries);
            bool ReadBlockAt(uint64_t offset, uint8_t* data, size_t len);
            void WriteBlockAt(uint64_t offset, const uint8_t* data, size_t len);
    };
}