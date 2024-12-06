#pragma once

#include <cstring>
#include <cstdint>
#include <cstddef>
#include <string>
#include <filesystem>
#include <map>
#include <vector>

namespace Tesses::Framework
{
    void TF_Init();
    void TF_ConnectToSelf(uint16_t port);
    void TF_RunEventLoop();
    void TF_RunEventLoopItteration();
    bool TF_IsRunning();
    void TF_Quit();
}