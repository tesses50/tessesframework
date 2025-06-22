#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include "TessesFramework/SDL2/Views/PictureView.hpp"

namespace Tesses::Framework::SDL2::Views
{
    PictureView::PictureView()
    {
        this->tex = nullptr;
        this->ownsTex=false;
        this->surf = nullptr;
        this->ownsSurf=false;
    }

    void PictureView::OnDraw(SDL_Renderer* renderer, SDL_Rect& r)
    {
        if(this->surf != nullptr)
        {
            this->SetPicture(SDL_CreateTextureFromSurface(renderer,this->surf));
            if(this->ownsSurf) SDL_FreeSurface(this->surf);
            this->surf=nullptr;
        }
        if(this->tex != nullptr)
        {
            SDL_Rect sr={.x=0,.y=0};
            SDL_QueryTexture(this->tex,nullptr,nullptr,&sr.w,&sr.h);
            SDL_RenderCopy(renderer,this->tex,&sr,&r);
        }
    }

    void PictureView::SetPicture(SDL_Texture* tex, bool owns)
    {
        
        if(this->tex != nullptr && this->tex != tex && this->ownsTex)
            SDL_DestroyTexture(this->tex);
        this->tex = tex;
        this->ownsTex = owns;
    }
    void PictureView::SetPicture(SDL_Surface* surface,bool owns)
    {
        if(this->surf != nullptr && this->surf != surf && this->ownsSurf)
            SDL_FreeSurface(this->surf);
        this->surf = surface;
        this->ownsSurf = owns;
    }

    PictureView::~PictureView()
    {
        if(this->tex != nullptr && this->ownsTex)
            SDL_DestroyTexture(this->tex);
        if(this->surf != nullptr && this->ownsSurf)
            SDL_FreeSurface(this->surf);
    }
}
#endif