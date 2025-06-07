#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include "TessesFramework/SDL2/FontCache.hpp"
namespace Tesses::Framework::SDL2 {
void FontCache::Load(SDL_Renderer* renderer,TTF_Font* font,const SDL_Color& color)
{
    this->mw=0;
    this->mh=0;
    this->ps=ps;
    
    for(size_t i = 0; i < this->font_chrs.size();i++)
    {
        SDL_Surface* surf = TTF_RenderGlyph_Blended(font,(Uint16)(i+32),color);
        if(surf->w > this->mw) mw = surf->w;
        if(surf->h > this->mh) mh = surf->h;
        this->font_chrs[i] = SDL_CreateTextureFromSurface(renderer,surf);
        SDL_FreeSurface(surf);
       
    }
}
FontCache::FontCache(SDL_Renderer* renderer,std::string font,int sz,const SDL_Color& color)
{
    TTF_Font* f = TTF_OpenFont(font.c_str(),sz);
    Load(renderer,f,color);
    TTF_CloseFont(f);
}
FontCache::FontCache(SDL_Renderer* renderer,const uint8_t* mem,size_t cnt,int sz,const SDL_Color& color)
{
    
    TTF_Font* f = TTF_OpenFontRW(SDL_RWFromConstMem(mem,cnt),1,sz);
    Load(renderer,f,color);
    TTF_CloseFont(f);
}
FontCache::FontCache(SDL_Renderer* renderer,const std::vector<uint8_t>& v,int sz,const SDL_Color& color) : FontCache(renderer,v.data(),v.size(),sz,color)
{

}
FontCache::FontCache(SDL_Renderer* renderer,TTF_Font* font,const SDL_Color& color)
{
    this->Load(renderer,font,color);
}

SDL_Texture* FontCache::operator[](char c)
{
    if(c >= 32 && c <= 126)
    {
        return this->font_chrs[c-32];
    }
    return this->font_chrs[95];
}
FontCache::~FontCache()
{
    for(auto item : this->font_chrs)
        SDL_DestroyTexture(item);
}
int FontCache::MaxWidth()
{
    return this->mw;
}
int FontCache::MaxHeight()
{
    return this->mh;
}

int FontCache::PointSize()
{
    return this->ps;
}
}
#endif