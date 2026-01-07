#pragma once
#include <cstdint>
#include <vector>
#include <variant>
#include <string>
#include "../Streams/Stream.hpp"
#include "../TextStreams/TextWriter.hpp"
#include "Json.hpp"
namespace Tesses::Framework::Serialization::Bencode {
    class BeArray;
    class BeDictionary;
    class BeString;
    using BeUndefined = std::monostate;

    using BeToken = std::variant<BeUndefined,BeArray,BeDictionary,BeString,int64_t>;

    class BeArray {
        public:
            std::vector<BeToken> tokens;
    };

    class BeDictionary {
        public:
            std::vector<std::pair<BeString,BeToken>> tokens;
            BeToken GetValue(BeString key) const;
            void SetValue(BeString key, BeToken value);
    };
    class BeString {
        public:
            BeString();
            BeString(const std::string& text);
            BeString(const char* text);
            BeString(const std::vector<uint8_t>& data);
            std::vector<uint8_t> data;

            operator std::string() const;
    };
    bool operator==(const BeString& lStr, const BeString& rStr);
    bool operator==(const BeString& lStr, const std::string& rStr);
    bool operator==(const std::string& lStr, const BeString& rStr);
    bool operator==(const BeString& lStr, const char* rStr);
    bool operator==(const char* lStr, const BeString& rStr);
    
    bool operator!=(const BeString& lStr, const BeString& rStr);
    bool operator!=(const BeString& lStr, const std::string& rStr);
    bool operator!=(const std::string& lStr, const BeString& rStr);
    bool operator!=(const BeString& lStr, const char* rStr);
    bool operator!=(const char* lStr, const BeString& rStr);
    
    class Bencode {
        public:           
            static void Save(std::shared_ptr<Tesses::Framework::Streams::Stream> strm,const BeToken& value);
            static BeToken Load(std::shared_ptr<Tesses::Framework::Streams::Stream> strm);
            //This cannot be converted back to torrent, this may (probably will) be out of order
            static Json::JToken ToJson(const BeToken& tkn);
            //This may (probably will) be out of order
            static void Print(std::shared_ptr<Tesses::Framework::TextStreams::TextWriter> writer, BeToken tkn);
    };
}