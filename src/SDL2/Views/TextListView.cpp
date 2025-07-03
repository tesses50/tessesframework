#if defined(TESSESFRAMEWORK_ENABLE_SDL2)

#include "TessesFramework/SDL2/Views/TextListView.hpp"

namespace Tesses::Framework::SDL2::Views
{
    TextListView::TextListView() : View()
    {
        this->firstIndex=0;
        this->selected=-1;
    }
    bool TextListView::OnEvent(SDL_Event& event, SDL_Rect& myBounds, SDL_Rect& visibleBounds)
    {
        auto win = this->GetWindow();
        auto item_height = win->normal_font->MaxHeight()+(win->palette.borderSize*2);
        auto no_items = (myBounds.h-(win->palette.borderSize*2)) / item_height;
        
        if(this->items.size() > no_items)
        {
            
            if(this->items.size() - this->firstIndex < no_items)
            {
                this->firstIndex = this->items.size() - no_items;
            }

        }
        else {
            this->firstIndex=0;

            no_items = (int)this->items.size();
        }
        if(event.type == SDL_KEYDOWN)
        {
            if(this->GetViewFlag(VIEWFLAG_ISACTIVE))
            {
                switch(event.key.keysym.sym)
                {
                    case SDLK_HOME:
                        this->selected=0;
                        this->firstIndex=0;
                        break;
                    case SDLK_END:
                        this->selected = (int)(this->items.size()-1);
                        this->firstIndex = (this->selected/no_items) * no_items;

                        break;
                    case SDLK_DOWN:
                        this->selected++;
                        if(this->selected < -1 || this->selected >= this->items.size())
                        {
                            this->selected=0;
                        }
                        this->firstIndex = (this->selected/no_items) * no_items;
                        break;
                    case SDLK_UP:
                        this->selected--;
                        if(this->selected < 0 || this->selected >= this->items.size())
                        {
                            this->selected = (int)(this->items.size()-1);
                        }

                        this->firstIndex = (this->selected/no_items) * no_items;
                       
                        break;
                }
            }
        }
        if(event.type == SDL_MOUSEBUTTONUP && event.button.x >= (visibleBounds.x+win->palette.borderSize) && event.button.x < (visibleBounds.x+visibleBounds.w)-(win->palette.borderSize*2) && event.button.y >= (visibleBounds.y+win->palette.borderSize) && event.button.y < (visibleBounds.y+visibleBounds.h)-(win->palette.borderSize*2))
        {
            win->MakeActive(this);
            auto myRealY=event.button.y - (myBounds.y+win->palette.borderSize);
            auto yThing = myRealY / item_height;

            if(yThing < no_items)
            {
                auto high= yThing+this->firstIndex;
                this->selected = (int)high;
            }

        }
        return View::OnEvent(event,myBounds,visibleBounds);
    }

    void TextListView::OnDraw(SDL_Renderer* renderer,SDL_Rect& rect)
    {
        auto win = this->GetWindow();
        auto item_height = win->normal_font->MaxHeight()+(win->palette.borderSize*2);
        auto no_items = (rect.h-(win->palette.borderSize*2)) / item_height;
        auto isHovering = this->GetViewFlag(VIEWFLAG_HOVER_STATE);
        auto isActive = this->GetViewFlag(VIEWFLAG_ISACTIVE);
        auto isMouseDown = this->GetViewFlag(VIEWFLAG_MOUSEDOWN_STATE);

        SDL_Color& color = win->palette.GetBorderColor(isHovering,isActive,isMouseDown);

        SDL_SetRenderDrawColor(renderer,color.r,color.g,color.b,color.a);

       
        
        
        
        if(this->items.size() > no_items)
        {
            
            if(this->items.size() - this->firstIndex < no_items)
            {
                this->firstIndex = this->items.size() - no_items;
            }
        }
        else {
            this->firstIndex=0;
            no_items = (int)this->items.size();
        }

        SDL_Rect r2={.x=rect.x,.y=rect.y,.w=rect.w,.h=rect.h};
        for(size_t i=0;i < win->palette.borderSize; i++)
        {
            SDL_RenderDrawRect(renderer,&r2);
            r2.x++;
            r2.y++;
            r2.w-=2;
            r2.h-=2;
        }

        for(int i = 0; i < no_items; i++)
        {
            int realI = i+(int)firstIndex;
            if(realI == this->selected)
            {
               SDL_SetRenderDrawColor(renderer,win->palette.accent.r,win->palette.accent.g,win->palette.accent.b,win->palette.accent.a);
                SDL_Rect r2={.x=rect.x+win->palette.borderSize,.y=rect.y+win->palette.borderSize+(item_height*i),.w=rect.w-(win->palette.borderSize*2),.h=item_height};
                SDL_RenderFillRect(renderer,&r2);
                win->normal_font->Render(renderer,rect.x+(win->palette.borderSize*3),(rect.y+(win->palette.borderSize*3))+(item_height*i),this->items[realI],color);                
            }
            else {
                win->normal_font->Render(renderer,rect.x+(win->palette.borderSize*3),(rect.y+(win->palette.borderSize*3))+(item_height*i),this->items[realI],win->palette.accent);
            }
        }
    }
}
#endif