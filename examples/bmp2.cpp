#include "TessesFramework/TessesFramework.hpp"
#include <cstdlib>

using namespace Tesses::Framework;
using namespace Tesses::Framework::Graphics;
using namespace Tesses::Framework::Graphics::ImageFormats;
using namespace Tesses::Framework::Streams;

#define WIDTH 19
#define HEIGHT 12
#define A 0x0A
#define B 0x0B
#define C 0x0C
char house[] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //1
0,0,0,0,0,0,0,0,9,9,0,0,0,7,0,0,B,B,0, //2
0,0,0,8,8,0,0,0,9,9,0,0,7,6,7,0,A,0,B, //3
0,0,0,8,8,0,0,0,0,0,0,7,C,C,C,7,A,0,0, //4
0,8,8,8,8,8,8,0,0,0,7,C,C,C,C,C,7,0,0, //5
0,8,8,8,8,8,8,0,0,7,7,7,7,7,7,7,7,7,0, //6
0,0,0,8,8,0,0,0,0,1,5,5,5,5,5,5,5,1,0, //7
0,0,0,8,8,0,0,0,0,1,6,5,3,3,3,5,6,1,0, //8
4,4,4,8,8,4,4,4,4,1,6,5,3,2,3,5,6,1,4, //9
4,4,4,8,8,4,4,4,4,1,5,5,3,2,3,5,5,1,4, //10
4,4,4,4,4,4,4,4,4,1,1,1,1,1,1,1,1,1,4, //11
4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4  //12
    
};

int main(int argc, char** argv)
{
    Color pallete[] = {
        Colors::SkyBlue,
        Colors::Grey,
        Colors::Crimson,
        Colors::Lime,
        Colors::LawnGreen,
        Colors::DarkOliveGreen,
        Colors::DimGrey,
        Colors::BurlyWood,
        Colors::White,
        Colors::Yellow,
        Colors::FireBrick,
        Colors::WhiteSmoke,
        Colors::DarkGoldenRod
    };

    Image img(WIDTH*32,HEIGHT*32);

    for(uint32_t y = 0; y < HEIGHT; y++)
    {
        for(uint32_t x = 0; x < WIDTH; x++)
        {
            char c = house[y * WIDTH + x];
            for(uint32_t y0 = 0; y0 < 32; y0++)
            {
                uint32_t y1 = y * 32 + y0;
                for(uint32_t x0 = 0; x0 < 32;x0++)
                {
                    uint32_t x1 = x * 32 + x0;
                    
                    img.SetPixel(x1,y1, pallete[c]);
                }
            }
        }
    }

    FileStream* strm = new FileStream("house.bmp","wb");
    ImageFormats::Bitmap bmp;
    bmp.Save(strm,&img);
    delete strm;
}