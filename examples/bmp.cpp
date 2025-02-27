#include "TessesFramework/TessesFramework.hpp"
#include <cstdlib>

using namespace Tesses::Framework;
using namespace Tesses::Framework::Graphics;
using namespace Tesses::Framework::Graphics::ImageFormats;
using namespace Tesses::Framework::Streams;

void doIt(const char* n)
{
    std::string p = n;
    srand(std::stoi(p));
    p.append(".bmp");
    FileStream* strm = new FileStream(p,"wb");
    Image image(320,240);

    std::vector<Color> colors = {
        Colors::Crimson,
        Colors::Chartreuse,
        Colors::FireBrick,
        Colors::Turquoise,
        Colors::DarkOrange,
        Colors::Orange
    };



    for(uint32_t y = 0; y < image.Height(); y++)
    {
        for(uint32_t x = 0; x < image.Width(); x++)
        {
            image.SetPixel(x,y,colors[rand() % (int)colors.size()]);
        }
    }

    ImageFormats::Bitmap bmp;
    bmp.Save(strm,&image);
    delete strm;
}

int main(int argc, char** argv)
{
   for(int i = 1; i < argc; i++)
   {
        doIt(argv[i]);
   }
}