#include "TessesFramework/Graphics/Image.hpp"

namespace Tesses::Framework::Graphics {

        Image::Image()
        {
            
        }
        Image::Image(uint32_t w, uint32_t h)
        {
            this->w = w;
            this->h = h;
            SetSize(w,h);
        }
        Image::Image(uint32_t w,uint32_t h,std::vector<Color> data)
        {
            this->w = w;
            this->h = h;
            this->data = data;
            if(this->data.size() != w * h) throw TextException("data size does not match the factor of width and height");
        }
        uint32_t Image::Width()
        {
            return this->w;
        }
        uint32_t Image::Height()
        {
            return this->h;
        }
        void Image::SetSize(uint32_t w, uint32_t h)
        {
            this->w = w;
            this->h = h;
            this->data.resize(w * h);
        }
        void Image::SetSize(uint32_t w, uint32_t h, Color c)
        {
            this->w = w;
            this->h = h;
            this->data.resize(w * h);
            for(size_t i = 0; i < this->data.size(); i++) this->data[i] = c;
        }
        void Image::SetPixel(uint32_t x, uint32_t y, Color c)
        {
            this->data[y*w+x] = c;
        }
        Color Image::GetPixel(uint32_t x, uint32_t y)
        {
            return this->data[y*w+x];
        }
            
        std::vector<Color>& Image::Data()
        {
            return this->data;
        }
}