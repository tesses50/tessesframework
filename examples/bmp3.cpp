#include "TessesFramework/TessesFramework.hpp"
#include <iostream>
using namespace Tesses::Framework;
using namespace Tesses::Framework::Graphics;
using namespace Tesses::Framework::Graphics::ImageFormats;
using namespace Tesses::Framework::Streams;

int main(int argc,char** argv)
{
    if(argc < 3) {
        std::cout << "INBMP OUTBMP" << std::endl;
        return 1;
    }
    FileStream* src = new FileStream(argv[1],"rb");

    Image img;
    Bitmap bmp;
    bmp.Load(src,&img);
    delete src;
    FileStream* dest = new FileStream(argv[2],"wb");
    bmp.Save(dest,&img);
    delete dest;
}