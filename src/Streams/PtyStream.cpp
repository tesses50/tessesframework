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

#include "TessesFramework/Streams/PtyStream.hpp"
#if !defined(GEKKO) && !defined(__PS2__) &&             \
    !defined(_WIN32) && !defined(__SWITCH__) &&       \
    defined(TESSESFRAMEWORK_ENABLE_PROCESS)
#if __has_include(<util.h>)
#include <util.h>
#elif __has_include(<pty.h>)
#include <pty.h>
#elif __has_include(<libutil.h>)
#include <libutil.h>
#endif
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>

#endif
namespace Tesses::Framework::Streams {
PtyStream::PtyStream(WindowSize windowSize, std::string filename,
                     std::vector<std::string> args,
                     std::vector<std::string> env) {
#if !defined(GEKKO) && !defined(__PS2__) &&             \
    !defined(_WIN32) && !defined(__SWITCH__) &&       \
    defined(TESSESFRAMEWORK_ENABLE_PROCESS)
    this->wS = windowSize;
    this->eos = false;
    winsize sz;
    sz.ws_col = (unsigned short)windowSize.Columns;
    sz.ws_row = (unsigned short)windowSize.Rows;
    sz.ws_xpixel = (unsigned short)windowSize.Width;
    sz.ws_ypixel = (unsigned short)windowSize.Height;
    termios ios;
    cfmakeraw(&ios);

    pid = forkpty(&this->socket, NULL, &ios, &sz);
    if (pid == -1) {
        this->eos = true;
    }
    if (pid == 0) {
        char **argv = new char *[args.size() + 1];
        argv[args.size()] = NULL;
        char **envp = new char *[env.size() + 1];
        envp[env.size()] = NULL;

        for (size_t i = 0; i < args.size(); i++) {
            argv[i] = (char *)args[i].c_str();
        }
        for (size_t i = 0; i < env.size(); i++) {
            envp[i] = (char *)env[i].c_str();
        }

        if (execve(filename.c_str(), argv, envp) == -1) {
            perror("execve returned -1");
            exit(1);
        }
    }
    int flags = fcntl(this->socket, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl F_GETFL");
        this->eos = true;
        return;
    }
    flags |= O_NONBLOCK;

    flags = fcntl(this->socket, F_SETFL, flags);
    if (flags == -1) {
        perror("fcntl F_SETFL");

        this->eos = true;
        return;
    }
#endif
}
bool PtyStream::EndOfStream() { return this->eos; }
bool PtyStream::CanRead() { return true; }
bool PtyStream::CanWrite() { return true; }
size_t PtyStream::Read(uint8_t *buff, size_t sz) {
    if (this->eos)
        return 0;
#if !defined(GEKKO) && !defined(__PS2__) &&             \
    !defined(_WIN32) && !defined(__SWITCH__) &&       \
    defined(TESSESFRAMEWORK_ENABLE_PROCESS)
    auto res = read(this->socket, buff, sz);

    if (res == -1) {
        if (errno != EAGAIN && errno != EWOULDBLOCK)
            this->eos = true;
        return 0;
    }
    return (size_t)res;
#else
    return 0;
#endif
}
size_t PtyStream::Write(const uint8_t *buff, size_t sz) {
#if !defined(GEKKO) && !defined(__PS2__) &&             \
    !defined(_WIN32) && !defined(__SWITCH__)  &&       \
    defined(TESSESFRAMEWORK_ENABLE_PROCESS)
    auto res = write(this->socket, buff, sz);
    return res;
#else
    return 0;
#endif
}
WindowSize PtyStream::GetWindowSize() { return this->wS; }
void PtyStream::Resize(WindowSize windowSize) {
#if !defined(GEKKO) && !defined(__PS2__) &&             \
    !defined(_WIN32) && !defined(__SWITCH__) &&       \
    defined(TESSESFRAMEWORK_ENABLE_PROCESS)
    this->wS = windowSize;
    winsize sz;
    sz.ws_col = (unsigned short)windowSize.Columns;
    sz.ws_row = (unsigned short)windowSize.Rows;
    sz.ws_xpixel = (unsigned short)windowSize.Width;
    sz.ws_ypixel = (unsigned short)windowSize.Height;

    ioctl(this->socket, TIOCSWINSZ, &sz);
#endif
}
PtyStream::~PtyStream() {
    if (this->eos)
        return;
    this->eos = true;
#if !defined(GEKKO) && !defined(__PS2__) &&             \
    !defined(_WIN32) && !defined(__SWITCH__) &&       \
    defined(TESSESFRAMEWORK_ENABLE_PROCESS)
    close(this->socket);

    kill((pid_t)this->pid, SIGHUP);
#endif
}
void PtyStream::Close() {
    if (this->eos)
        return;
    this->eos = true;
#if !defined(GEKKO) && !defined(__PS2__) &&             \
    !defined(_WIN32) && !defined(__SWITCH__) &&       \
    defined(TESSESFRAMEWORK_ENABLE_PROCESS)
    close(this->socket);

    kill((pid_t)this->pid, SIGHUP);
#endif
}
} // namespace Tesses::Framework::Streams