#pragma once
#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include <array>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
namespace Tesses::Framework::SDL2
{
class FontCache 
{
    std::array<SDL_Texture*,96> font_chrs;
    int mw,mh,ps;
    void Load(SDL_Renderer* renderer,TTF_Font* font,const SDL_Color& color);
    public:
        FontCache(SDL_Renderer* renderer,TTF_Font* font,const SDL_Color& color);
        FontCache(SDL_Renderer* renderer,std::string font,int sz,const SDL_Color& color);
        FontCache(SDL_Renderer* renderer,const uint8_t* mem,size_t cnt,int sz,const SDL_Color& color);
        FontCache(SDL_Renderer* renderer,const std::vector<uint8_t>& v,int sz,const SDL_Color& color);

        SDL_Texture* operator[](char c);
        int MaxWidth();
        int MaxHeight();
        int PointSize();
        ~FontCache();
};
}
#endif