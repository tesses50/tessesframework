#pragma once
#include "Common.hpp"

namespace Tesses::Framework {
    class Random {
        uint64_t num;
        public:
            Random();
            Random(uint64_t seed);
            uint64_t Next();
            uint32_t Next(uint32_t max);
            int32_t Next(int32_t min,int32_t max);
            uint8_t NextByte();
    };
}   