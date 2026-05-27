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

#include "TessesFramework/Console.hpp"
#include "TessesFramework/Http/HttpUtils.hpp"
#include "TessesFramework/Text/StringConverter.hpp"

#if __has_include(<termios.h>)
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#endif
#if defined(_WIN32)
#include <cstdlib>
#include <windows.h>
#endif

#include <limits>

namespace Tesses::Framework {

size_t Console::List(std::vector<std::string> &strs) {
    if (!IsTTY())
        return std::string::npos;

    auto echo = Console::GetEcho();
    auto canonical = Console::GetCanonical();
    Console::SetEcho(false);
    Console::SetCanonical(false);
    size_t i = 0;
    while (true) {
        Console::Clear();
        auto size = Console::GetSize();
        // Console::SetBackgroundColor(ConsoleColor::CC_WHITE,true);
        // Console::SetForegroundColor(ConsoleColor::CC_BLACK,true);

        size_t page = size.second == 0 ? 0 : (i / (size.second));
        size_t offsetInPage = size.second == 0 ? 0 : (i % (size.second));

        for (int item = 0; item < size.second; ++item) {
            if (item == offsetInPage) {
                Console::SetBackgroundColor(ConsoleColor::CC_WHITE, true);
                Console::SetForegroundColor(ConsoleColor::CC_BLACK, false);
            } else {
                Console::SetBackgroundColor(ConsoleColor::CC_BLACK, false);
                Console::SetForegroundColor(ConsoleColor::CC_WHITE, true);
            }

            size_t myOffset = (size_t)item + page * (size.second);

            if (myOffset < strs.size()) {
                if (item == size.second - 1) {
                    Console::Write(strs[myOffset]);
                } else {
                    Console::WriteLine(strs[myOffset]);
                }
            }
        }

        Console::SetBackgroundColor(ConsoleColor::CC_BLACK, false);
        Console::SetForegroundColor(ConsoleColor::CC_WHITE, true);

        int code = Console::Read();

        if (code == 10)
            break;
        if (code == -1)
            break;

        if (code == 27) {
            code = Console::Read();
            if (code == 91) {
                code = Console::Read();
                if (code == 65) {
                    i--;
                    if (i >= strs.size())
                        i = strs.size() - 1;
                } else if (code == 66) {
                    i++;
                    if (i >= strs.size())
                        i = 0;
                } else if (code == -1)
                    break;
            } else if (code == -1)
                break;
        }
    }
    Console::SetEcho(echo);
    Console::SetCanonical(canonical);
    Console::SetBackgroundColor(ConsoleColor::CC_BLACK, false);
    Console::SetForegroundColor(ConsoleColor::CC_WHITE, true);
    return i;
}

void Console::SetForegroundColor(ConsoleColor col, bool alt) {

    if (!IsTTY())
        return;
#if __has_include(<termios.h>)
    if (alt) {
        printf("\x1b[%im", ((int)col) + 90); // this should be Write
    } else {
        printf("\x1b[%im", ((int)col) + 30); // this too
    }
#elif defined(_WIN32)

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE)
        return;
    CONSOLE_SCREEN_BUFFER_INFO bi;

    GetConsoleScreenBufferInfo(hConsole, &bi);

    bi.wAttributes &= ~(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE |
                        FOREGROUND_INTENSITY);

    if (col == ConsoleColor::CC_RED || col == ConsoleColor::CC_YELLOW ||
        col == ConsoleColor::CC_MAGENTA || col == ConsoleColor::CC_WHITE)
        bi.wAttributes |= FOREGROUND_RED;

    if (col == ConsoleColor::CC_GREEN || col == ConsoleColor::CC_YELLOW ||
        col == ConsoleColor::CC_CYAN || col == ConsoleColor::CC_WHITE)
        bi.wAttributes |= FOREGROUND_GREEN;

    if (col == ConsoleColor::CC_BLUE || col == ConsoleColor::CC_CYAN ||
        col == ConsoleColor::CC_MAGENTA || col == ConsoleColor::CC_WHITE)
        bi.wAttributes |= FOREGROUND_BLUE;

    if (alt)
        bi.wAttributes |= FOREGROUND_INTENSITY;

    SetConsoleTextAttribute(hConsole, bi.wAttributes);

#else
    if (alt) {
        printf("\x1b[%im", ((int)col) + 90); // this should be Write
    } else {
        printf("\x1b[%im", ((int)col) + 30); // this too
    }
#endif
}

void Console::SetBackgroundColor(ConsoleColor col, bool alt) {
    if (!IsTTY())
        return;
#if __has_include(<termios.h>)
    if (alt) {
        printf("\x1b[%im", ((int)col) + 100); // this should be Write
    } else {
        printf("\x1b[%im", ((int)col) + 40); // this too
    }
#elif defined(_WIN32)

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE)
        return;
    CONSOLE_SCREEN_BUFFER_INFO bi;

    GetConsoleScreenBufferInfo(hConsole, &bi);

    bi.wAttributes &= ~(BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE |
                        BACKGROUND_INTENSITY);

    if (col == ConsoleColor::CC_RED || col == ConsoleColor::CC_YELLOW ||
        col == ConsoleColor::CC_MAGENTA || col == ConsoleColor::CC_WHITE)
        bi.wAttributes |= BACKGROUND_RED;

    if (col == ConsoleColor::CC_GREEN || col == ConsoleColor::CC_YELLOW ||
        col == ConsoleColor::CC_CYAN || col == ConsoleColor::CC_WHITE)
        bi.wAttributes |= BACKGROUND_GREEN;

    if (col == ConsoleColor::CC_BLUE || col == ConsoleColor::CC_CYAN ||
        col == ConsoleColor::CC_MAGENTA || col == ConsoleColor::CC_WHITE)
        bi.wAttributes |= BACKGROUND_BLUE;

    if (alt)
        bi.wAttributes |= BACKGROUND_INTENSITY;

    SetConsoleTextAttribute(hConsole, bi.wAttributes);
#else
    if (alt) {
        printf("\x1b[%im", ((int)col) + 100); // this should be Write
    } else {
        printf("\x1b[%im", ((int)col) + 40); // this too
    }
#endif
}

bool Console::SetEcho(bool echo) {
    if (!IsTTY())
        return false;
#if __has_include(<termios.h>)
    struct termios raw;
    if (tcgetattr(0, &raw) != 0)
        return false;
    if (echo) {
        raw.c_lflag |= ECHO;
    } else {
        raw.c_lflag &= ~(ECHO);
    }

    if (tcsetattr(0, TCSAFLUSH, &raw) != 0)
        return false;

#elif defined(_WIN32)
    HANDLE hConsole = GetStdHandle(STD_INPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE)
        return false;
    DWORD dwMode;

    if (!GetConsoleMode(hConsole, &dwMode))
        return false;
    if (echo)
        dwMode |= ENABLE_ECHO_INPUT;
    else
        dwMode &= ~(ENABLE_ECHO_INPUT);
    if (SetConsoleMode(hConsole, dwMode))
        return true;
#endif
    return false;
}
bool Console::GetEcho() {
    if (!IsTTY())
        return false;
#if __has_include(<termios.h>)

    struct termios raw;
    if (tcgetattr(0, &raw) != 0)
        return true;
    return (raw.c_lflag & ECHO) > 0;
#elif defined(_WIN32)
    HANDLE hConsole = GetStdHandle(STD_INPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE)
        return false;
    DWORD dwMode;

    if (GetConsoleMode(hConsole, &dwMode)) {
        return (dwMode & ENABLE_ECHO_INPUT) != 0;
    }
#endif

    return true;
}
bool Console::GetCanonical() {
    if (!IsTTY())
        return false;
#if __has_include(<termios.h>)
    struct termios raw;
    if (tcgetattr(0, &raw) != 0)
        return false;
    return (raw.c_lflag & ICANON) > 0;
#elif defined(_WIN32)
    HANDLE hConsole = GetStdHandle(STD_INPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE)
        return false;
    DWORD dwMode;

    if (GetConsoleMode(hConsole, &dwMode)) {
        return (dwMode & ENABLE_LINE_INPUT) != 0;
    }
#endif
    return false;
}
bool Console::SetCanonical(bool can) {
    if (!IsTTY())
        return false;
#if __has_include(<termios.h>)
    struct termios raw;
    if (tcgetattr(0, &raw) != 0)
        return false;
    if (can) {
        raw.c_lflag |= ICANON;
    } else {
        raw.c_lflag &= ~(ICANON);
    }

    if (tcsetattr(0, TCSAFLUSH, &raw) != 0)
        return false;

#elif defined(_WIN32)
    HANDLE hConsole = GetStdHandle(STD_INPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE)
        return false;
    DWORD dwMode;

    if (!GetConsoleMode(hConsole, &dwMode))
        return false;
    if (can)
        dwMode |= ENABLE_LINE_INPUT;
    else
        dwMode &= ~(ENABLE_LINE_INPUT);
    if (SetConsoleMode(hConsole, dwMode))
        return true;
#endif
    return false;
}

bool Console::GetSignals() {
    if (!IsTTY())
        return true;
#if __has_include(<termios.h>)
    struct termios raw;
    if (tcgetattr(0, &raw) != 0)
        return false;
    return (raw.c_lflag & ISIG) > 0;

#endif
    return false;
}
bool Console::SetSignals(bool sig) {
    if (!IsTTY())
        return false;
#if __has_include(<termios.h>)
    struct termios raw;
    if (tcgetattr(0, &raw) != 0)
        return false;
    if (sig) {
        raw.c_lflag |= ISIG;
    } else {
        raw.c_lflag &= ~(ISIG);
    }

    if (tcsetattr(0, TCSAFLUSH, &raw) != 0)
        return false;

#endif
    return false;
}

// ISIG

std::string Console::ReadPassword() {
    bool echo = GetEcho();
    if (!SetEcho(false)) {
        Write("\nWARN: the password will be visible: ");
    }
    std::string text = ReadLine();
    SetEcho(echo);
    Write("\n");

    return text;
}

#if defined(_WIN32)
thread_local std::string key;
thread_local int keyOffset = -1;
#endif

int Console::Read() {
#if defined(WIN32)
    if (keyOffset >= 0 && keyOffset < key.size()) {
        return (int)key[keyOffset++];
    } else {
        keyOffset = -1;
    }
    if (!Console::GetCanonical()) {

        HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
        if (hIn == INVALID_HANDLE_VALUE)
            return -1;

        DWORD nRead;

        INPUT_RECORD ir;

        while (ReadConsoleInputW(hIn, &ir, 1, &nRead)) {
            if (nRead == 0)
                return -1;
            if (ir.EventType == KEY_EVENT && ir.Event.KeyEvent.bKeyDown) {
                if (ir.Event.KeyEvent.wVirtualKeyCode == VK_UP) {
                    key = {27, 91, 65};
                    keyOffset = 1;
                    return key[0];
                } else if (ir.Event.KeyEvent.wVirtualKeyCode == VK_DOWN) {
                    key = {27, 91, 66};
                    keyOffset = 1;
                    return key[0];
                } else if (ir.Event.KeyEvent.wVirtualKeyCode == VK_LEFT) {
                    key = {27, 91, 68};
                    keyOffset = 1;
                    return key[0];
                } else if (ir.Event.KeyEvent.wVirtualKeyCode == VK_RIGHT) {
                    key = {27, 91, 67};
                    keyOffset = 1;
                    return key[0];
                } else if (ir.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE) {
                    return 27;
                } else if (ir.Event.KeyEvent.wVirtualKeyCode == VK_DELETE) {
                    key = {27, 91, 51, 126};
                    keyOffset = 1;
                    return key[0];
                } else if (ir.Event.KeyEvent.wVirtualKeyCode == VK_RETURN) {
                    return 10;
                }

                else if (ir.Event.KeyEvent.uChar.UnicodeChar != 0) {
                    if (ir.Event.KeyEvent.uChar.UnicodeChar <= 127) {
                        return (int)ir.Event.KeyEvent.uChar.UnicodeChar;
                    } else if (ir.Event.KeyEvent.uChar.UnicodeChar >= 0xD800 &&
                               ir.Event.KeyEvent.uChar.UnicodeChar <= 0xDBFF) {

                        std::u16string str = {
                            {(char16_t)ir.Event.KeyEvent.uChar.UnicodeChar, 0}};
                        while (ReadConsoleInputW(hIn, &ir, 1, &nRead)) {
                            if (ir.EventType == KEY_EVENT &&
                                ir.Event.KeyEvent.bKeyDown) {

                                if (nRead == 0)
                                    return -1;
                                if (ir.Event.KeyEvent.uChar.UnicodeChar >=
                                        0xDC00 &&
                                    ir.Event.KeyEvent.uChar.UnicodeChar <=
                                        0xDFFF) {
                                    str[1] =
                                        (char16_t)
                                            ir.Event.KeyEvent.uChar.UnicodeChar;
                                    break;
                                } else {
                                    return -1;
                                }
                            }
                        }

                        key.clear();
                        keyOffset = 1;
                        Tesses::Framework::Text::StringConverter::UTF8::
                            FromUTF16(key, str);
                        if (!key.empty())
                            return key[0];
                        return -1;

                    } else if (ir.Event.KeyEvent.uChar.UnicodeChar > 127) {
                        // normal unicode char
                        key.clear();
                        keyOffset = 1;

                        std::u16string str = {
                            {(char16_t)ir.Event.KeyEvent.uChar.UnicodeChar}};

                        Tesses::Framework::Text::StringConverter::UTF8::
                            FromUTF16(key, str);

                        if (!key.empty())
                            return key[0];
                        return -1;
                    }
                }
            }
        }

        return -1;
    } else {
        WCHAR chr;
        DWORD chrRead;
        HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
        if (hIn == INVALID_HANDLE_VALUE)
            return -1;
        if (ReadConsoleW(hIn, &chr, 1, &chrRead, NULL)) {
            if (chrRead == 0)
                return -1;

            if (chr <= 127)
                return (int)chr;
            else if (chr >= 0xD800 && chr <= 0xDBFF) {
                std::u16string str = {{(char16_t)chr, 0}};

                if (ReadConsoleW(hIn, &chr, 1, &chrRead, NULL)) {
                    if (chrRead == 0)
                        return -1;
                    if (chr >= 0xDC00 && chr <= 0xDFFF) {
                        str[1] = (char16_t)chr;

                    } else {
                        return -1;
                    }
                }

                key.clear();
                keyOffset = 1;
                Tesses::Framework::Text::StringConverter::UTF8::FromUTF16(key,
                                                                          str);
                if (!key.empty())
                    return key[0];
                return -1;
            } else if (chr > 127) {
                // normal unicode char
                key.clear();
                keyOffset = 1;

                std::u16string str = {{(char16_t)chr}};

                Tesses::Framework::Text::StringConverter::UTF8::FromUTF16(key,
                                                                          str);

                if (!key.empty())
                    return key[0];
                return -1;
            }
        }
        return -1;
    }
#else
    int res = fgetc(stdin);
    if (res == EOF)
        return -1; // make sure it's -1
    return res;
#endif
}

std::string Console::ReadLine() {
    std::string text;
    int c = Read();
    while (c != '\n' && c != -1) {
        text += (char)c;

        c = Read();
    }
    if (!text.empty() && text.back() == '\r')
        text.resize(text.size() - 1);
    return text;
}
#if defined(_WIN32)
void writeTextWin32(HANDLE hndl, const std::string &text) {
    std::u16string str = {};

    Text::StringConverter::UTF16::FromUTF8(str, text);

    auto buff = str.data();
    DWORD left = (DWORD)str.size();

    DWORD read = 0;
    do {

        if (WriteConsoleW(hndl, buff, left, &read, NULL)) {
            buff += read;
            left -= read;
        }
    } while (read > 0);
}
#else
void writeTextUnix(FILE *f, std::string_view view) {
    size_t left = view.size();
    const char *ptr = view.data();
    size_t read = 0;
    do {
        read = fwrite(ptr, 1, left, f);
        ptr += read;
        left -= read;
    } while (read != 0);
}
#endif
void Console::WriteToStream(std::string_view view, bool isStderr) {
#if defined(_WIN32)
    HANDLE hOut = GetStdHandle(isStderr ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return;
    std::string text;

    for (size_t i = 0; i < view.size(); ++i) {
        if (view[i] == '\x1b') {
            if (!text.empty()) {
                writeTextWin32(hOut, text);
                // fwrite(text.c_str(), 1, text.size(), f);
                text.clear();
            }
            i++;
            if (i < view.size()) {
                if (view[i] == '[') {
                    i++;
                    for (; i < view.size(); ++i) {
                        text += view[i];
                        if (view[i] >= 0x40 && view[i] <= 0x7e)
                            break;
                    }
                    if (text.size() > 0 && text.back() == 'H') {
                        MoveToHome();
                    }
                    if (text.size() > 0 && text.back() == 'J') {
                        if (text[0] == '2') {
                            ClearRetainPosition(
                                ClearBehaviour::CB_ENTIRESCREEN);
                        }

                        if (text[0] == '1') {
                            ClearRetainPosition(
                                ClearBehaviour::CB_CURSORANDABOVE);
                        }
                        if (text[0] == '0') {
                            ClearRetainPosition(
                                ClearBehaviour::CB_CURSORANDBELOW);
                        }
                    }

                    if (text.size() > 0 && text.back() == 'm') {

                        try {
                            auto num =
                                std::stol(text.substr(0, text.size() - 1));
                            if (num >= 30 && num <= 37) {
                                SetForegroundColor((ConsoleColor)(num - 30),
                                                   false);
                            } else if (num >= 40 && num <= 47) {
                                SetBackgroundColor((ConsoleColor)(num - 40),
                                                   false);
                            } else if (num >= 90 && num <= 97) {
                                SetForegroundColor((ConsoleColor)(num - 90),
                                                   true);
                            } else if (num >= 100 && num <= 107) {
                                SetBackgroundColor((ConsoleColor)(num - 100),
                                                   true);
                            }
                        } catch (...) {
                        }
                    }

                    text.clear();

                } else {
                    text += "\x1B";
                    text += view[i];
                }
            }

        } else if (view[i] == '\n') {
            text += "\r\n";

            writeTextWin32(hOut, text);
            text.clear();
        } else {
            text += view[i];
        }
    }

    if (!text.empty()) {

        writeTextWin32(hOut, text);
        text.clear();
    }

#else
    writeTextUnix(isStderr ? stderr : stdout, view);
#endif
}

void Console::Write(std::string text) { WriteToStream(text, false); }
void Console::WriteLine(std::string text) {
    Write(text);
    Write("\n");
}

void Console::WriteLineView(std::string_view view) {
    std::string lf = "\n";
    WriteToStream(view, false);
    WriteToStream(lf, false);
}
void Console::WriteView(std::string_view view) { WriteToStream(view, false); }
void Console::Error(std::string text) { WriteToStream(text, true); }
void Console::ErrorLine(std::string text) {
    Error(text);
    Error("\n");
}

void Console::ErrorLineView(std::string_view view) {
    std::string lf = "\n";
    WriteToStream(view, true);
    WriteToStream(lf, true);
}
void Console::ErrorView(std::string_view view) { WriteToStream(view, true); }
bool Console::IsTTY() {
#if defined(_WIN32)
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return false;
    DWORD tmp;
    if (GetConsoleMode(hOut, &tmp))
        return true;
#elif __has_include(<termios.h>)
    if (isatty(STDOUT_FILENO))
        return true;
#endif

    return false;
}

void Console::ProgressBar(double pdbl) {
    bool showBar = IsTTY();
    if (pdbl < 0)
        pdbl = 0;
    if (pdbl > 1)
        pdbl = 1;

    WriteView("\r");
    if (showBar) {
        auto sz = GetSize();

        int totalBlocks = sz.first - 10;
        if (totalBlocks > 0) {
            WriteView("[\033[0;32m");
            int i;
            int off = pdbl * totalBlocks;
            for (int i = 0; i < totalBlocks; i++) {
                if (i < off)
                    WriteView("=");
                else
                    WriteView(" ");
            }

            WriteView("\033[0m] ");
        }
    }

    std::string mesg = Http::HttpUtils::LeftPad(
        std::to_string((int)(pdbl * 100)) + "%", 4, ' ');
    WriteView(mesg);
    Flush();
}
void Console::ProgressBar(int v) { ProgressBar((double)v / 100.0); }

std::pair<int, int> Console::GetSize() {
    if (!Console::IsTTY())
        return std::pair<int, int>(0, 0);

#if __has_include(<termios.h>)
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
        return std::make_pair<int, int>(ws.ws_col, ws.ws_row);
    }
#elif defined(_WIN32)

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE)
        return std::make_pair<int, int>(0, 0);
    CONSOLE_SCREEN_BUFFER_INFO bi;

    GetConsoleScreenBufferInfo(hConsole, &bi);

    return std::make_pair<int, int>(bi.srWindow.Right - bi.srWindow.Left + 1,
                                    bi.srWindow.Bottom - bi.srWindow.Top + 1);
#endif

    return std::make_pair<int, int>(0, 0);
}

void Console::SetPosition(int x, int y) {
    if (!IsTTY())
        return;
#if __has_include(<termios.h>)
    printf("\x1B[%i;%iH", y + 1, x + 1);
#elif defined(_WIN32)
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return;
    COORD coord = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(hOut, coord);
#else
    printf("\x1B[%i;%iH", y + 1, x + 1);
    Flush();
#endif
}

void Console::Flush() { fflush(stdout); }

void Console::Clear() {
    ClearRetainPosition(ClearBehaviour::CB_ENTIRESCREEN);
    MoveToHome();
}

void Console::ClearRetainPosition(ClearBehaviour cb) {
#if __has_include(<termios.h>)
    if (isatty(STDOUT_FILENO)) {
        printf("\x1b[%iJ", (int)cb);
        Flush();
    }
#elif defined(_WIN32)
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return;
    CONSOLE_SCREEN_BUFFER_INFO bi;

    GetConsoleScreenBufferInfo(hOut, &bi);

    switch (cb) {
    case ClearBehaviour::CB_CURSORANDABOVE: {
        COORD home = {0, 0};

        DWORD cells =
            ((bi.dwCursorPosition.Y) * bi.dwSize.X) + bi.dwCursorPosition.X + 1;
        DWORD written;

        FillConsoleOutputCharacter(hOut, ' ', cells, home, &written);
        FillConsoleOutputAttribute(hOut, bi.wAttributes, cells, home, &written);

    } break;
    case ClearBehaviour::CB_CURSORANDBELOW: {
        if (bi.dwCursorPosition.X == 0) {
            COORD home = bi.dwCursorPosition;
            DWORD cells = bi.dwSize.X * (bi.dwSize.Y - bi.dwCursorPosition.Y);
            DWORD written;

            FillConsoleOutputCharacter(hOut, ' ', cells, home, &written);
            FillConsoleOutputAttribute(hOut, bi.wAttributes, cells, home,
                                       &written);
        } else {
            COORD home = bi.dwCursorPosition;
            DWORD cells = bi.dwSize.X - home.X;
            DWORD written;

            FillConsoleOutputCharacter(hOut, ' ', cells, home, &written);
            FillConsoleOutputAttribute(hOut, bi.wAttributes, cells, home,
                                       &written);

            if (home.Y + 1 < bi.dwSize.Y) {
                home = {0, (SHORT)(bi.dwCursorPosition.Y + 1)};
                cells = bi.dwSize.X * (bi.dwSize.Y - home.Y);

                FillConsoleOutputCharacter(hOut, ' ', cells, home, &written);
                FillConsoleOutputAttribute(hOut, bi.wAttributes, cells, home,
                                           &written);
            }
        }
    } break;
    case ClearBehaviour::CB_ENTIRESCREEN: {
        COORD home = {0, 0};

        DWORD cells = bi.dwSize.X * bi.dwSize.Y;
        DWORD written;

        FillConsoleOutputCharacter(hOut, ' ', cells, home, &written);
        FillConsoleOutputAttribute(hOut, bi.wAttributes, cells, home, &written);
    } break;
    }

#else
    printf("\x1b[%iJ", (int)cb);
    Flush();
#endif
}
void Console::MoveToHome() {
#if __has_include(<termios.h>)
    if (isatty(STDOUT_FILENO)) {
        Write("\x1b[H");
        Flush();
    }
#elif defined(_WIN32)
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return;
    SetConsoleCursorPosition(hOut, {0, 0});
#else
    Write("\x1b[H");
    Flush();
#endif
}
} // namespace Tesses::Framework