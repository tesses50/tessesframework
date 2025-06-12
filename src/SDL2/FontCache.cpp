#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include "TessesFramework/SDL2/FontCache.hpp"
#include "TessesFramework/Http/HttpUtils.hpp"
#include "TessesFramework/TanoheSans-Regular.h"
using HU= Tesses::Framework::Http::HttpUtils;
namespace Tesses::Framework::SDL2 {
void FontCache::Load(SDL_Renderer* renderer,TTF_Font* font)
{
    this->mw=0;
    this->mh=0;
    this->ps=ps;
    
    for(size_t i = 0; i < this->font_chrs.size();i++)
    {
        SDL_Surface* surf = TTF_RenderGlyph_Blended(font,(Uint16)(i+32),{.r=255,.g=255,.b=255,.a=255});
        if(surf->w > this->mw) mw = surf->w;
        if(surf->h > this->mh) mh = surf->h;
        this->font_chrs[i] = SDL_CreateTextureFromSurface(renderer,surf);
        SDL_FreeSurface(surf);
       
    }
}
FontCache::FontCache(SDL_Renderer* renderer,std::string font,int sz)
{
    TTF_Font* f = TTF_OpenFont(font.c_str(),sz);
    Load(renderer,f);
    TTF_CloseFont(f);
}
FontCache::FontCache(SDL_Renderer* renderer,const uint8_t* mem,size_t cnt,int sz)
{
    
    TTF_Font* f = TTF_OpenFontRW(SDL_RWFromConstMem(mem,cnt),1,sz);
    Load(renderer,f);
    TTF_CloseFont(f);
}
FontCache::FontCache(SDL_Renderer* renderer,const std::vector<uint8_t>& v,int sz) : FontCache(renderer,v.data(),v.size(),sz)
{

}
FontCache::FontCache(SDL_Renderer* renderer,TTF_Font* font)
{
    this->Load(renderer,font);
}
FontCache::FontCache(SDL_Renderer* renderer,int sz) : FontCache(renderer,TANOHESANSREGULAR,TANOHESANSREGULAR_SIZE,sz)
{
    
}
void FontCache::CalculateSize(std::string text, int& x,int& y)
{
    int myX = 0;
    x=0;
    y=0;
    int maxH =  MaxHeight();
    y=maxH;
    for(auto c : text)
    {
        switch(c)
        {
            case '\n':
            {
                y += maxH;
                if(myX > x) x = myX;
                myX = 0;
            }
            break;
            case '\t':
            {
                auto tex = operator[](' ');
                int wi;
                SDL_QueryTexture(tex,NULL,NULL,&wi,NULL);
                
                myX += wi * 4;
            }
            break;
            default:
            {
                auto tex = operator[](c);
                int wi;
                int he;
                SDL_QueryTexture(tex,NULL,NULL,&wi,&he);
                
                myX += wi;
            }
            break;

        }
    }
    if(myX > x) x = myX;
}
void FontCache::Render(SDL_Renderer* renderer,int x,int y, std::string text,const SDL_Color& color)
{
    int myX = x;
    int maxH =  MaxHeight();
    for(auto c : text)
    {
        switch(c)
        {
            case '\n':
            {
                y += maxH;
                myX = x;
            }
            break;
            case '\t':
            {
                auto tex = GetCharOfColor(' ',color);
                int wi;
                SDL_QueryTexture(tex,NULL,NULL,&wi,NULL);
                
                myX += wi * 4;
            }
            break;
            default:
            {
                auto tex = GetCharOfColor(c,color);
                int wi;
                int he;
                SDL_QueryTexture(tex,NULL,NULL,&wi,&he);
                SDL_Rect src={.x=0,.y=0,.w=wi,.h=he};
                
                SDL_Rect dest={.x=myX,.y=y,.w=wi,.h=he};

                myX += wi;
                SDL_RenderCopy(renderer,tex,&src,&dest);
            }
            break;

        }
    }
}
SDL_Texture* FontCache::GetCharOfColor(char c, const SDL_Color& color)
{
    auto res = (c >= 32 && c <= 126) ? this->font_chrs[c-32] : this->font_chrs[95];
    SDL_SetTextureColorMod(res,color.r,color.g,color.b);
    return res;
}
SDL_Texture* FontCache::operator[](char c)
{
    return GetCharOfColor(c,{.r=255,.g=255,.b=255,.a=255});
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

bool TryParseSDLColor(std::string str, SDL_Color& col)
{
    str = HU::Replace(str," ","");
    //rgba(255,255,255,1.0)
    //rgb(197, 30, 30)
    //#rgb
    //#rgba
    //#rrggbb
    //#rrggbbaa

    if(str.size() >= 4 && str[0] == '#')
    {
        if(str.size() == 4)
        {
            auto r = HU::HexToNibble(str[1]);
            r |= r << 4;
            auto g = HU::HexToNibble(str[2]);
            g |= g << 4;
            auto b = HU::HexToNibble(str[3]);
            b |= b << 4;
            col.r = r;
            col.g = g;
            col.b = b;
            col.a = 255;
            return true;
        }
        else if(str.size() == 5)
        {
            auto r = HU::HexToNibble(str[1]);
            r |= r << 4;
            auto g = HU::HexToNibble(str[2]);
            g |= g << 4;
            auto b = HU::HexToNibble(str[3]);
            b |= b << 4;
            auto a = HU::HexToNibble(str[4]);
            a |= a << 4;
            col.r = r;
            col.g = g;
            col.b = b;
            col.a = a;
            return true;
        }
        else if(str.size() == 7) {
            auto r = HU::HexToNibble(str[1]);
            r |= HU::HexToNibble(str[2]) << 4;
            auto g = HU::HexToNibble(str[3]);
            g |= HU::HexToNibble(str[4]) << 4;
            auto b = HU::HexToNibble(str[5]);
            b |= HU::HexToNibble(str[6]) << 4;
            
            col.r = r;
            col.g = g;
            col.b = b;
            col.a = 255;
            return true;
        }
        else if(str.size() == 9)
        {
            auto r = HU::HexToNibble(str[1]);
            r |= HU::HexToNibble(str[2]) << 4;
            auto g = HU::HexToNibble(str[3]);
            g |= HU::HexToNibble(str[4]) << 4;
            auto b = HU::HexToNibble(str[5]);
            b |= HU::HexToNibble(str[6]) << 4;
            auto a = HU::HexToNibble(str[7]);
            a |= HU::HexToNibble(str[8]) << 4;
            
            col.r = r;
            col.g = g;
            col.b = b;
            col.a = a;
            return true;
        }
    }
    else if(str.size() > 4 && strncmp(str.c_str(),"rgb(",4) == 0 && str[str.size()-1] == ')')
    {
        str = HU::Replace(str.substr(4),")","");
        auto parts = HU::SplitString(str,",");
        if(parts.size() != 3) return false;
        try{
        auto r = std::stoul(parts[0]);
        auto g = std::stoul(parts[1]);
        auto b = std::stoul(parts[2]);
        col.r = (Uint8)r;
        col.g = (Uint8)g;
        col.b = (Uint8)b;
        col.a = 255;
        } catch(...) {
            return false;
        }

        return true;
    }
    else if(str.size() > 5 && strncmp(str.c_str(),"rgba(",5) == 0 && str[str.size()-1] == ')')
    {
        str = HU::Replace(str.substr(5),")","");
        auto parts = HU::SplitString(str,",");
        if(parts.size() != 4) return false;
        try{
        auto r = std::stoul(parts[0]);
        auto g = std::stoul(parts[1]);
        auto b = std::stoul(parts[2]);
        double a= std::stod(parts[3]);
        col.r = (Uint8)r;
        col.g = (Uint8)g;
        col.b = (Uint8)b;
        col.a = (Uint8)(a * 255);
        } catch(...) {
            return false;
        }
        return true;
    }
    return false;
}

}
#endif