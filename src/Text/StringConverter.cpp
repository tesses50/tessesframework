/*
    TessesFramework a library to make C++ easier for me, used in CrossLang:
    https://git.tesses.org/tesses50/crosslang

    Copyright (C) 2026 Mike Nolan
    SPDX-License-Identifier: GPL-3.0-or-later WITH TessesFramework-Exception-1.0

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

#include "TessesFramework/Text/StringConverter.hpp"
#include "icubaby.hpp"

namespace Tesses::Framework::Text::StringConverter {
void UTF8::FromUTF16(std::basic_string<char> &utf8,
                     const std::basic_string<char16_t> &utf16) {
    icubaby::t16_8 t;
    auto it = icubaby::iterator{&t, std::back_inserter(utf8)};
    std::copy(std::begin(utf16), std::end(utf16), it);
    t.end_cp(it);
}
void UTF8::FromUTF32(std::basic_string<char> &utf8,
                     const std::basic_string<char32_t> &utf32) {
    icubaby::t32_8 t;
    auto it = icubaby::iterator{&t, std::back_inserter(utf8)};
    std::copy(std::begin(utf32), std::end(utf32), it);
    t.end_cp(it);
}

void UTF16::FromUTF8(std::basic_string<char16_t> &utf16,
                     const std::basic_string<char> &utf8) {
    icubaby::t8_16 t;
    auto it = icubaby::iterator{&t, std::back_inserter(utf16)};
    std::copy(std::begin(utf8), std::end(utf8), it);
    t.end_cp(it);
}
void UTF16::FromUTF32(std::basic_string<char16_t> &utf16,
                      const std::basic_string<char32_t> &utf32) {
    icubaby::t32_16 t;
    auto it = icubaby::iterator{&t, std::back_inserter(utf16)};
    std::copy(std::begin(utf32), std::end(utf32), it);
    t.end_cp(it);
}

void UTF32::FromUTF8(std::basic_string<char32_t> &utf32,
                     const std::basic_string<char> &utf8) {
    icubaby::t8_32 t;
    auto it = icubaby::iterator{&t, std::back_inserter(utf32)};
    std::copy(std::begin(utf8), std::end(utf8), it);
    t.end_cp(it);
}

void UTF32::FromUTF16(std::basic_string<char32_t> &utf32,
                      const std::basic_string<char16_t> &utf16) {
    icubaby::t16_32 t;
    auto it = icubaby::iterator{&t, std::back_inserter(utf32)};
    std::copy(std::begin(utf16), std::end(utf16), it);
    t.end_cp(it);
}
} // namespace Tesses::Framework::Text::StringConverter