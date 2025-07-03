#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include "TessesFramework/SDL2/Views/ButtonView.hpp"

namespace Tesses::Framework::SDL2::Views
{
    ButtonView::ButtonView() : View()
    {

    }
    ButtonView::ButtonView(std::string text) : View(text)
    {
        
    }
    std::pair<int,int> ButtonView::PreferedMinSize()
    {
        int w=-2;
        int h=-2;
        
        auto win = GetWindow();
        if(win != nullptr)
        {
            win->normal_font->CalculateSize(text,w,h);

            w += win->palette.borderSize*3;

            h += win->palette.borderSize*3;
        }
        return std::pair<int,int>(w,h);
    }
    bool ButtonView::OnEvent(SDL_Event& event, SDL_Rect& myBounds, SDL_Rect& myVisibleBounds)
    {
        if(this->GetViewFlag(VIEWFLAG_ISACTIVE))
        {
            if(event.type == SDL_KEYUP)
            {
                switch(event.key.keysym.sym)
                {
                    case SDL_KeyCode::SDLK_SPACE:
                    {
                        GUIEventArgs e;
                        this->OnClick(e);
                        this->Click.Invoke(this,e);
                    }   break;
                }
            }
        }
        if(event.type == SDL_MOUSEBUTTONUP)
        {
            if(event.button.x >= myVisibleBounds.x && event.button.x < myVisibleBounds.x+myVisibleBounds.w && event.button.y >= myVisibleBounds.y && event.button.y < myVisibleBounds.y+myVisibleBounds.h)
            {
                this->GetWindow()->MakeActive(this);   
            }
        }
        if(event.type == SDL_FINGERUP)
        {
            auto pos = this->GetCordFromEvent(event);
            if(pos.first >= myVisibleBounds.x && pos.first < myVisibleBounds.x+myVisibleBounds.w && pos.second >= myVisibleBounds.y && pos.second < myVisibleBounds.y+myVisibleBounds.h)
            {
                this->GetWindow()->MakeActive(this);   
            }
        }
        return View::OnEvent(event,myBounds,myVisibleBounds);
    }
    void ButtonView::OnDraw(SDL_Renderer* renderer, SDL_Rect& r)
    {
        auto win = this->GetWindow();
        SDL_SetRenderDrawColor(renderer,win->palette.accent.r,win->palette.accent.g,win->palette.accent.b,win->palette.accent.a);
        SDL_RenderFillRect(renderer,&r);
        int textW;
        int textH;
        win->normal_font->CalculateSize(this->text,textW,textH);

        int x=(r.w/2)-(textW/2);
        int y=(r.h/2)-(textH/2);
        x+=r.x;
        y+=r.y;

        auto isHovering = this->GetViewFlag(VIEWFLAG_HOVER_STATE);
        auto isActive = this->GetViewFlag(VIEWFLAG_ISACTIVE);
        auto isMouseDown = this->GetViewFlag(VIEWFLAG_MOUSEDOWN_STATE);

        SDL_Color& color = win->palette.GetBorderColor(isHovering,isActive,isMouseDown);

        win->normal_font->Render(renderer,x,y,this->text,color);
        SDL_SetRenderDrawColor(renderer,color.r,color.g,color.b,color.a);

        SDL_Rect r2={.x=r.x,.y=r.y,.w=r.w,.h=r.h};
        for(size_t i=0;i < win->palette.borderSize; i++)
        {
            SDL_RenderDrawRect(renderer,&r2);
            r2.x++;
            r2.y++;
            r2.w-=2;
            r2.h-=2;
        }
        
    
    }
    
}
#endif