#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include "TessesFramework/SDL2/Views/CheckView.hpp"

namespace Tesses::Framework::SDL2::Views
{
    void CheckView::OnCheckChanged(GUIEventArgs& event)
    {
        
    }
    void CheckView::OnDraw(SDL_Renderer* renderer, SDL_Rect& r)
    {
        auto win = this->GetWindow();

        const int checkSz=32;

        SDL_Rect checkBoxRect={.x=r.x,.y=r.y+((r.h/2)-(checkSz/2)),.w=checkSz,.h=checkSz};

        int textW;
        int textH;
        win->normal_font->CalculateSize(this->text,textW,textH);

        //we only need the y

        int x = r.x+checkSz+8;
        int y = r.y+((r.h/2)-(textH/2));

        win->normal_font->Render(renderer,x,y,text,win->palette.accent);



        if(this->GetViewFlag(VIEWFLAG_CHECKED))
        {
            SDL_SetRenderDrawColor(renderer,win->palette.accent.r,win->palette.accent.g,win->palette.accent.b,win->palette.accent.a);
       
            //x=0,y=0.5, x=0.5, y=1

            int x1=checkBoxRect.x+4;
            int y1=checkBoxRect.y+4+((checkSz-8)/2);
            int x2=checkBoxRect.x+4+((checkSz-8)/2);
            int y2=checkBoxRect.y+4+(checkSz-8);

            int x3=checkBoxRect.x+4+(checkSz-8);
            int y3=checkBoxRect.y+4;

            for(int i = 0; i < 4; i++)
            {
                SDL_RenderDrawLine(renderer,x1,y1-i,x2,y2-i);
                SDL_RenderDrawLine(renderer,x2-i,y2,x3-i,y3);
            }
        }

        auto isHovering = this->GetViewFlag(VIEWFLAG_HOVER_STATE);
        auto isActive = this->GetViewFlag(VIEWFLAG_ISACTIVE);
        auto isMouseDown = this->GetViewFlag(VIEWFLAG_MOUSEDOWN_STATE);

        SDL_Color& color = win->palette.GetBorderColor(isHovering,isActive,isMouseDown);

        SDL_SetRenderDrawColor(renderer,color.r,color.g,color.b,color.a);

        for(int i = 0; i < 4; i++)
        {
            SDL_RenderDrawRect(renderer,&checkBoxRect);
            checkBoxRect.x++;
            checkBoxRect.y++;
            checkBoxRect.w-=2;
            checkBoxRect.h-=2;
        }
    }
    bool CheckView::OnEvent(SDL_Event& event, SDL_Rect& myBounds, SDL_Rect& visibleBounds)
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

                        this->SetViewFlag(VIEWFLAG_CHECKED,!this->GetViewFlag(VIEWFLAG_CHECKED));
                        this->OnClick(e);
                        this->Click.Invoke(this,e);
                        this->OnCheckChanged(e);
                        this->CheckChanged.Invoke(this,e);
                    }   break;
                }
            }
        }
        if(event.type == SDL_FINGERUP)
        {
            auto pos = this->GetCordFromEvent(event);
            if(pos.first >= visibleBounds.x && pos.first < visibleBounds.x+visibleBounds.w && pos.second >= visibleBounds.y && pos.second < visibleBounds.y+visibleBounds.h)
            {
                this->GetWindow()->MakeActive(this);   
                this->SetViewFlag(VIEWFLAG_CHECKED,!this->GetViewFlag(VIEWFLAG_CHECKED));
                GUIEventArgs e;
                this->OnCheckChanged(e);
                this->CheckChanged.Invoke(this,e);   
            }
        }
        if(event.type == SDL_MOUSEBUTTONUP)
        {
            if(event.button.x >= visibleBounds.x && event.button.x < visibleBounds.x+visibleBounds.w && event.button.y >= visibleBounds.y && event.button.y < visibleBounds.y+visibleBounds.h)
            {
                this->GetWindow()->MakeActive(this);   
                this->SetViewFlag(VIEWFLAG_CHECKED,!this->GetViewFlag(VIEWFLAG_CHECKED));
                GUIEventArgs e;
                this->OnCheckChanged(e);
                this->CheckChanged.Invoke(this,e);
            }
        }
        return View::OnEvent(event,myBounds,visibleBounds);
    }
        
    CheckView::CheckView(bool checked, std::string label) : View(label)
    {
        this->SetViewFlag(VIEWFLAG_CHECKED,checked);
    }
    CheckView::CheckView() : View()
    {
        this->SetViewFlag(VIEWFLAG_CHECKED,false);
    }

    bool CheckView::GetChecked()
    {
        return this->GetViewFlag(VIEWFLAG_CHECKED);
    }
    void CheckView::SetChecked(bool value)
    {
        this->SetViewFlag(VIEWFLAG_CHECKED,true);
    }
}
#endif