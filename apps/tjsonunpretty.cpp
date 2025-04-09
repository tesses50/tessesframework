#include "TessesFramework/TessesFramework.hpp"
using namespace Tesses::Framework::Streams;
using namespace Tesses::Framework::Serialization::Json;
using namespace Tesses::Framework::TextStreams;
FileStream* OpenWrite(std::string dest)
{
    if(dest == "-")
    {
        return new FileStream(stdout,false,"w");
    }
    else
    {
        FileStream* strm = new FileStream(dest,"w");
        if(!strm->CanWrite())
        {
            delete strm;
            return nullptr;
        }
        return strm;
    }
}
FileStream* OpenRead(std::string src)
{
    if(src == "-")
    {
        return new FileStream(stdin,false,"r");
    }
    else
    {
        FileStream* strm = new FileStream(src,"r");
        if(!strm->CanRead())
        {
            delete strm;
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
        std::cout << "SRC: json file or - for stdin to make unpretty" << std::endl;
        std::cout << "DEST: unprettied file or - for stdout" << std::endl;
        return 0;
    }
    FileStream* src = OpenRead(argv[1]);

    FileStream* dest = OpenWrite(argv[2]);

    if(src == nullptr)
    {
        if(dest != nullptr) delete dest;
        std::cerr << "ERROR: Input could not be read" << std::endl;
        return 1;
    }
    if(dest == nullptr)
    {
        delete src;
        std::cerr << "ERROR: Output could not be read" << std::endl;
        return 1;
    }

    StreamReader reader(src,true);
    StreamWriter writer(dest,true);

    auto str = reader.ReadToEnd();
    writer.WriteLine(Json::Encode(Json::Decode(str),false));


    return 0;
}