#pragma once

#include "../Image.hpp"
#include "../Rectangle.hpp"
namespace Tesses::Framework::Graphics::Renderers {


class Renderer {
    public:
        virtual uint32_t Width()=0;
        virtual uint32_t Height()=0;
        virtual void SetPixel(Point pt,Color c) = 0;
        virtual void DrawRectangle(Rectangle rect,Color c,bool fill);
        virtual void DrawImage(Point pt, Image* image);
        virtual ~Renderer();
};
};