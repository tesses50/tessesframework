#pragma once

#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include <array>
#include "SDL_Headers.hpp"
#include <string>
#include <vector>
namespace Tesses::Framework::SDL2
{
class FontCache 
{
    std::array<SDL_Texture*,96> font_chrs;
    int mw,mh,ps;
    void Load(SDL_Renderer* renderer,TTF_Font* font);
    public:
        FontCache(SDL_Renderer* renderer,TTF_Font* font);
        FontCache(SDL_Renderer* renderer,std::string font,int sz);
        FontCache(SDL_Renderer* renderer,const uint8_t* mem,size_t cnt,int sz);
        FontCache(SDL_Renderer* renderer,const std::vector<uint8_t>& v,int sz);
        FontCache(SDL_Renderer* renderer,int sz);
        SDL_Texture* operator[](char c);
        SDL_Texture* GetCharOfColor(char c, const SDL_Color& color);
        int MaxWidth();
        int MaxHeight();
        int PointSize();
        void CalculateSize(std::string text, int& x,int& y);
        void Render(SDL_Renderer* renderer,int x,int y, std::string text, const SDL_Color& color);
        ~FontCache();
};
}
#endif