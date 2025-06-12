#include "TessesFramework/Http/ContentDisposition.hpp"
#include "TessesFramework/Http/HttpUtils.hpp"
#include <iostream>
namespace Tesses::Framework::Http
{
    bool ContentDisposition::TryParse(std::string str, ContentDisposition& cd)
    {
        auto res = HttpUtils::SplitString(str,"; ");
        

        if(res.empty()) return false;
        cd.type = res[0];
        bool hasFileNameStar = false;
        for(size_t i = 1; i < res.size(); i++)
        {
            auto res2 = HttpUtils::SplitString(res[i],"=",2);
            if(res2.size() == 2)
            {
                if(res2[0] == "filename*")
                {
                    //cd.filename = res2[1];
                    //UTF-8''
                    std::string p = res2[1];
                    if(p.find("UTF-8''") == 0)
                    {
                        hasFileNameStar = true;
                        p = HttpUtils::UrlPathDecode(p.substr(7));
                        cd.filename = p;
                    }
                }
                else if(res2[0] == "filename" && !hasFileNameStar)
                {
                    std::string p = res2[1];
                    if(!p.empty() && p[0] == '\"')
                    {
                        p = p.substr(1, p.size()-2);
                    }


                    p = HttpUtils::UrlPathDecode(p);

                    

                    cd.filename = p;
                }
                else if(res2[0] == "name")
                {
                       std::string p = res2[1];
                    if(!p.empty() && p[0] == '\"')
                    {
                        p = p.substr(1, p.size()-2);
                    }

                    cd.fieldName = HttpUtils::UrlPathDecode(p);
                 
                }
            }
        }
        return true;
    }
    

    std::string ContentDisposition::ToString()
    {
        std::vector<std::string> parts;

        parts.push_back(this->type);

        if(!this->fieldName.empty())
        {
            parts.push_back("name=\"" + HttpUtils::UrlPathEncode(this->fieldName,true) +  "\"");
        }

        if(!this->filename.empty())
        {
            parts.push_back("filename=\"" + HttpUtils::UrlPathEncode(this->filename,true) +  "\"");
        }

        return HttpUtils::Join("; ", parts);
    }
}