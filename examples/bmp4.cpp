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
    
    Image img2(img.Width()+64,img.Height()+64);
    Renderers::ImageRenderer irdr(&img2);
    irdr.DrawRectangle(Rectangle(0,0,(int32_t)img2.Width(),(int32_t)img2.Height()),Colors::Crimson,false);
    irdr.DrawRectangle(Rectangle(1,1,(int32_t)img2.Width()-2,(int32_t)img2.Height()-2),Colors::Chartreuse,true);
    irdr.DrawImage(Point(32,32),&img);
    
    FileStream* dest = new FileStream(argv[2],"wb");
    bmp.Save(dest,&img2);
    delete dest;
}