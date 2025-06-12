#pragma once
#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include "../Streams/Stream.hpp"
#include <SDL2/SDL_rwops.h>
namespace Tesses::Framework::SDL2
{
    SDL_RWops* RwopsFromStream(Tesses::Framework::Streams::Stream* strm, bool owns=true);
}
#endif