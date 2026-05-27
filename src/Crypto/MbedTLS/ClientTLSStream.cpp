/*
    TessesFramework a library to make C++ easier for me, used in CrossLang:
   https://git.tesses.org/tesses50/crosslang Copyright (C) 2026 Mike Nolan

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

#include "TessesFramework/Crypto/ClientTLSStream.hpp"

#if defined(TESSESFRAMEWORK_ENABLE_MBED)
#if defined(TESSESFRAMEWORK_EMBED_CERT_BUNDLE)
#include "TessesFramework/CertificateChain.h"
#else
#include "TessesFramework/TextStreams/StreamReader.hpp"
using StreamReader = Tesses::Framework::TextStreams::StreamReader;
#endif

#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>
#include <mbedtls/error.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>
#include <mbedtls/x509.h>
#endif
#include <cstring>
using namespace Tesses::Framework::Streams;

namespace Tesses::Framework::Crypto {
#if defined(TESSESFRAMEWORK_ENABLE_MBED)
class ClientTLSPrivateData {
  public:
    bool eos;
    bool success;
    std::shared_ptr<Stream> strm;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_x509_crt cachain;
    ~ClientTLSPrivateData() {
        mbedtls_x509_crt_free(&cachain);
        mbedtls_ctr_drbg_free(&ctr_drbg);
        mbedtls_entropy_free(&entropy);
        mbedtls_ssl_config_free(&conf);
        mbedtls_ssl_free(&ssl);
    }
};
static int strm_send(void *ctx, const unsigned char *buf, size_t len) {
    auto priv = static_cast<ClientTLSPrivateData *>(ctx);
    return (int)priv->strm->Write(buf, len);
}
static int strm_recv(void *ctx, unsigned char *buf, size_t len) {
    auto priv = static_cast<ClientTLSPrivateData *>(ctx);
    return (int)priv->strm->Read(buf, len);
}
#endif

std::string ClientTLSStream::GetCertChain() {
#if defined(TESSESFRAMEWORK_ENABLE_MBED)
#if defined(TESSESFRAMEWORK_EMBED_CERT_BUNDLE)
    return std::string((const char *)CERTIFICATECHAIN, CERTIFICATECHAIN_SIZE);
#else
#if defined(TESSESFRAMEWORK_CERT_BUNDLE_FILE)
    StreamReader sr(TESSESFRAMEWORK_CERT_BUNDLE_FILE);
    return sr.ReadToEnd();
#endif
#endif
#endif
    return "";
}

ClientTLSStream::ClientTLSStream(
    std::shared_ptr<Tesses::Framework::Streams::Stream> innerStream,
    bool verify, std::string domain)
    : ClientTLSStream(innerStream, verify, domain, "") {}

ClientTLSStream::ClientTLSStream(
    std::shared_ptr<Tesses::Framework::Streams::Stream> innerStream,
    bool verify, std::string domain, std::string cert) {
#if defined(TESSESFRAMEWORK_ENABLE_MBED)
    if (cert.empty()) {
        cert = GetCertChain();
    }

    ClientTLSPrivateData *data = new ClientTLSPrivateData();
    this->privateData = static_cast<void *>(data);
    data->eos = false;
    data->success = false;
    data->strm = innerStream;

    mbedtls_ssl_init(&data->ssl);
    mbedtls_ssl_config_init(&data->conf);
    mbedtls_x509_crt_init(&data->cachain);
    mbedtls_ctr_drbg_init(&data->ctr_drbg);
    mbedtls_entropy_init(&data->entropy);

    const char *pers = "TessesFramework";

    int ret = 0;

    if ((ret = mbedtls_ctr_drbg_seed(
             &data->ctr_drbg, mbedtls_entropy_func, &data->entropy,
             (const unsigned char *)pers, strlen(pers))) != 0) {
        printf("FAILED mbedtls_ctr_drbg_seed\n");
        return;
    }

    if (ret != 0) {
        printf("FAILED mbedtls_x509_crt_parse cert %i\n", ret);
        return;
    }
    ret = mbedtls_x509_crt_parse(
        &data->cachain, (const unsigned char *)cert.c_str(), cert.size() + 1);

    if (ret != 0) {
        printf("FAILED mbedtls_x509_crt_parse chain %i\n", ret);
        return;
    }

    if ((ret = mbedtls_ssl_config_defaults(&data->conf, MBEDTLS_SSL_IS_CLIENT,
                                           MBEDTLS_SSL_TRANSPORT_STREAM,
                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        char buffer[100];
        mbedtls_strerror(ret, buffer, sizeof(buffer));
        printf("FAILED mbedtls_ssl_conf_defaults %s\n", buffer);
        return;
    }

    mbedtls_ssl_conf_rng(&data->conf, mbedtls_ctr_drbg_random, &data->ctr_drbg);

    /* #if defined(MBEDTLS_SSL_CACHE_C)
  mbedtls_ssl_conf_session_cache(&conf, &cache,
                                 mbedtls_ssl_cache_get,
                                 mbedtls_ssl_cache_set);
#endif*/
    mbedtls_ssl_conf_authmode(&data->conf, verify ? MBEDTLS_SSL_VERIFY_REQUIRED
                                                  : MBEDTLS_SSL_VERIFY_NONE);
    mbedtls_ssl_conf_ca_chain(&data->conf, &data->cachain, NULL);

    mbedtls_ssl_set_bio(&data->ssl, static_cast<void *>(data), strm_send,
                        strm_recv, NULL);
    if ((ret = mbedtls_ssl_setup(&data->ssl, &data->conf) != 0)) {
        printf("FAILED mbedtls_ssl_setup %i\n", ret);
        return;
    }
    if ((ret = mbedtls_ssl_set_hostname(&data->ssl, domain.c_str()) != 0)) {
        printf("FAILED mbedtls_ssl_set_hostname %i\n", ret);
        return;
    }
    if ((ret = mbedtls_ssl_handshake(&data->ssl)) != 0) {
        char buffer[100];
        mbedtls_strerror(ret, buffer, sizeof(buffer));
        printf("FAILED mbedtls_ssl_handshake %s\n", buffer);
        return;
    }
    uint32_t flags;
    if ((flags = mbedtls_ssl_get_verify_result(&data->ssl)) != 0) {
#if !defined(MBEDTLS_X509_REMOVE_INFO)
        char vrfy_buf[512];
#endif

#if !defined(MBEDTLS_X509_REMOVE_INFO)
        mbedtls_x509_crt_verify_info(vrfy_buf, sizeof(vrfy_buf), "  ! ", flags);

#endif
        if (verify)
            return;
    }

    data->success = true;

#endif
}
size_t ClientTLSStream::Read(uint8_t *buffer, size_t len) {
#if defined(TESSESFRAMEWORK_ENABLE_MBED)
    auto priv = static_cast<ClientTLSPrivateData *>(this->privateData);
    if (!priv->success)
        return 0;
    if (priv->eos)
        return 0;
    int r = mbedtls_ssl_read(&priv->ssl, buffer, len);

    if (r == -30848) {
        priv->eos = true;
        return 0;
    }
    return (size_t)r;
#else
    return (size_t)0;
#endif
}
size_t ClientTLSStream::Write(const uint8_t *buffer, size_t len) {
#if defined(TESSESFRAMEWORK_ENABLE_MBED)
    auto priv = static_cast<ClientTLSPrivateData *>(this->privateData);
    if (!priv->success)
        return 0;
    int r = mbedtls_ssl_write(&priv->ssl, buffer, len);
    return (size_t)r;
#else
    return (size_t)0;
#endif
}

bool ClientTLSStream::CanRead() {
#if defined(TESSESFRAMEWORK_ENABLE_MBED)
    return !(!static_cast<ClientTLSPrivateData *>(this->privateData)->success ||
             static_cast<ClientTLSPrivateData *>(this->privateData)->eos);
#else
    return false;
#endif
}
bool ClientTLSStream::CanWrite() {

#if defined(TESSESFRAMEWORK_ENABLE_MBED)
    return !(!static_cast<ClientTLSPrivateData *>(this->privateData)->success ||
             static_cast<ClientTLSPrivateData *>(this->privateData)->eos);
#else
    return false;
#endif
}
bool ClientTLSStream::EndOfStream() {
#if defined(TESSESFRAMEWORK_ENABLE_MBED)
    return !static_cast<ClientTLSPrivateData *>(this->privateData)->success ||
           static_cast<ClientTLSPrivateData *>(this->privateData)->eos;
#else
    return true;
#endif
}
ClientTLSStream::~ClientTLSStream() {
#if defined(TESSESFRAMEWORK_ENABLE_MBED)
    delete static_cast<ClientTLSPrivateData *>(this->privateData);
#endif
}
} // namespace Tesses::Framework::Crypto