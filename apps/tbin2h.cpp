#include "TessesFramework/TessesFramework.hpp"

int main(int argc, char** argv)
{
    if(argc < 4)
    {
        std::cout <<  "USAGE: " << argv[0] << " BINARYFILE VARNAME HEADERFILE" << std::endl;
        return 0;
    }
    Tesses::Framework::Streams::FileStream fs(argv[1],"rb");
    if(!fs.CanRead()) return 1;
    Tesses::Framework::TextStreams::StreamWriter writer(argv[3]);
    Tesses::Framework::Text::GenerateCHeaderFile(&fs, argv[2], &writer);
    return 0;
}