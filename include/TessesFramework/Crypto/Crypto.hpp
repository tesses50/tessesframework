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
#include "../Streams/Stream.hpp"
namespace Tesses::Framework::Crypto {
/**
 * @brief Do I have crypto
 *
 * @return true yes I do
 * @return false no I don't
 */
bool HaveCrypto();
/**
 * @brief Encode bytes into a base64 string
 *
 * @param data the bytes
 * @return std::string base64 encoded text
 */
std::string Base64_Encode(std::vector<uint8_t> data);
/**
 * @brief Decode bytes from a base64 string
 *
 * @param str base64 encoded text
 * @return std::vector<uint8_t> the bytes
 */
std::vector<uint8_t> Base64_Decode(std::string str);
/**
 * @brief Hash using SHA1 (don't use unless protocol needs it like websocket's
 * handshake, https://marc-stevens.nl/research/shattered.io/)
 *
 */
class Sha1 {
    void *inner;

  public:
    /**
     * @brief Construct a new Sha 1 object
     *
     */
    Sha1();
    /**
     * @brief Start it up
     *
     * @return true you can use it
     * @return false there was an error
     */
    bool Start();
    /**
     * @brief Update the hash, by writing more bytes
     *
     * @param buffer the buffer
     * @param sz the size of the buffer
     * @return true you are ok
     * @return false there was an error
     */
    bool Update(const uint8_t *buffer, size_t sz);
    /**
     * @brief Update the hash, by using a stream, but I would say use the static
     * ComputeHash then
     *
     * @param strm the stream
     * @return true you are ok
     * @return false there was an error
     */
    bool Update(std::shared_ptr<Tesses::Framework::Streams::Stream> strm);
    /**
     * @brief Finish the hashing process
     *
     * @return std::vector<uint8_t> the hash bytes (binary, not text)
     */
    std::vector<uint8_t> Finish();
    ~Sha1();
    /**
     * @brief Convience function to hash an entire buffer
     *
     * @param buffer the buffer
     * @param len the size of the buffer
     * @return std::vector<uint8_t> the hash bytes (binary, not text)
     */
    static std::vector<uint8_t> ComputeHash(const uint8_t *buffer, size_t len);
    /**
     * @brief Convience function to hash a stream
     *
     * @param strm the stream
     * @return std::vector<uint8_t> the hash bytes (binary, not text)
     */
    static std::vector<uint8_t>
    ComputeHash(std::shared_ptr<Tesses::Framework::Streams::Stream> strm);
};
/**
 * @brief Hash using SHA256 or SHA224
 *
 */
class Sha256 {
    void *inner;
    bool is224;

  public:
    /**
     * @brief Construct a new Sha 256 object
     *
     */
    Sha256();
    /**
     * @brief
     *
     * @param is224 if true, use SHA224 (which is resiliant to length extension
     * attacks) instead of SHA256 (which is not)
     * @return true you can use it
     * @return false there was an error
     */
    bool Start(bool is224 = false);
    /**
     * @brief Do we use SHA224
     *
     * @return true we use SHA224
     * @return false we use SHA256
     */
    bool Is224();
    /**
     * @brief Update the hash, by writing more bytes
     *
     * @param buffer the buffer
     * @param sz the size of the buffer
     * @return true you are ok
     * @return false there was an error
     */
    bool Update(const uint8_t *buffer, size_t sz);
    /**
     * @brief Update the hash, by using a stream, but I would say use the static
     * ComputeHash then
     *
     * @param strm the stream
     * @return true you are ok
     * @return false there was an error
     */
    bool Update(std::shared_ptr<Tesses::Framework::Streams::Stream> strm);
    /**
     * @brief Finish the hashing process
     *
     * @return std::vector<uint8_t> the hash bytes (binary, not text)
     */
    std::vector<uint8_t> Finish();
    /**
     * @brief Destroy the Sha 256 object
     *
     */
    ~Sha256();
    /**
     * @brief Convience function to hash an entire buffer
     *
     * @param buffer the buffer
     * @param len the size of the buffer
     * @param is224 if true, use SHA224 (which is resiliant to length extension
     * attacks) instead of SHA256 (which is not)
     * @return std::vector<uint8_t> the hash bytes (binary, not text)
     */
    static std::vector<uint8_t> ComputeHash(const uint8_t *buffer, size_t len,
                                            bool is224 = false);
    /**
     * @brief Convience function to hash a stream
     *
     * @param strm the stream
     * @param is224 if true, use SHA224 (which is resiliant to length extension
     * attacks) instead of SHA256 (which is not)
     * @return std::vector<uint8_t> the hash bytes (binary, not text)
     */
    static std::vector<uint8_t>
    ComputeHash(std::shared_ptr<Tesses::Framework::Streams::Stream> strm,
                bool is224 = false);
};
/**
 * @brief Hash using SHA512 or SHA384
 *
 */
class Sha512 {
    void *inner;
    bool is384;

  public:
    /**
     * @brief Construct a new Sha 512 object
     *
     */
    Sha512();
    /**
     * @brief
     *
     * @param is384 if true, use SHA384 (which is resiliant to length extension
     * attacks) instead of SHA512 (which is not)
     * @return true you can use it
     * @return false there was an error
     */
    bool Start(bool is384 = false);
    /**
     * @brief Do we use SHA384
     *
     * @return true we use SHA384
     * @return false we use SHA512
     */
    bool Is384();
    /**
     * @brief Update the hash, by writing more bytes
     *
     * @param buffer the buffer
     * @param sz the size of the buffer
     * @return true you are ok
     * @return false there was an error
     */
    bool Update(const uint8_t *buffer, size_t sz);
    /**
     * @brief Update the hash, by using a stream, but I would say use the static
     * ComputeHash then
     *
     * @param strm the stream
     * @return true you are ok
     * @return false there was an error
     */
    bool Update(std::shared_ptr<Tesses::Framework::Streams::Stream> strm);
    /**
     * @brief Finish the hashing process
     *
     * @return std::vector<uint8_t> the hash bytes (binary, not text)
     */
    std::vector<uint8_t> Finish();
    /**
     * @brief Destroy the Sha 512 object
     *
     */
    ~Sha512();
    /**
     * @brief Convience function to hash an entire buffer
     *
     * @param buffer the buffer
     * @param len the size of the buffer
     * @param is384 if true, use SHA384 (which is resiliant to length extension
     * attacks) instead of SHA512 (which is not)
     * @return std::vector<uint8_t> the hash bytes (binary, not text)
     */
    static std::vector<uint8_t> ComputeHash(const uint8_t *buffer, size_t len,
                                            bool is384 = false);
    /**
     * @brief Convience function to hash a stream
     *
     * @param strm the stream
     * @param is384 if true, use SHA384 (which is resiliant to length extension
     * attacks) instead of SHA512 (which is not)
     * @return std::vector<uint8_t> the hash bytes (binary, not text)
     */
    static std::vector<uint8_t>
    ComputeHash(std::shared_ptr<Tesses::Framework::Streams::Stream> strm,
                bool is384 = false);
};
/**
 * @brief The sha version struct for PBKDF2 and friends
 *
 */
typedef enum {
    /**
     * @brief Use sha1, please don't
     * (https://marc-stevens.nl/research/shattered.io/) (I don't even know why I
     * have this here)
     *
     */
    VERSION_SHA1 = 1,
    /**
     * @brief Use SHA224
     *
     */
    VERSION_SHA224 = 224,
    /**
     * @brief Use SHA256 (vulnerable to length extension attacks)
     *
     */
    VERSION_SHA256 = 256,
    /**
     * @brief Use SHA384
     *
     */
    VERSION_SHA384 = 384,
    /**
     * @brief Use SHA512 (vulnerable to length extension attacks)
     *
     */
    VERSION_SHA512 = 512
} ShaVersion;
/**
 * @brief
 *
 * @param output the output, which is safe for being in your database
 * @param pass the password
 * @param salt the salt (what is safe to be in your db)
 * @param itterations how many itterations
 * @param version the sha version
 * @return true
 * @return false
 */
bool PBKDF2(std::vector<uint8_t> &output, std::string pass,
            std::vector<uint8_t> &salt, long itterations, ShaVersion version);

bool RandomBytes(std::vector<uint8_t> &output, std::string personal_str);
} // namespace Tesses::Framework::Crypto