#pragma once

#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include <SDL2/SDL.h>
#include <string>

namespace Tesses::Framework::SDL2
{
    bool TryParseSDLColor(std::string str, SDL_Color& col);
}

#endif