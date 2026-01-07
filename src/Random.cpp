#include "TessesFramework/Random.hpp"

namespace Tesses::Framework {
    Random::Random() : Random((uint64_t)time(NULL))
    {

    }
    Random::Random(uint64_t seed) : num(seed)
    {

    }
    uint32_t Random::Next(uint32_t max)
    {
        return (uint32_t)Next(0,(int32_t)max);
    }
    int32_t Random::Next(int32_t min, int32_t max)
    {
        uint32_t number = (uint32_t)(Next() >> 31);
        int32_t range = max-min;
       
        return (uint32_t)((((double)number / (double)0xFFFFFFFF) * (double)range)+min);
    }
    uint64_t Random::Next()
    {
        return num = 6364136223846793005 * num + 1;
    }
    uint8_t Random::NextByte()
    {
        return (uint8_t)Next(0,256);
    }
}