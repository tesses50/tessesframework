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

#pragma once
#include "Common.hpp"

namespace Tesses::Framework {
enum class UuidStringifyConfig {
    IsUppercase = 0b001,
    HasCurly = 0b010,
    HasDashes = 0b100,
    UppercaseCompact = IsUppercase,
    LowercaseCompact = 0,
    UppercaseNoCurly = IsUppercase | HasDashes,
    LowercaseNoCurly = HasDashes,
    UppercaseCurly = IsUppercase | HasDashes | HasCurly,
    LowercaseCurly = HasDashes | HasCurly
};
struct Uuid {
    Uuid() = default;
    uint32_t time_low = 0;
    uint16_t time_mid = 0;
    uint16_t time_hi_and_version = 0;
    uint8_t clock_seq_hi_and_reserved = 0;
    uint8_t clock_seq_low = 0;
    uint8_t node[6] = {0, 0, 0, 0, 0, 0};

    static Uuid Generate();
    static void Generate(Uuid &uuid);

    static bool TryParse(std::string text, Uuid &uuid);

    std::string ToString(
        UuidStringifyConfig cfg = UuidStringifyConfig::UppercaseCurly) const;

    bool IsEmpty() const;
};

bool operator==(const Uuid &left, const Uuid &right);
bool operator!=(const Uuid &left, const Uuid &right);
} // namespace Tesses::Framework