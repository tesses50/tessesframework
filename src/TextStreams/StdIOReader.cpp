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
#include "TessesFramework/TextStreams/StdIOReader.hpp"
#include "TessesFramework/Console.hpp"

namespace Tesses::Framework::TextStreams {
ConsoleReader::ConsoleReader() {}
bool ConsoleReader::ReadBlock(std::string &str, size_t len) {
    size_t i = 0;

    for (; i < len;) {
        int rd = Console::Read();
        if (rd == -1)
            break;

        str.push_back((char)rd);

        i++;
    }

    if (i == 0)
        return false;

    return true;
}

ConsoleReader StdIn() { return ConsoleReader(); }
} // namespace Tesses::Framework::TextStreams