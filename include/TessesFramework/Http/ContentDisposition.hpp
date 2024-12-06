#pragma once
#include "../Common.hpp"
namespace Tesses::Framework::Http
{
    class ContentDisposition {
        public:
        std::string filename;
        std::string type;
        std::string fieldName;
        static bool TryParse(std::string str, ContentDisposition& cd);
        std::string ToString();
    };
}   