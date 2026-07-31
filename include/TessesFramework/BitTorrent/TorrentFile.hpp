/*
    TessesFramework a library to make C++ easier for me, used in CrossLang:
    https://git.tesses.org/tesses50/crosslang

    Copyright (C) 2026 Mike Nolan
    SPDX-License-Identifier: GPL-3.0-or-later WITH TessesFramework-Exception-1.0

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once
#include "../Serialization/Bencode.hpp"
#include "TorrentStream.hpp"
namespace Tesses::Framework::BitTorrent {
/**
 * @brief Length of torrent pieces
 *
 */
constexpr int DEFAULT_PIECE_LENGTH = 524288;
/**
 * @brief The Torrent File Info Section
 *
 */
class TorrentFileInfo {

    Serialization::Bencode::BeDictionary info;

  public:
    /// @brief Constructs the object with empty data
    TorrentFileInfo();
    /// @brief Constructs the object with bencoded data
    /// @param tkn the bencoded dictionary
    TorrentFileInfo(const Serialization::Bencode::BeDictionary &tkn);
    /// @brief The piece length
    int64_t piece_length = DEFAULT_PIECE_LENGTH;
    /// @brief The pieces
    Serialization::Bencode::BeString pieces;
    /// @brief Uses private trackers
    int64_t isPrivate = 0;
    /// @brief The torrent name
    Serialization::Bencode::BeString name;
    /// @brief The file list or length
    std::variant<int64_t, std::vector<TorrentFileEntry>> fileListOrLength;
    /// @brief Get the torrent file size
    int64_t GetTorrentFileSize();
    /// @brief Generate the info
    /// @return the dictionary containing the info
    Serialization::Bencode::BeDictionary &GenerateInfoDictionary();
    /// @brief Get the info hash for info (if you are filling out the fields,
    /// you need to call GenerateInfoDictionary first)
    /// @return the 20 byte info hash
    Serialization::Bencode::BeString GetInfoHash();
    /// @brief Get the info (does not regenerate info from the fields)
    /// @return the dictionary containing the info
    Serialization::Bencode::BeDictionary &GetInfoDictionary();

    /// @brief Used to load a dictionary in, will overwrite all the fields
    /// @param dict the dictionary to load
    void Load(const Serialization::Bencode::BeDictionary &dict);
    /// @brief Gets a read write based on this info object
    /// @param vfs the vfs object
    /// @param path file or directory inside vfs
    /// @return The read write at object
    std::shared_ptr<ReadWriteAt> GetStreamFromFilesystem(
        std::shared_ptr<Tesses::Framework::Filesystem::VFS> vfs,
        const Tesses::Framework::Filesystem::VFSPath &path);

    /// @brief Create info from file/directory
    /// @param vfs the vfs object
    /// @param path file or directory inside vfs
    /// @param isPrivate whether you use private trackers
    /// @param pieceLength length of pieces
    void CreateFromFilesystem(
        std::shared_ptr<Tesses::Framework::Filesystem::VFS> vfs,
        const Tesses::Framework::Filesystem::VFSPath &path,
        bool isPrivate = false, int64_t pieceLength = DEFAULT_PIECE_LENGTH);
};
/**
 * @brief The torrent file
 *
 */
class TorrentFile {
  public:
    /// @brief Construct the object with empty data
    TorrentFile();
    /// @brief Construct the object with bencoded data
    /// @param tkn the bencoded dictionary
    TorrentFile(const Serialization::Bencode::BeDictionary &tkn);
    /// @brief The first tracker
    Serialization::Bencode::BeString announce;
    /// @brief The trackers
    std::vector<Serialization::Bencode::BeString> announce_list;
    /// @brief The webseed urls
    std::vector<Serialization::Bencode::BeString> url_list;
    /// @brief The comment
    Serialization::Bencode::BeString comment;
    /// @brief The creator
    Serialization::Bencode::BeString created_by;
    /// @brief When it was created
    int64_t creation_date = 0;
    /// @brief The info
    TorrentFileInfo info;
    /// @brief Convert to dictionary
    Serialization::Bencode::BeDictionary ToDictionary();

    /// @brief Print the info
    /// @param writer the text writer
    void Print(std::shared_ptr<TextStreams::TextWriter> writer);

    /// @brief Create a torrent
    /// @param torrent_file_stream the bencoded file stream to write to
    /// @param trackers the trackers you want
    /// @param webseeds the webseeds you want
    /// @param vfs the vfs you want to populate from
    /// @param path the path you want the root to be or the file
    /// @param isPrivate is for private trackers
    /// @param pieceLength the size of the pieces
    /// @param comment a comment, anything really
    /// @param created_by the tool who created it, I would keep it
    /// TessesFrameworkTorrent if I were you
    static void CreateTorrent(
        std::shared_ptr<Tesses::Framework::Streams::Stream> torrent_file_stream,
        const std::vector<Serialization::Bencode::BeString> &trackers,
        const std::vector<Serialization::Bencode::BeString> &webseeds,
        std::shared_ptr<Tesses::Framework::Filesystem::VFS> vfs,
        const Tesses::Framework::Filesystem::VFSPath &path,
        bool isPrivate = false, int64_t pieceLength = DEFAULT_PIECE_LENGTH,
        Serialization::Bencode::BeString comment = "",
        Serialization::Bencode::BeString created_by = "TessesFrameworkTorrent");
};
} // namespace Tesses::Framework::BitTorrent