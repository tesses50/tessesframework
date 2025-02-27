#pragma once
#include "Point.hpp"
#include "Size.hpp"
namespace Tesses::Framework::Graphics {
    class Rectangle {
        public:
            Rectangle()
            {

            }
            Rectangle(Point pt, int32_t square) : Rectangle(pt, Graphics::Size(square))
            {

            }
            Rectangle(int32_t x, int32_t y, int32_t square) : Rectangle(Point(x,y),square)
            {

            }
            Rectangle(int32_t x, int32_t y, int32_t width, int32_t height) : Rectangle(Point(x,y),Graphics::Size(width,height))
            {

            }
            Rectangle(Point pt, Size sz)
            {
                this->Location = pt;
                this->Size = sz;
            }

            Graphics::Point Location;
            Graphics::Size Size;

            std::string ToString()
            {
                return std::to_string(Location.X) + ", " + std::to_string(Location.Y) + ", " + std::to_string(Size.Width) + ", " + std::to_string(Size.Height);
            }

            bool Intersects(Point pt)
            {
                
            }
    };
};