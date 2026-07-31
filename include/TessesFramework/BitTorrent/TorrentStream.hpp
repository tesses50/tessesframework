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

#include "TessesFramework/Http/HttpClient.hpp"
#include "TessesFramework/Streams/NetworkStream.hpp"
namespace Tesses::Framework::BitTorrent {
/// @brief The file entry in the torrent
class TorrentFileEntry {
  public:
    /// @brief Construct the object with empty values
    TorrentFileEntry();
    /// @brief Construct the object with a path and a length
    /// @param path the path in the torrent
    /// @param length the length of the file in bytes
    TorrentFileEntry(Tesses::Framework::Filesystem::VFSPath path,
                     int64_t length);
    /// @brief The path in the torrent
    Tesses::Framework::Filesystem::VFSPath path;
    /// @brief The length of the file in bytes
    int64_t length = 0;
};
/// @brief an abstract stream with explicit offset
class ReadWriteAt {
  public:
    /// @brief Read a block at offset
    /// @param offset the offset in the stream
    /// @param data the pointer to a buffer
    /// @param len the length of the buffer
    /// @return Did it succeed
    virtual bool ReadBlockAt(uint64_t offset, uint8_t *data, size_t len) = 0;
    /// @brief Write a block at offset
    /// @param offset the offset in the stream
    /// @param data the pointer to a buffer
    /// @param len the length of the buffer
    virtual void WriteBlockAt(uint64_t offset, const uint8_t *data,
                              size_t len) = 0;
    virtual ~ReadWriteAt();
};
/// @brief A concrette stream for the file
class TorrentFileStream : public ReadWriteAt {
    Tesses::Framework::Threading::Mutex mtx;

  public:
    /// @brief The vfs
    std::shared_ptr<Tesses::Framework::Filesystem::VFS> vfs;
    /// @brief The path
    Tesses::Framework::Filesystem::VFSPath path;
    /// @brief the length
    uint64_t length;
    /**
     * @brief Construct a new Torrent File Stream object
     *
     * @param vfs the filesystem
     * @param path the file (not the bencoded torrent file)
     * @param length the file length (according to the bencoded torrent file)
     */
    TorrentFileStream(std::shared_ptr<Tesses::Framework::Filesystem::VFS> vfs,
                      Tesses::Framework::Filesystem::VFSPath path,
                      uint64_t length);
    /// @brief Read a block at offset
    /// @param offset the offset in the stream
    /// @param data the pointer to a buffer
    /// @param len the length of the buffer
    /// @return Did it succeed
    bool ReadBlockAt(uint64_t offset, uint8_t *data, size_t len);
    /// @brief Write a block at offset
    /// @param offset the offset in the stream
    /// @param data the pointer to a buffer
    /// @param len the length of the buffer
    void WriteBlockAt(uint64_t offset, const uint8_t *data, size_t len);
};
/**
 * @brief A concrette stream for a torrent with multiple files
 *
 */
class TorrentDirectoryStream : public ReadWriteAt {

    std::vector<Tesses::Framework::Threading::Mutex> mtxs;

  public:
    /// @brief The filesystem
    std::shared_ptr<Tesses::Framework::Filesystem::VFS> vfs;
    /// @brief The root directory
    Tesses::Framework::Filesystem::VFSPath path;
    /// @brief The files
    std::vector<TorrentFileEntry> entries;
    /**
     * @brief Construct a new Torrent Directory Stream object
     *
     * @param vfs the filesystem
     * @param path the root directory
     * @param entries the files
     */
    TorrentDirectoryStream(
        std::shared_ptr<Tesses::Framework::Filesystem::VFS> vfs,
        Tesses::Framework::Filesystem::VFSPath path,
        std::vector<TorrentFileEntry> entries);

    /// @brief Read a block at offset
    /// @param offset the offset in the stream
    /// @param data the pointer to a buffer
    /// @param len the length of the buffer
    /// @return Did it succeed
    bool ReadBlockAt(uint64_t offset, uint8_t *data, size_t len);
    /// @brief Write a block at offset
    /// @param offset the offset in the stream
    /// @param data the pointer to a buffer
    /// @param len the length of the buffer
    void WriteBlockAt(uint64_t offset, const uint8_t *data, size_t len);
};
} // namespace Tesses::Framework::BitTorrent