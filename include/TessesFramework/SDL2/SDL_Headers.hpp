#pragma once
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#if defined(TESSESFRAMEWORK_FETCHCONTENT)
#include <SDL_image.h>
#include <SDL_ttf.h>
#else
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#endif