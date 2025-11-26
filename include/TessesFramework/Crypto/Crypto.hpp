#pragma once
#include "../Streams/Stream.hpp"
namespace Tesses::Framework::Crypto
{
    bool HaveCrypto();
    std::string Base64_Encode(std::vector<uint8_t> data);
    std::vector<uint8_t> Base64_Decode(std::string str);
    class Sha1 {
        void* inner;
        public:
            Sha1();
            bool Start();
            bool Update(const uint8_t* buffer, size_t sz);
            bool Update(std::shared_ptr<Tesses::Framework::Streams::Stream> strm);
           std::vector<uint8_t> Finish();
            ~Sha1();
            static std::vector<uint8_t> ComputeHash(const uint8_t* buffer, size_t len);
            static std::vector<uint8_t> ComputeHash(std::shared_ptr<Tesses::Framework::Streams::Stream> strm);
    };
    class Sha256 {
        void* inner;
        bool is224;
        public:
            Sha256();
            bool Start(bool is224=false);
            bool Is224();
            bool Update(const uint8_t* buffer, size_t sz);
            bool Update(std::shared_ptr<Tesses::Framework::Streams::Stream> strm);
            std::vector<uint8_t> Finish();
            ~Sha256();
            static std::vector<uint8_t> ComputeHash(const uint8_t* buffer, size_t len,bool is224=false);
            static std::vector<uint8_t> ComputeHash(std::shared_ptr<Tesses::Framework::Streams::Stream> strm,bool is224=false);
    
    };
    class Sha512 {
        void* inner;
        bool is384;
        public:
            Sha512();
            bool Start(bool is384=false);
            bool Is384();
            bool Update(const uint8_t* buffer, size_t sz);
            bool Update(std::shared_ptr<Tesses::Framework::Streams::Stream> strm);
             std::vector<uint8_t> Finish();
            ~Sha512();

            static std::vector<uint8_t> ComputeHash(const uint8_t* buffer, size_t len,bool is384=false);
            static std::vector<uint8_t> ComputeHash(std::shared_ptr<Tesses::Framework::Streams::Stream> strm,bool is384=false);
    };
    typedef enum  {
        VERSION_SHA1=1,
        VERSION_SHA224=224,
        VERSION_SHA256=256,
        VERSION_SHA384=384,
        VERSION_SHA512=512
    } ShaVersion;
    bool PBKDF2(std::vector<uint8_t>& output,std::string pass, std::vector<uint8_t>& salt, long itterations, ShaVersion version);

    bool RandomBytes(std::vector<uint8_t>& output, std::string personal_str);
}