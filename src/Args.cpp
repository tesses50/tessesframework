#include "TessesFramework/TessesFramework.hpp"

namespace Tesses::Framework {
    Args::Args(std::vector<std::string> args)
    {
        if(args.size() < 1) return;
        filename = args[0];
        bool onlyPos=false;

        for(size_t i = 1; i < args.size(); i++)
        {
            std::string& arg = args[i];
            if(arg == "--")
            {
                onlyPos=true;
                continue;
            }
            if(!onlyPos && arg.size() > 2 && arg[0] == '-' && arg[1] == '-')
            {
                auto p = Tesses::Framework::Http::HttpUtils::SplitString(arg.substr(2),"=",2);
                if(p.size() == 1)
                    flags.push_back(p[0]);
                else if(p.size() == 2)
                    options.push_back(std::pair<std::string,std::string>(p[0],p[1]));
            }
            else {
                positional.push_back(arg);
            }
        }

    }
    Args::Args(int argc, char** argv)
    {
        if(argc < 1) return;
        filename = argv[0];
        bool onlyPos=false;

        for(int i = 1; i < argc; i++)
        {
            std::string_view arg = argv[i];
            if(arg == "--")
            {
                onlyPos=true;
                continue;
            }
            if(!onlyPos && arg.size() > 2 && arg[0] == '-' && arg[1] == '-')
            {
                auto p = Tesses::Framework::Http::HttpUtils::SplitString((std::string)arg.substr(2),"=",2);
                if(p.size() == 1)
                    flags.push_back(p[0]);
                else if(p.size() == 2)
                    options.push_back(std::pair<std::string,std::string>(p[0],p[1]));
            }
            else {
                positional.push_back((std::string)arg);
            }
        }

    }
}