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
#include "Crypto.hpp"

namespace Tesses::Framework::Crypto {
/**
 * @brief This stream is to encrypt data for TLS (client side)
 *
 */
class ClientTLSStream : public Tesses::Framework::Streams::Stream {
    void *privateData;

  public:
    /**
     * @brief Get the Cert Chain object
     *
     * @return std::string The cert chain
     */
    static std::string GetCertChain();
    /**
     * @brief Construct a new Client TLS Stream object
     *
     * @param innerStream the underlying encrypted in transit stream
     * @param verify do we verify the certificate
     * @param domain the domain name
     */
    ClientTLSStream(
        std::shared_ptr<Tesses::Framework::Streams::Stream> innerStream,
        bool verify, std::string domain);
    /**
     * @brief Construct a new Client TLS Stream object with an alternative
     * certificate chain (for server with self signed certificates)
     *
     * @param innerStream the underlying encrypted in transit stream
     * @param verify do we verify the certificate
     * @param domain the domain name
     * @param cert the actual certificate
     */
    ClientTLSStream(
        std::shared_ptr<Tesses::Framework::Streams::Stream> innerStream,
        bool verify, std::string domain, std::string cert);
    /**
     * @brief Construct a new Client TLS Stream object, with mTLS
     *
     * @param innerStream the underlying encrypted in transit stream
     * @param verify do we verify the certificate
     * @param domain the domain name
     * @param keyStore the keystore for mTLS
     */
    ClientTLSStream(
        std::shared_ptr<Tesses::Framework::Streams::Stream> innerStream,
        bool verify, std::string domain, CertificateKeyStore keyStore);
    /**
     * @brief Construct a new Client TLS Stream object with an alternative
     * certificate chain (for server with self signed certificates) for mTLS
     *
     * @param innerStream the underlying encrypted in transit stream
     * @param verify do we verify the certificate
     * @param domain the domain name
     * @param cert the actual certificate
     * @param keyStore the keystore for mTLS
     */
    ClientTLSStream(
        std::shared_ptr<Tesses::Framework::Streams::Stream> innerStream,
        bool verify, std::string domain, std::string cert,
        CertificateKeyStore keyStore);

    /**
     * @brief Read from the stream
     *
     * @param buff the buffer
     * @param sz the size of the buffer
     * @return size_t the bytes read
     */
    size_t Read(uint8_t *buff, size_t sz);
    /**
     * @brief Write to the stream
     *
     * @param buff the buffer
     * @param sz the size of the buffer
     * @return size_t the bytes writen
     */
    size_t Write(const uint8_t *buff, size_t sz);
    /**
     * @brief Can I read from the stream
     *
     * @return true Yes I can
     * @return false No I can't
     */
    bool CanRead();
    /**
     * @brief Can I write to the stream
     *
     * @return true Yes I can
     * @return false No I can't
     */
    bool CanWrite();
    /**
     * @brief Is it the end of the stream
     *
     * @return true yes
     * @return false no
     */
    bool EndOfStream();

    void Shutdown(Tesses::Framework::Streams::StreamShutdownMode sdm);
    void SetSendTimeout(uint64_t seconds);
    void SetRecvTimeout(uint64_t seconds);
    ~ClientTLSStream();
};

} // namespace Tesses::Framework::Crypto