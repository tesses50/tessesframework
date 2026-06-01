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

#include "Common.hpp"
#include <cstdio>

namespace Tesses::Framework {
enum class ConsoleColor {
    CC_BLACK,
    CC_RED,
    CC_GREEN,
    CC_YELLOW,
    CC_BLUE,
    CC_MAGENTA,
    CC_CYAN,
    CC_WHITE
};
enum class ClearBehaviour {
    CB_CURSORANDBELOW,
    CB_CURSORANDABOVE,
    CB_ENTIRESCREEN
};
struct Console {
    static bool IsTTY();
    static std::pair<int, int> GetSize();
    static void ProgressBar(double);
    static void ProgressBar(int);
    static void Write(std::string text);
    static void WriteLine(std::string text);
    static void WriteView(std::string_view text);
    static void WriteLineView(std::string_view text);
    static void Error(std::string text);
    static void ErrorLine(std::string text);
    static void ErrorView(std::string_view text);
    static void ErrorLineView(std::string_view text);
    static int Read();
    static std::string ReadLine();

    static bool GetEcho();
    static bool SetEcho(bool echo);
    static bool GetCanonical();
    static bool SetCanonical(bool can);
    static bool GetSignals();
    static bool SetSignals(bool sig);

    static void SetPosition(int x, int y);

    static void SetBackgroundColor(ConsoleColor c, bool alt);
    static void SetForegroundColor(ConsoleColor c, bool alt);
    static void Clear();
    static void MoveToHome();
    static void ClearRetainPosition(ClearBehaviour clearBehaviour);
    static void Reset();
    static void SetInvertedColors(bool inverted);
    static void Flush();

    // returns std::string::npos if no tty
    static size_t List(std::vector<std::string> &strs);

    static std::string ReadPassword();

  private:
    static void WriteToStream(std::string_view text, bool isStderr);
};
} // namespace Tesses::Framework