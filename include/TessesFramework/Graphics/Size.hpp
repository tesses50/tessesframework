#pragma once
#include <cstdint>
#include <string>
namespace Tesses::Framework::Graphics {
    class Size {
        public:
            Size()
            {

            }
            Size(int32_t square)
            {
                this->Width = square;
                this->Height = square;
            }
            Size(int32_t width, int32_t height)
            {
                this->Width = width;
                this->Height = height;
            }
            int32_t Width=0;
            int32_t Height=0;
            bool IsSquare()
            {
                return Width == Height;
            }
            std::string ToString()
            {
                return std::to_string(Width) + "x" + std::to_string(Height);
            }
    };
};