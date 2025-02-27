#pragma once

#include "Renderer.hpp"

namespace Tesses::Framework::Graphics::Renderers {


class ImageRenderer : public Renderer {
    Image* image;
    public:
        ImageRenderer(Image* image);
        uint32_t Width();
        uint32_t Height();
        void SetPixel(Point pt,Color c);
        
};
};