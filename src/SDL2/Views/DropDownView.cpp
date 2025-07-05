#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include "TessesFramework/SDL2/Views/DropDownView.hpp"

namespace Tesses::Framework::SDL2::Views {
    
    void DropDownView::OnDraw(SDL_Renderer* renderer, SDL_Rect& r)
    {
        std::string text = this->text;
        if(this->listView.selected > -1 && this->listView.selected < this->listView.items.size())
        {
            text = this->listView.items[this->listView.selected];
        }
        auto win = this->GetWindow();
       
        int textW;
        int textH;
        win->normal_font->CalculateSize(text,textW,textH);

        int x=win->palette.borderSize*2;
        int y=(r.h/2)-(textH/2);
        x+=r.x;
        y+=r.y;

        auto isHovering = this->GetViewFlag(VIEWFLAG_HOVER_STATE);
        auto isActive = this->GetViewFlag(VIEWFLAG_ISACTIVE);
        auto isMouseDown = this->GetViewFlag(VIEWFLAG_MOUSEDOWN_STATE);

        SDL_Color& color = win->palette.GetBorderColor(isHovering,isActive,isMouseDown);
        
        win->normal_font->Render(renderer,x,y,text,win->palette.accent);
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
    bool DropDownView::OnEvent(SDL_Event& event, SDL_Rect& myBounds, SDL_Rect& visibleBounds)
    {
        auto win = this->GetWindow();
        if(event.type == SDL_MOUSEBUTTONUP)
        {
            if(event.button.x >= visibleBounds.x && event.button.x < visibleBounds.x+visibleBounds.w && event.button.y >= visibleBounds.y && event.button.y < visibleBounds.y+visibleBounds.h)
            {
                win->MakeActive(this);
                this->popup.bounds.x = myBounds.x;
                this->popup.bounds.y = myBounds.y+myBounds.h;
                this->popup.bounds.w = myBounds.w;
                this->popup.bounds.h = 150;
                auto pu = &this->popup;
                if(!this->hasSet) {
                    
                    this->listView.ValueChanged += std::make_shared<FunctionalEvent<View*,GUIEventArgs&>>([pu](View* view,GUIEventArgs& args)->void {
                        pu->Close();
                    });
                    
                    this->popup.SetView(&this->listView,false);
                    this->hasSet=true;
                }
                win->ShowPopup(pu);
            }
        }
        else if(event.type == SDL_KEYDOWN && this->GetViewFlag(VIEWFLAG_ISACTIVE))
        {
            switch(event.key.keysym.sym)
            {
                case SDLK_UP:
                {
                    this->listView.selected--;
                    if(this->listView.selected < 0 || this->listView.selected >= this->listView.items.size())
                    {
                        this->listView.selected = (int)(this->listView.items.size()-1);
                    }
                }
                break;
                case SDLK_DOWN:
                {
                    this->listView.selected++;
                    if(this->listView.selected < -1 || this->listView.selected >= this->listView.items.size())
                    {
                        this->listView.selected=0;
                    }
                }
                break;
            }
        }
        return false;
    }
    DropDownView::DropDownView() : View("--PLEASE SELECT ONE--")
    {
        this->hasSet=false;
        
    }
    std::vector<std::string>& DropDownView::GetItems()
    {
        return this->listView.items;
    }
    void DropDownView::SetIndex(int index)
    {
        this->listView.selected = index;
    }
    int DropDownView::GetIndex()
    {
        return this->listView.selected;
    }
    
}
#endif