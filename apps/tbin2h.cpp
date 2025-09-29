#include "TessesFramework/TessesFramework.hpp"

int main(int argc, char** argv)
{
    if(argc < 4)
    {
        std::cout <<  "USAGE: " << argv[0] << " BINARYFILE VARNAME HEADERFILE" << std::endl;
        return 0;
    }
    auto fs=std::make_shared<Tesses::Framework::Streams::FileStream>(argv[1],"rb");
    if(!fs->CanRead()) return 1;
    auto writer = std::make_shared<Tesses::Framework::TextStreams::StreamWriter>(argv[3]);
    Tesses::Framework::Text::GenerateCHeaderFile(fs, argv[2], writer);
    return 0;
}