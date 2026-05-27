/*
    TessesFramework a library to make C++ easier for me, used in CrossLang:
   https://git.tesses.org/tesses50/crosslang Copyright (C) 2026 Mike Nolan

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "TessesFramework/Random.hpp"

namespace Tesses::Framework {
Random::Random() : Random((uint64_t)time(NULL)) {}
Random::Random(uint64_t seed) : num(seed) {}
uint32_t Random::Next(uint32_t max) { return (uint32_t)Next(0, (int32_t)max); }
int32_t Random::Next(int32_t min, int32_t max) {
    uint32_t number = (uint32_t)(Next() >> 31);
    int32_t range = max - min;

    return (uint32_t)((((double)number / (double)0xFFFFFFFF) * (double)range) +
                      min);
}
uint64_t Random::Next() { return num = 6364136223846793005 * num + 1; }
uint8_t Random::NextByte() { return (uint8_t)Next(0, 256); }
} // namespace Tesses::Framework