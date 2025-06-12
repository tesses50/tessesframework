#if defined(TESSESFRAMEWORK_ENABLE_SDL2)

#include "TessesFramework/SDL2/Views/ProgressView.hpp"

namespace Tesses::Framework::SDL2::Views
{
    ProgressView::ProgressView() : ProgressView(0.0)
    {
        
    }
    ProgressView::ProgressView(double v) : View()
    {
        this->value = v;
        this->SetViewFlag(VIEWFLAG_TABSTOP,false);
    }
    void ProgressView::OnDraw(SDL_Renderer* renderer, SDL_Rect& rect)
    {
        auto win = this->GetWindow();


        SDL_SetRenderDrawColor(renderer,win->palette.border_color.r,win->palette.border_color.g,win->palette.border_color.b,win->palette.border_color.a);

        SDL_Rect r2={.x=rect.x,.y=rect.y,.w=rect.w,.h=rect.h};
        for(size_t i=0;i < 4; i++)
        {
            SDL_RenderDrawRect(renderer,&r2);
            r2.x++;
            r2.y++;
            r2.w-=2;
            r2.h-=2;
        }
        
        auto res = (int)((rect.w-8)*(this->value/100.0));
        r2={.x=rect.x+4,.y=rect.y+4,.w=res,.h=rect.h-8};    
        SDL_SetRenderDrawColor(renderer,win->palette.accent.r,win->palette.accent.g,win->palette.accent.b,win->palette.accent.a);
        SDL_RenderFillRect(renderer,&r2);
    }
}
#endif