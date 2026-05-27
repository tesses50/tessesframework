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
#include <cstdint>
#include <string>
namespace Tesses::Framework::Text::StringConverter {
class UTF8 {
  public:
    static void FromUTF16(std::basic_string<char> &utf8,
                          const std::basic_string<char16_t> &utf16);
    static void FromUTF32(std::basic_string<char> &utf8,
                          const std::basic_string<char32_t> &utf32);
};
class UTF16 {
  public:
    static void FromUTF8(std::basic_string<char16_t> &utf16,
                         const std::basic_string<char> &utf8);
    static void FromUTF32(std::basic_string<char16_t> &utf16,
                          const std::basic_string<char32_t> &utf32);
};
class UTF32 {
  public:
    static void FromUTF8(std::basic_string<char32_t> &utf32,
                         const std::basic_string<char> &utf8);
    static void FromUTF16(std::basic_string<char32_t> &utf32,
                          const std::basic_string<char16_t> &utf16);
};
} // namespace Tesses::Framework::Text::StringConverter