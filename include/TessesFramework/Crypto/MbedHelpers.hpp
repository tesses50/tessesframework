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
            bool Update(Tesses::Framework::Streams::Stream* strm);
            bool Update(Tesses::Framework::Streams::Stream& strm);
            std::vector<uint8_t> Finish();
            ~Sha1();
            static std::vector<uint8_t> ComputeHash(const uint8_t* buffer, size_t len);
            static std::vector<uint8_t> ComputeHash(Tesses::Framework::Streams::Stream* strm);
            static std::vector<uint8_t> ComputeHash(Tesses::Framework::Streams::Stream& strm);
    };
    class Sha256 {
        void* inner;
        bool is224;
        public:
            Sha256();
            bool Start(bool is224=false);
            bool Is224();
            bool Update(const uint8_t* buffer, size_t sz);
            bool Update(Tesses::Framework::Streams::Stream* strm);
            bool Update(Tesses::Framework::Streams::Stream& strm);
            std::vector<uint8_t> Finish();
            ~Sha256();
            static std::vector<uint8_t> ComputeHash(const uint8_t* buffer, size_t len,bool is224=false);
            static std::vector<uint8_t> ComputeHash(Tesses::Framework::Streams::Stream* strm,bool is224=false);
            static std::vector<uint8_t> ComputeHash(Tesses::Framework::Streams::Stream& strm,bool is224=false);
    
    };
    class Sha512 {
        void* inner;
        bool is384;
        public:
            Sha512();
            bool Start(bool is384=false);
            bool Is384();
            bool Update(const uint8_t* buffer, size_t sz);
            bool Update(Tesses::Framework::Streams::Stream* strm);
            bool Update(Tesses::Framework::Streams::Stream& strm);
            std::vector<uint8_t> Finish();
            ~Sha512();

            static std::vector<uint8_t> ComputeHash(const uint8_t* buffer, size_t len,bool is384=false);
            static std::vector<uint8_t> ComputeHash(Tesses::Framework::Streams::Stream* strm,bool is384=false);
            static std::vector<uint8_t> ComputeHash(Tesses::Framework::Streams::Stream& strm,bool is384=false);
    };
}