#pragma once
#include "Color.hpp"
#include <vector>
namespace Tesses::Framework::Graphics {
    class Image {
        uint32_t w=0;
        uint32_t h=0;
        std::vector<Color> data={};
        public:
            Image();
            Image(uint32_t w, uint32_t h);
            Image(uint32_t w,uint32_t h,std::vector<Color> data);
            uint32_t Width();
            uint32_t Height();
            void SetSize(uint32_t w, uint32_t h);
            void SetSize(uint32_t w, uint32_t h, Color c);
            void SetPixel(uint32_t x, uint32_t y, Color c);
            Color GetPixel(uint32_t x, uint32_t y);
            
            std::vector<Color>& Data();

            
    };  
}