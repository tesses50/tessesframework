#include "TessesFramework/Crypto/MbedHelpers.hpp"

#if defined(TESSESFRAMEWORK_ENABLE_MBED)
#include <mbedtls/sha1.h>
#include <mbedtls/sha256.h>
#include <mbedtls/sha512.h>
#include <mbedtls/base64.h>
#endif
namespace Tesses::Framework::Crypto
{
    bool HaveCrypto()
    {
        #if defined(TESSESFRAMEWORK_ENABLE_MBED)
        return true;
        #else
        return false;
        #endif
    }
    std::string Base64_Encode(std::vector<uint8_t> data)
    {
        #if defined(TESSESFRAMEWORK_ENABLE_MBED)
        std::string str={};
        size_t olen=0;
        mbedtls_base64_encode((uint8_t*)str.data(), 0, &olen, data.data(),data.size());
        str.resize(olen-1);
                

        if(mbedtls_base64_encode((uint8_t*)str.data(), olen, &olen, data.data(),data.size())==0)
        {   
            return str;
        }

        #endif
        return "";
    }
    std::vector<uint8_t> Base64_Decode(std::string str)
    {
        #if defined(TESSESFRAMEWORK_ENABLE_MBED)
         size_t olen;

         std::vector<uint8_t> data;
        

        mbedtls_base64_decode(data.data(), 0, &olen, (const uint8_t*)str.data(),str.size());
        data.resize(olen);
                

        if(mbedtls_base64_decode(data.data(), olen, &olen, (const uint8_t*)str.data(),str.size())==0)
        {   
                    return data;
        }
        #endif
        return {};
    }
    Sha1::Sha1()
    {
        #if defined(TESSESFRAMEWORK_ENABLE_MBED)
            mbedtls_sha1_context* ctx = new mbedtls_sha1_context();
            this->inner = ctx;
            mbedtls_sha1_init(ctx);
            
        #endif
    }
    bool Sha1::Start()
    {
        #if defined(TESSESFRAMEWORK_ENABLE_MBED)
        auto ctx = static_cast<mbedtls_sha1_context*>(this->inner);
        if(mbedtls_sha1_starts_ret(ctx) != 0) return false;
        return true;
        #endif
        return false;
    }
    bool Sha1::Update(const uint8_t* buffer, size_t sz)
    {
        #if defined(TESSESFRAMEWORK_ENABLE_MBED)
        auto ctx = static_cast<mbedtls_sha1_context*>(this->inner);
        if(mbedtls_sha1_update_ret(ctx,buffer,sz) != 0) return false;
        return true;
        #endif
        return false;
    }
    bool Sha1::Update(Tesses::Framework::Streams::Stream* strm)
    {
        #if defined(TESSESFRAMEWORK_ENABLE_MBED)
        if(strm == nullptr) return false;
        uint8_t buffer[1024];
        size_t read;
        do {
            read = strm->Read(buffer,sizeof(buffer));
            if(!Update(buffer,read)) return false;
        } while(read != 0);
        return true;
        #endif
        return false;
    }
    bool Sha1::Update(Tesses::Framework::Streams::Stream& strm)
    {
        return Update(&strm);
    }
    std::vector<uint8_t> Sha1::Finish()
    {
        #if defined(TESSESFRAMEWORK_ENABLE_MBED)
        auto ctx = static_cast<mbedtls_sha1_context*>(this->inner);
        std::vector<uint8_t> hash;
        hash.resize(20);
        if(mbedtls_sha1_finish_ret(ctx,hash.data()) != 0) return {};
        return hash;
        #endif
        return {};
    }
    Sha1::~Sha1()
    {
        #if defined(TESSESFRAMEWORK_ENABLE_MBED)
        auto ctx = static_cast<mbedtls_sha1_context*>(this->inner);
        mbedtls_sha1_free(ctx);
        delete ctx;
        #endif
    }
    std::vector<uint8_t> Sha1::ComputeHash(const uint8_t* buffer, size_t len)
    {
        Sha1 sha1;
        if(!sha1.Start()) return {};
        if(!sha1.Update(buffer,len)) return {};
        return sha1.Finish();
    }
    std::vector<uint8_t> Sha1::ComputeHash(Tesses::Framework::Streams::Stream* strm)
    {
        Sha1 sha1;
        if(!sha1.Start()) return {};
        if(!sha1.Update(strm)) return {};
        return sha1.Finish();
    }
    std::vector<uint8_t> Sha1::ComputeHash(Tesses::Framework::Streams::Stream& strm)
    {
        Sha1 sha1;
        if(!sha1.Start()) return {};
        if(!sha1.Update(strm)) return {};
        return sha1.Finish();
    }

    Sha256::Sha256()
    {
        #if defined(TESSESFRAMEWORK_ENABLE_MBED)
            mbedtls_sha256_context* ctx = new mbedtls_sha256_context();
            this->inner = ctx;
            mbedtls_sha256_init(ctx);
            
        #endif
    }
    bool Sha256::Start(bool is224)
    {
        #if defined(TESSESFRAMEWORK_ENABLE_MBED)
        auto ctx = static_cast<mbedtls_sha256_context*>(this->inner);
        this->is224=is224;
        if(mbedtls_sha256_starts_ret(ctx,is224) != 0) return false;
        return true;
        #endif
        return false;
    }
    bool Sha256::Is224()
    {
        return this->is224;
    }
    bool Sha256::Update(const uint8_t* buffer, size_t sz)
    {

        #if defined(TESSESFRAMEWORK_ENABLE_MBED)
        auto ctx = static_cast<mbedtls_sha256_context*>(this->inner);
        if(mbedtls_sha256_update_ret(ctx,buffer,sz) != 0) return false;
        return true;
        #endif
        return false;
    }
    bool Sha256::Update(Tesses::Framework::Streams::Stream* strm)
    {
        #if defined(TESSESFRAMEWORK_ENABLE_MBED)
        if(strm == nullptr) return false;
        uint8_t buffer[1024];
        size_t read;
        do {
            read = strm->Read(buffer,sizeof(buffer));
            if(!Update(buffer,read)) return false;
        } while(read != 0);
        return true;
        #endif
        return false;
    }
    bool Sha256::Update(Tesses::Framework::Streams::Stream& strm)
    {
        return Update(&strm);
    }
    std::vector<uint8_t> Sha256::Finish()
    {
        #if defined(TESSESFRAMEWORK_ENABLE_MBED)
        auto ctx = static_cast<mbedtls_sha256_context*>(this->inner);
        std::vector<uint8_t> hash;
        hash.resize(32);
        if(mbedtls_sha256_finish_ret(ctx,hash.data()) != 0) return {};
        return hash;
        #endif
        return {};
    }
    Sha256::~Sha256()
    {
        #if defined(TESSESFRAMEWORK_ENABLE_MBED)
        auto ctx = static_cast<mbedtls_sha256_context*>(this->inner);
        mbedtls_sha256_free(ctx);
        delete ctx;
        #endif
    }
    std::vector<uint8_t> Sha256::ComputeHash(const uint8_t* buffer, size_t len,bool is224)
    {
        Sha256 sha256;
        if(!sha256.Start(is224)) return {};
        if(!sha256.Update(buffer,len)) return {};
        return sha256.Finish();
    }
    std::vector<uint8_t> Sha256::ComputeHash(Tesses::Framework::Streams::Stream* strm,bool is224)
    {
        Sha256 sha256;
        if(!sha256.Start(is224)) return {};
        if(!sha256.Update(strm)) return {};
        return sha256.Finish();
    }
    std::vector<uint8_t> Sha256::ComputeHash(Tesses::Framework::Streams::Stream& strm,bool is224)
    {
        Sha256 sha256;
        if(!sha256.Start(is224)) return {};
        if(!sha256.Update(strm)) return {};
        return sha256.Finish();
    }
    
    Sha512::Sha512()
    {
        #if defined(TESSESFRAMEWORK_ENABLE_MBED)
            mbedtls_sha512_context* ctx = new mbedtls_sha512_context();
            this->inner = ctx;
            mbedtls_sha512_init(ctx);
        #endif
    }
    bool Sha512::Start(bool is384)
    {
        #if defined(TESSESFRAMEWORK_ENABLE_MBED)
        auto ctx = static_cast<mbedtls_sha512_context*>(this->inner);
        this->is384=is384;
        if(mbedtls_sha512_starts_ret(ctx,is384) != 0) return false;
        return true;
        #endif
        return false;
    }
    bool Sha512::Is384()
    {
        return this->is384;
    }
    bool Sha512::Update(const uint8_t* buffer, size_t sz)
    {
        #if defined(TESSESFRAMEWORK_ENABLE_MBED)
        auto ctx = static_cast<mbedtls_sha512_context*>(this->inner);
        if(mbedtls_sha512_update_ret(ctx,buffer,sz) != 0) return false;
        return true;
        #endif
        return false;
    }
    bool Sha512::Update(Tesses::Framework::Streams::Stream* strm)
    {
        #if defined(TESSESFRAMEWORK_ENABLE_MBED)
        if(strm == nullptr) return false;
        uint8_t buffer[1024];
        size_t read;
        do {
            read = strm->Read(buffer,sizeof(buffer));
            if(!Update(buffer,read)) return false;
        } while(read != 0);
        return true;
        #endif
        return false;
    }
    bool Sha512::Update(Tesses::Framework::Streams::Stream& strm)
    {
        return Update(&strm);
    }
    std::vector<uint8_t> Sha512::Finish()
    {
        #if defined(TESSESFRAMEWORK_ENABLE_MBED)
        auto ctx = static_cast<mbedtls_sha512_context*>(this->inner);
        std::vector<uint8_t> hash;
        hash.resize(64);
        if(mbedtls_sha512_finish_ret(ctx,hash.data()) != 0) return {};
        return hash;
        #endif
        return {};
    }
    Sha512::~Sha512()
    {
        #if defined(TESSESFRAMEWORK_ENABLE_MBED)
        auto ctx = static_cast<mbedtls_sha512_context*>(this->inner);
        mbedtls_sha512_free(ctx);
        delete ctx;
        #endif
    }

    std::vector<uint8_t> Sha512::ComputeHash(const uint8_t* buffer, size_t len,bool is384)
    {
        Sha512 sha512;
        if(!sha512.Start(is384)) return {};
        if(!sha512.Update(buffer,len)) return {};
        return sha512.Finish();
    }
    std::vector<uint8_t> Sha512::ComputeHash(Tesses::Framework::Streams::Stream* strm,bool is384)
    {
        Sha512 sha512;
        if(!sha512.Start(is384)) return {};
        if(!sha512.Update(strm)) return {};
        return sha512.Finish();
    }
    std::vector<uint8_t> Sha512::ComputeHash(Tesses::Framework::Streams::Stream& strm,bool is384)
    {
        Sha512 sha512;
        if(!sha512.Start(is384)) return {};
        if(!sha512.Update(strm)) return {};
        return sha512.Finish();
    }
}
