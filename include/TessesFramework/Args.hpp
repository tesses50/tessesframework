#pragma once
#include "Common.hpp"
namespace Tesses::Framework {
    class Args {
        public:
            Args(std::vector<std::string> args);
            Args(int argc, char** argv);
            std::string filename;
            std::vector<std::string> positional;
            std::vector<std::string> flags;
            std::vector<std::pair<std::string,std::string>> options;
    };
}