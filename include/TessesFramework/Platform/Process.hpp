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
#include "TessesFramework/HiddenField.hpp"
#include "TessesFramework/Streams/Stream.hpp"
#include <signal.h>
#include <string>
#include <vector>
#if defined(_WIN32)
#define SIGKILL 9
#endif

namespace Tesses::Framework::Platform {
class Process {
  private:
    HiddenField hidden;
    bool exited = false;
    int exitCode = 0;

  public:
    std::string name;
    std::vector<std::string> args;
    std::string workingDirectory;
    std::vector<std::pair<std::string, std::string>> env;
    bool includeThisEnv;
    bool redirectStdIn = false;
    bool redirectStdOut = false;
    bool redirectStdErr = false;
    bool HasExited();

    void CloseStdInNow();

    std::shared_ptr<Tesses::Framework::Streams::Stream> GetStdinStream();

    std::shared_ptr<Tesses::Framework::Streams::Stream> GetStdoutStream();
    std::shared_ptr<Tesses::Framework::Streams::Stream> GetStderrStream();

    Process();
    Process(std::string name, std::vector<std::string> args,
            bool includeThisEnv = true);
    Process(std::string name, std::vector<std::string> args,
            std::vector<std::pair<std::string, std::string>> env,
            bool includeThisEnv = false);
    Process(std::string name, std::vector<std::string> args,
            std::vector<std::string> env, bool includeThisEnv = false);

    bool Start();

    void Kill(int signal);

    int WaitForExit();

    ~Process();
};
} // namespace Tesses::Framework::Platform