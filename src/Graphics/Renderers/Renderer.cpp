

#include "TessesFramework/Graphics/Renderers/Renderer.hpp"
namespace Tesses::Framework::Graphics::Renderers {

    void Renderer::DrawImage(Point pt, Image* image)
    {
        auto width = std::min(image->Width(),Width()-  pt.X);
        auto height = std::min(image->Height(),Height()-pt.Y);
        for(uint32_t y = 0; y < height; y++)
        {
            for(uint32_t x = 0; x < width; x++)
            {
                this->SetPixel(Point(x+pt.X,y+pt.Y),image->GetPixel(x,y));
            }
        }
    }
    void Renderer::DrawRectangle(Rectangle rect, Color c, bool fill)
    {

        int32_t xMin = rect.Location.X;
        int32_t xMax = (rect.Size.Width + xMin)-1;
        int64_t yMin = rect.Location.Y;
        int64_t yMax = (rect.Size.Height + yMin)-1;
        if(fill)
        {
            for(int32_t y = yMin; y <= yMax;y++)
            {
                for(int32_t x = xMin; x <=xMax;x++)
                {
                    SetPixel(Point(x,y),c);
                }
            }
        }
        else
        {
            for(int32_t x = xMin; x <=xMax;x++)
            {
                SetPixel(Point(x,yMin),c);
                SetPixel(Point(x,yMax),c);
            }
            for(int32_t y = yMin; y <= yMax;y++)
            {
                SetPixel(Point(xMin,y),c);
                SetPixel(Point(xMax,y),c);
            }
        }
    }
    Renderer::~Renderer()
    {

    }
};