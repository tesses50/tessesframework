

#include "TessesFramework/Graphics/Renderers/ImageRenderer.hpp"

namespace Tesses::Framework::Graphics::Renderers {


    ImageRenderer::ImageRenderer(Image* image)
    {
        this->image = image;
    }
    uint32_t ImageRenderer::Width()
    {
        return this->image->Width();
    }
    uint32_t ImageRenderer::Height()
    {
        return this->image->Height();
    }
    void ImageRenderer::SetPixel(Point pt,Color c)
    {
        this->image->SetPixel(pt.X,pt.Y,c);
    }
};