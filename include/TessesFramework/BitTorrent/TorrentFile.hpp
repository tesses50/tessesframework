#pragma once
#include "../Serialization/Bencode.hpp"
#include "TorrentStream.hpp"
namespace Tesses::Framework::BitTorrent
{
    constexpr int DEFAULT_PIECE_LENGTH = 524288;
    
    class TorrentFileInfo {
        
        Serialization::Bencode::BeDictionary info;
        public:
            TorrentFileInfo();
            TorrentFileInfo(const Serialization::Bencode::BeDictionary& tkn);
            
            
            int64_t piece_length=DEFAULT_PIECE_LENGTH;
            Serialization::Bencode::BeString pieces;
            int64_t isPrivate=0;
            Serialization::Bencode::BeString name;
            std::variant<int64_t, std::vector<TorrentFileEntry>> fileListOrLength;
            int64_t GetTorrentFileSize();
            /// @brief Generate the info
            /// @return the dictionary containing the info
            Serialization::Bencode::BeDictionary& GenerateInfoDictionary();
            /// @brief Get the info hash for info (if you are filling out the fields, you need to call GenerateInfoDictionary first)
            /// @return the 20 byte info hash
            Serialization::Bencode::BeString GetInfoHash();
            /// @brief Get the info (does not regenerate info from the fields)
            /// @return the dictionary containing the info
            Serialization::Bencode::BeDictionary& GetInfoDictionary();

            /// @brief Used to load a dictionary in, will overwrite all the fields
            /// @param dict the dictionary to load
            void Load(const Serialization::Bencode::BeDictionary& dict);
            /// @brief Gets a read write based on this info object
            /// @param vfs the vfs object
            /// @param path file or directory inside vfs
            /// @return The read write at object
            std::shared_ptr<ReadWriteAt> GetStreamFromFilesystem(std::shared_ptr<Tesses::Framework::Filesystem::VFS> vfs, const Tesses::Framework::Filesystem::VFSPath& path);

            /// @brief Create info from file/directory
            /// @param vfs the vfs object
            /// @param path file or directory inside vfs
            /// @param isPrivate whether you use private trackers
            /// @param pieceLength length of pieces
            void CreateFromFilesystem(std::shared_ptr<Tesses::Framework::Filesystem::VFS> vfs, const Tesses::Framework::Filesystem::VFSPath& path, bool isPrivate=false, int64_t pieceLength=DEFAULT_PIECE_LENGTH);
    };
    class TorrentFile {
        public:
            TorrentFile();
            TorrentFile(const Serialization::Bencode::BeDictionary& tkn);


            Serialization::Bencode::BeString announce;
            std::vector<Serialization::Bencode::BeString> announce_list;
            std::vector<Serialization::Bencode::BeString> url_list;
            Serialization::Bencode::BeString comment;
            Serialization::Bencode::BeString created_by;
            int64_t creation_date=0;
            TorrentFileInfo info;


            Serialization::Bencode::BeDictionary ToDictionary(); 

            void Print(std::shared_ptr<TextStreams::TextWriter> writer);

            static void CreateTorrent(std::shared_ptr<Tesses::Framework::Streams::Stream> torrent_file_stream,const std::vector<Serialization::Bencode::BeString>& trackers,const std::vector<Serialization::Bencode::BeString>& webseeds, std::shared_ptr<Tesses::Framework::Filesystem::VFS> vfs, const Tesses::Framework::Filesystem::VFSPath& path, bool isPrivate=false, int64_t pieceLength=DEFAULT_PIECE_LENGTH, Serialization::Bencode::BeString comment="", Serialization::Bencode::BeString created_by = "TessesFrameworkTorrent");
            
    };
}