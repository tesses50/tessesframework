#if defined(TESSESFRAMEWORK_ENABLE_SDL2)

#include "TessesFramework/SDL2/Views/ScrollableTextListView.hpp"

namespace Tesses::Framework::SDL2::Views
{
    ScrollableTextListView::ScrollableTextListView() : View()
    {
        this->firstIndex=0;
        this->selected=-1;
    }
    bool ScrollableTextListView::OnEvent(SDL_Event& event, SDL_Rect& myBounds, SDL_Rect& visibleBounds)
    {
        auto win = this->GetWindow();
        auto item_height = win->normal_font->MaxHeight()+(win->palette.borderSize*2);
        auto no_items = (myBounds.h-(win->palette.borderSize*2)) / item_height;
        auto max_items = no_items;

        if(this->firstIndex + no_items > this->items.size())
        {

            no_items = this->items.size()-this->firstIndex;
        }
        
        
        if(event.type == SDL_KEYDOWN)
        {
            if(this->GetViewFlag(VIEWFLAG_ISACTIVE))
            {
                switch(event.key.keysym.sym)
                {
                    case SDLK_RETURN:
                    {

                        GUIEventArgs e;
                        if(this->selected > -1)
                        this->ValueChanged.Invoke(this,e);
                    }
                        break;
                    case SDLK_HOME:
                        this->selected=0;
                        this->firstIndex=0;
                        break;
                    case SDLK_END:
                        this->selected = (int)(this->items.size()-1);

                        this->firstIndex= this->selected-(this->selected % max_items);

                        break;
                    case SDLK_DOWN:
                        this->selected++;
                        if(this->selected < -1 || this->selected >= this->items.size())
                        {
                            this->selected=0;
                        }
                        this->firstIndex= this->selected-(this->selected % max_items);
                        
                        break;
                    case SDLK_UP:
                        this->selected--;
                        if(this->selected < 0 || this->selected >= this->items.size())
                        {
                            this->selected = (int)(this->items.size()-1);
                        }
                        
                        this->firstIndex= this->selected-(this->selected % max_items);
                       
                        break;
                }
            }
        }
        
        if(event.type == SDL_MOUSEMOTION)
        {
            bool inside = event.motion.x >= visibleBounds.x && event.motion.x < visibleBounds.x+visibleBounds.w && event.motion.y >= visibleBounds.y && event.motion.y < visibleBounds.y+visibleBounds.h;
            bool hoverFlag = this->GetViewFlag(VIEWFLAG_HOVER_STATE) || this->GetViewFlag(VIEWFLAG_HOVER_B1STATE) || this->GetViewFlag(VIEWFLAG_HOVER_B2STATE);
            if(inside && !hoverFlag)
            {
                int y = event.motion.y - myBounds.y;
                if(y <= 32)
                {
                    //up btn
                    this->SetViewFlag(VIEWFLAG_HOVER_B1STATE,true);
                    this->SetViewFlag(VIEWFLAG_HOVER_B2STATE,false);
                    this->SetViewFlag(VIEWFLAG_HOVER_STATE,false);
                }
                else if(y >= myBounds.h-32)
                {
                    //down btn
                    this->SetViewFlag(VIEWFLAG_HOVER_B1STATE,false);
                    this->SetViewFlag(VIEWFLAG_HOVER_B2STATE,true);
                    this->SetViewFlag(VIEWFLAG_HOVER_STATE,false);
                }
                else {
                    this->SetViewFlag(VIEWFLAG_HOVER_B1STATE,false);
                    this->SetViewFlag(VIEWFLAG_HOVER_B2STATE,false);
                    this->SetViewFlag(VIEWFLAG_HOVER_STATE,true);
                }

                GUIEventArgs e;
                this->OnEnter(e);
                this->Enter.Invoke(this,e);
            }
            else if(!inside && hoverFlag)
            {
                this->SetViewFlag(VIEWFLAG_HOVER_STATE,false);
                GUIEventArgs e;
                this->OnLeave(e);
                this->Leave.Invoke(this,e);
            } else if(inside) {
                 int y = event.motion.y - myBounds.y;
                if(y <= 32)
                {
                    //up btn
                    this->SetViewFlag(VIEWFLAG_HOVER_B1STATE,true);
                    this->SetViewFlag(VIEWFLAG_HOVER_B2STATE,false);
                    this->SetViewFlag(VIEWFLAG_HOVER_STATE,false);
                }
                else if(y >= myBounds.h-32)
                {
                    //down btn
                    this->SetViewFlag(VIEWFLAG_HOVER_B1STATE,false);
                    this->SetViewFlag(VIEWFLAG_HOVER_B2STATE,true);
                    this->SetViewFlag(VIEWFLAG_HOVER_STATE,false);
                }
                else {
                    this->SetViewFlag(VIEWFLAG_HOVER_B1STATE,false);
                    this->SetViewFlag(VIEWFLAG_HOVER_B2STATE,false);
                    this->SetViewFlag(VIEWFLAG_HOVER_STATE,true);

                    
                }
                
            }

            if(this->GetViewFlag(VIEWFLAG_MOUSEDOWN_STATE))
            {
                int y = event.motion.y - myBounds.y;
                if(y <= 32)
                {
                    this->firstIndex = 0;
                }
                else if(y >= myBounds.h-32)
                {
                    uint64_t count = max_items == 0 ? 0 :  this->items.size() / max_items;
        
                    if((items.size() % max_items) == 0) count--;

                    this->firstIndex = count*max_items;
                }
                else {
                    uint64_t height = (uint64_t)myBounds.h - 66;
                    uint64_t y2 = y-33;

                    double off = (double)y2 / (double)height;

                    
                    uint64_t count = max_items == 0 ? 0 :  this->items.size() / max_items;
                    if((items.size() % max_items) == 0) count--;

                    this->firstIndex = round(off*count) * max_items;
                        
                }
                        
            }
            return false;
        }
        if(event.type == SDL_MOUSEBUTTONDOWN)
        {
            if(event.button.x >= visibleBounds.x && event.button.x < visibleBounds.x+visibleBounds.w && event.button.y >= visibleBounds.y && event.button.y < visibleBounds.y+visibleBounds.h)
            {
                int x=  event.button.x - myBounds.x;
                int y = event.button.y - myBounds.y;
                
                if(x >= myBounds.w-32)
                {
               
                if(y <= 32)
                {
                    //up btn
                    this->SetViewFlag(VIEWFLAG_MOUSEDOWN_B1STATE,true);
                }
                else if(y >= myBounds.h-32)
                {
                    //down btn
                    this->SetViewFlag(VIEWFLAG_MOUSEDOWN_B2STATE,true);
                }
                else {
                    this->SetViewFlag(VIEWFLAG_MOUSEDOWN_STATE,true);
                    
                    uint64_t height = (uint64_t)myBounds.h - 66;
                        uint64_t y2 = y-33;

                        double off = (double)y2 / (double)height;

                        uint64_t count = max_items == 0 ? 0 :  this->items.size() / max_items;
                        if((items.size() % max_items) == 0) count--;

                        this->firstIndex = round(off*count) * max_items;
                        //value = round((max-min)*off)+min;
                        
                    }
                }
                

                return false;
            }
        }
        else if(event.type == SDL_MOUSEBUTTONUP)
        {
            int x=  event.button.x - myBounds.x;
            if(x < myBounds.w-32  && event.button.x >= (visibleBounds.x+win->palette.borderSize)  && event.button.y >= (visibleBounds.y+win->palette.borderSize) && event.button.y < (visibleBounds.y+visibleBounds.h)-(win->palette.borderSize*2))
            {
                win->MakeActive(this);
                auto myRealY=event.button.y - (myBounds.y+win->palette.borderSize);
                auto yThing = myRealY / item_height;

                if(yThing < no_items)
                {
                    auto high= yThing+this->firstIndex;
                    this->selected = (int)high;
                    GUIEventArgs e;
                    this->ValueChanged.Invoke(this,e);
                }

            }
            if(this->GetViewFlag(VIEWFLAG_MOUSEDOWN_STATE))
            {

                win->MakeActive(this);
                this->SetViewFlag(VIEWFLAG_MOUSEDOWN_STATE,false);
                
                return true;
            }
            if(this->GetViewFlag(VIEWFLAG_MOUSEDOWN_B1STATE))
            {
                
                win->MakeActive(this);
                this->SetViewFlag(VIEWFLAG_MOUSEDOWN_B1STATE,false);
                
                //this->value -= step;
                //if(this->value < min) this->value=min;
                //if(this->value >= max) this->value=min;
                //OnValueChanged(cea2);
                //this->ValueChanged.Invoke(this,cea2);

                uint64_t count = max_items == 0 ? 0 :  this->items.size() / max_items;
        
                if((items.size() % max_items) == 0) count--;
                
                auto c = max_items == 0 ? 0 : (this->firstIndex / max_items);
                c--;
                if(c > count) c = 0;
                this->firstIndex = c * max_items;

                return true;
            }
            if(this->GetViewFlag(VIEWFLAG_MOUSEDOWN_B2STATE))
            {
               
                win->MakeActive(this);
                this->SetViewFlag(VIEWFLAG_MOUSEDOWN_B2STATE,false);
                
                uint64_t count = max_items == 0 ? 0 :  this->items.size() / max_items;
        
                if((items.size() % max_items) == 0) count--;
                
                auto c = max_items == 0 ? 0 : (this->firstIndex / max_items);
                c++;
                if(c > count) c = count;
                this->firstIndex = c * max_items;
                //this->value += step;
                //if(this->value > max) this->value=max;
                //OnValueChanged(cea2);
                //this->ValueChanged.Invoke(this,cea2);
                return true;
            }
            return false;
        }
        
        
        
        
        
        
        
        return View::OnEvent(event,myBounds,visibleBounds);
    }

    void ScrollableTextListView::OnDraw(SDL_Renderer* renderer,SDL_Rect& rect)
    {
        auto win = this->GetWindow();
        auto item_height = win->normal_font->MaxHeight()+(win->palette.borderSize*2);
        auto no_items = (rect.h-(win->palette.borderSize*2)) / item_height;
        auto max_items = no_items;
        auto isHovering = this->GetViewFlag(VIEWFLAG_HOVER_STATE);
        auto isActive = this->GetViewFlag(VIEWFLAG_ISACTIVE);
        auto isMouseDown = this->GetViewFlag(VIEWFLAG_MOUSEDOWN_STATE);

        SDL_Color& color = win->palette.GetBorderColor(isHovering,isActive,isMouseDown);

        SDL_SetRenderDrawColor(renderer,color.r,color.g,color.b,color.a);

       
        
        
        
        if(this->firstIndex + no_items > this->items.size())
        {

            no_items = this->items.size()-this->firstIndex;
        }

        SDL_Rect _r2={.x=rect.x,.y=rect.y,.w=rect.w-32,.h=rect.h};
        for(size_t i=0;i < win->palette.borderSize; i++)
        {
            SDL_RenderDrawRect(renderer,&_r2);
            _r2.x++;
            _r2.y++;
            _r2.w-=2;
            _r2.h-=2;
        }

        for(int i = 0; i < no_items; i++)
        {
            int realI = i+(int)firstIndex;
            if(realI == this->selected)
            {
               SDL_SetRenderDrawColor(renderer,win->palette.accent.r,win->palette.accent.g,win->palette.accent.b,win->palette.accent.a);
                SDL_Rect r2={.x=rect.x+win->palette.borderSize,.y=rect.y+win->palette.borderSize+(item_height*i),.w=rect.w-(win->palette.borderSize*2)-32,.h=item_height};
                SDL_RenderFillRect(renderer,&r2);
                win->normal_font->Render(renderer,rect.x+(win->palette.borderSize*3),(rect.y+(win->palette.borderSize*3))+(item_height*i),this->items[realI],color);                
            }
            else {
                win->normal_font->Render(renderer,rect.x+(win->palette.borderSize*3),(rect.y+(win->palette.borderSize*3))+(item_height*i),this->items[realI],win->palette.accent);
            }
        }
        
        auto scrollX = (rect.w-32)+rect.x;
        auto topIsHovering = this->GetViewFlag(VIEWFLAG_HOVER_B1STATE);
        
        auto topIsMouseDown = this->GetViewFlag(VIEWFLAG_MOUSEDOWN_B1STATE);

        auto middleIsHovering = this->GetViewFlag(VIEWFLAG_HOVER_STATE);
       
        auto middleIsMouseDown = this->GetViewFlag(VIEWFLAG_MOUSEDOWN_STATE);


        auto bottomIsHovering = this->GetViewFlag(VIEWFLAG_HOVER_B2STATE);
       
        auto bottomIsMouseDown = this->GetViewFlag(VIEWFLAG_MOUSEDOWN_B2STATE);

        SDL_Color& topcolor = win->palette.GetBorderColor(topIsHovering,isActive,topIsMouseDown);
        SDL_Color& topMiddleColor = win->palette.GetBorderColor(topIsHovering||middleIsHovering,isActive,topIsMouseDown||middleIsMouseDown);
        SDL_Color& middleColor = win->palette.GetBorderColor(middleIsHovering,isActive,middleIsMouseDown);
        SDL_Color& bottomMiddleColor = win->palette.GetBorderColor(bottomIsHovering||middleIsHovering,isActive,bottomIsMouseDown||middleIsMouseDown);
        SDL_Color& bottomColor = win->palette.GetBorderColor(bottomIsHovering,isActive,bottomIsMouseDown);

        SDL_SetRenderDrawColor(renderer,topcolor.r,topcolor.g,topcolor.b,topcolor.a);

        for(int i = 0; i < win->palette.borderSize; i++)
        {
            SDL_RenderDrawLine(renderer,scrollX,rect.y+i,scrollX+32,rect.y+i);
            SDL_RenderDrawLine(renderer,scrollX+i,rect.y,scrollX+i,rect.y+32);
            SDL_RenderDrawLine(renderer,scrollX+32-(1+i),rect.y,scrollX+32-(1+i),rect.y+32);
        }
        SDL_SetRenderDrawColor(renderer,topMiddleColor.r,topMiddleColor.g,topMiddleColor.b,topMiddleColor.a);
        for(int i = 0; i < win->palette.borderSize; i++)
        {
            SDL_RenderDrawLine(renderer,scrollX,(rect.y+32)-i,scrollX+32,(rect.y+32)-i);
        }
        SDL_SetRenderDrawColor(renderer,middleColor.r,middleColor.g,middleColor.b,middleColor.a);

        for(int i = 0; i < win->palette.borderSize; i++)
        {
            SDL_RenderDrawLine(renderer,scrollX+i,rect.y+33,scrollX+i,rect.y+(rect.h-33));
            SDL_RenderDrawLine(renderer,scrollX+32-(1+i),rect.y+33,scrollX+32-(1+i),rect.y+(rect.h-33));
        }
        SDL_SetRenderDrawColor(renderer,bottomColor.r,bottomColor.g,bottomColor.b,bottomColor.a);

        for(int i = 0; i < win->palette.borderSize; i++)
        {
            SDL_RenderDrawLine(renderer,scrollX,(rect.y+rect.h)-(i+1),scrollX+32,(rect.y+rect.h)-(i+1));
            SDL_RenderDrawLine(renderer,scrollX+i,rect.y+(rect.h-32),scrollX+i,rect.y+rect.h);
            SDL_RenderDrawLine(renderer,scrollX+32-(1+i),rect.y+(rect.h-32),scrollX+32-(1+i),rect.y+rect.h);
        }
        SDL_SetRenderDrawColor(renderer,bottomMiddleColor.r,bottomMiddleColor.g,bottomMiddleColor.b,bottomMiddleColor.a);
        for(int i = 0; i < win->palette.borderSize; i++)
        {
            SDL_RenderDrawLine(renderer,scrollX,(rect.y+(rect.h-32))+i,scrollX+32,(rect.y+(rect.h-32))+i);
        }
        


        SDL_SetRenderDrawColor(renderer,win->palette.accent.r,win->palette.accent.g,win->palette.accent.b,win->palette.accent.a);

        for(int i = 0; i < 4; i++)
        {
            int x1 = 15;
            int y1 = 8 + i;

            int x2 = 9;
            int y2 = 18 + i;

            SDL_RenderDrawLine(renderer,x1+scrollX,y1+rect.y,x2+scrollX,y2+rect.y);

            int x3 = 31-x1;
            int x4 = 31-x2;
            SDL_RenderDrawLine(renderer,x3+scrollX,y1+rect.y,x4+scrollX,y2+rect.y);

            int y3 = 31-y1;
            int y4 = 31-y2;

            SDL_RenderDrawLine(renderer,x1+scrollX,y3+rect.y+(rect.h-32),x2+scrollX,y4+rect.y+(rect.h-32));
            SDL_RenderDrawLine(renderer,x3+scrollX,y3+rect.y+(rect.h-32),x4+scrollX,y4+rect.y+(rect.h-32));
        }

        uint64_t height = (uint64_t)rect.h - 66;
        uint64_t scrollSize = 4;
        uint64_t count = max_items == 0 ? 0 :  this->items.size() / max_items;
        
        if((items.size() % max_items) == 0) count--;
      
        if(count > 0)
            scrollSize = height / count;
        
        if(scrollSize < 4) scrollSize=4;

        
        double scroll = 0;
        if(count > 0 && max_items > 0)
        scroll = (double)(this->firstIndex / max_items) / (double)count;
        
        uint64_t scrollY = scroll * (height-scrollSize);



        if(scrollY > height-scrollSize) scrollY = height-scrollSize;
        SDL_Rect r2={.x=scrollX+win->palette.borderSize,.y=(int)scrollY+rect.y+32,.w = 32-(win->palette.borderSize*2),.h=(int)scrollSize};
        SDL_RenderFillRect(renderer,&r2);
  
    }
}
#endif