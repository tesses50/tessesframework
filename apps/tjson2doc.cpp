#include "TessesFramework/TessesFramework.hpp"
using namespace Tesses::Framework::Streams;
using namespace Tesses::Framework::Serialization::Json;
using namespace Tesses::Framework::TextStreams;
std::shared_ptr<FileStream> OpenWrite(std::string dest)
{
    if(dest == "-")
    {
        return std::make_shared<FileStream>(stdout,false,"w");
    }
    else
    {
        auto strm = std::make_shared<FileStream>(dest,"w");
        if(!strm->CanWrite())
        {
            return nullptr;
        }
        return strm;
    }
}
std::shared_ptr<FileStream> OpenRead(std::string src)
{
    if(src == "-")
    {
        return std::make_shared<FileStream>(stdin,false,"r");
    }
    else
    {
        auto strm = std::make_shared<FileStream>(src,"r");
        if(!strm->CanRead())
        {
            return nullptr;
        }
        return strm;
    }
}
int main(int argc, char** argv)
{
    if(argc < 3)
    {
        std::cout << "USAGE: " << argv[0] << " SRC DEST" << std::endl;
        std::cout << "SRC: json file or - for stdin to convert to json doc" << std::endl;
        std::cout << "DEST: json doc file or - for stdout" << std::endl;
        return 0;
    }
    auto src = OpenRead(argv[1]);

    auto dest = OpenWrite(argv[2]);

    if(src == nullptr)
    {
       
        std::cerr << "ERROR: Input could not be read" << std::endl;
        return 1;
    }
    if(dest == nullptr)
    {
        std::cerr << "ERROR: Output could not be read" << std::endl;
        return 1;
    }

    StreamReader reader(src);
    StreamWriter writer(dest);

    auto str = reader.ReadToEnd();
    auto doc = Json::Decode(str);
    JArray docLs;
    TryGetJToken(doc, docLs);
    writer.WriteLine(Json::DocEncode(docLs));


    return 0;
}