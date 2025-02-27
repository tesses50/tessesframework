#pragma once
#include <cstdint>
#include <string>
namespace Tesses::Framework::Graphics {
    class Point {
        public:
            Point()
            {

            }
            Point(int32_t x, int32_t y)
            {
                this->X = x;
                this->Y = y;
            }
            int32_t X=0;
            int32_t Y=0;
            std::string ToString()
            {
                return std::to_string(X) + ", " + std::to_string(Y);
            }
    };
};