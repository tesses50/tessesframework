#if defined(TESSESFRAMEWORK_ENABLE_SDL2)

#include "TessesFramework/SDL2/Views/VScrollView.hpp"

namespace Tesses::Framework::SDL2::Views
{
    std::pair<int,int> VScrollView::PreferedMinSize()
    {
        return std::pair<int,int>(32,-1);
    }
    void VScrollView::OnValueChanged(GUIEventArgs& e)
    {

    }
    VScrollView::VScrollView() : VScrollView(0, 0, 100)
    {

    }
    VScrollView::VScrollView(uint64_t value, uint64_t min, uint64_t max,uint64_t step) : View()
    {
        this->value = value;
        this->min = min;
        this->max = max;

        this->step=step;
    }
    void VScrollView::OnDraw(SDL_Renderer* renderer, SDL_Rect& r)
    {
        auto win = this->GetWindow();
        auto topIsHovering = this->GetViewFlag(VIEWFLAG_HOVER_B1STATE);
        auto isActive = this->GetViewFlag(VIEWFLAG_ISACTIVE);
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
            SDL_RenderDrawLine(renderer,r.x,r.y+i,r.x+r.w,r.y+i);
            SDL_RenderDrawLine(renderer,r.x+i,r.y,r.x+i,r.y+32);
            SDL_RenderDrawLine(renderer,r.x+r.w-(1+i),r.y,r.x+r.w-(1+i),r.y+32);
        }
        SDL_SetRenderDrawColor(renderer,topMiddleColor.r,topMiddleColor.g,topMiddleColor.b,topMiddleColor.a);
        for(int i = 0; i < win->palette.borderSize; i++)
        {
            SDL_RenderDrawLine(renderer,r.x,(r.y+32)-i,r.x+r.w,(r.y+32)-i);
        }
        SDL_SetRenderDrawColor(renderer,middleColor.r,middleColor.g,middleColor.b,middleColor.a);

        for(int i = 0; i < win->palette.borderSize; i++)
        {
            SDL_RenderDrawLine(renderer,r.x+i,r.y+33,r.x+i,r.y+(r.h-33));
            SDL_RenderDrawLine(renderer,r.x+r.w-(1+i),r.y+33,r.x+r.w-(1+i),r.y+(r.h-33));
        }
        SDL_SetRenderDrawColor(renderer,bottomColor.r,bottomColor.g,bottomColor.b,bottomColor.a);

        for(int i = 0; i < win->palette.borderSize; i++)
        {
            SDL_RenderDrawLine(renderer,r.x,(r.y+r.h)-(i+1),r.x+r.w,(r.y+r.h)-(i+1));
            SDL_RenderDrawLine(renderer,r.x+i,r.y+(r.h-32),r.x+i,r.y+r.h);
            SDL_RenderDrawLine(renderer,r.x+r.w-(1+i),r.y+(r.h-32),r.x+r.w-(1+i),r.y+r.h);
        }
        SDL_SetRenderDrawColor(renderer,bottomMiddleColor.r,bottomMiddleColor.g,bottomMiddleColor.b,bottomMiddleColor.a);
        for(int i = 0; i < win->palette.borderSize; i++)
        {
            SDL_RenderDrawLine(renderer,r.x,(r.y+(r.h-32))+i,r.x+r.w,(r.y+(r.h-32))+i);
        }
        


        SDL_SetRenderDrawColor(renderer,win->palette.accent.r,win->palette.accent.g,win->palette.accent.b,win->palette.accent.a);

        for(int i = 0; i < 4; i++)
        {
            int x1 = 15;
            int y1 = 8 + i;

            int x2 = 9;
            int y2 = 18 + i;

            SDL_RenderDrawLine(renderer,x1+r.x,y1+r.y,x2+r.x,y2+r.y);

            int x3 = 31-x1;
            int x4 = 31-x2;
            SDL_RenderDrawLine(renderer,x3+r.x,y1+r.y,x4+r.x,y2+r.y);

            int y3 = 31-y1;
            int y4 = 31-y2;

            SDL_RenderDrawLine(renderer,x1+r.x,y3+r.y+(r.h-32),x2+r.x,y4+r.y+(r.h-32));
            SDL_RenderDrawLine(renderer,x3+r.x,y3+r.y+(r.h-32),x4+r.x,y4+r.y+(r.h-32));
        }

        uint64_t height = (uint64_t)r.h - 66;
        uint64_t scrollSize = 4;
        uint64_t dif = max-min;
        if(dif > 0)
            scrollSize = height / dif;
        
        if(scrollSize < 4) scrollSize=4;

        
        double scroll = 0;
        if(dif > 0)
        scroll = (double)(value-min) / (double)dif;
        
        uint64_t scrollY = scroll * (height-scrollSize);



        if(scrollY > height-scrollSize) scrollY = height-scrollSize;
        SDL_Rect r2={.x=r.x+win->palette.borderSize,.y=(int)scrollY+r.y+32,.w = r.w-(win->palette.borderSize*2),.h=(int)scrollSize};
        SDL_RenderFillRect(renderer,&r2);
    }
    bool VScrollView::OnEvent(SDL_Event& event, SDL_Rect& myBounds, SDL_Rect& visibleBounds)
    {
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
                    value = min;
                }
                else if(y >= myBounds.h-32)
                {
                    value=max;
                }
                else {
                    uint64_t height = (uint64_t)myBounds.h - 66;
                        uint64_t y2 = y-33;

                        double off = (double)y2 / (double)height;

                        value = round((max-min)*off)+min;
                        GUIEventArgs cea2;
                this->ValueChanged.Invoke(this,cea2);
                }
                        
            }
            return false;
        }
        if(event.type == SDL_MOUSEBUTTONDOWN)
        {
            if(event.button.x >= visibleBounds.x && event.button.x < visibleBounds.x+visibleBounds.w && event.button.y >= visibleBounds.y && event.button.y < visibleBounds.y+visibleBounds.h)
            {
                GUIMouseButtonEventArgs cea;
                cea.button = (int)event.button.button;
                cea.x = event.button.x - myBounds.x;
                cea.y = event.button.y - myBounds.y;
                cea.which = event.button.which;
                if(cea.y <= 32)
                {
                    //up btn
                    this->SetViewFlag(VIEWFLAG_MOUSEDOWN_B1STATE,true);
                }
                else if(cea.y >= myBounds.h-32)
                {
                    //down btn
                    this->SetViewFlag(VIEWFLAG_MOUSEDOWN_B2STATE,true);
                }
                else {
                    this->SetViewFlag(VIEWFLAG_MOUSEDOWN_STATE,true);
                    
                    uint64_t height = (uint64_t)myBounds.h - 66;
                        uint64_t y2 = cea.y-33;

                        double off = (double)y2 / (double)height;

                        value = round((max-min)*off)+min;
 GUIEventArgs cea2;
                this->ValueChanged.Invoke(this,cea2);
                }
                OnMouseDown(cea);
                this->MouseDown.Invoke(this,cea);

                

                return false;
            }
        }
        else if(event.type == SDL_MOUSEBUTTONUP)
        {
            if(this->GetViewFlag(VIEWFLAG_MOUSEDOWN_STATE))
            {
                GUIMouseButtonEventArgs cea;
                cea.button = (int)event.button.button;
                cea.x = event.button.x - myBounds.x;
                cea.y = event.button.y - myBounds.y;
                cea.which = event.button.which;
                this->SetViewFlag(VIEWFLAG_MOUSEDOWN_STATE,false);
                OnMouseUp(cea);
                this->MouseUp.Invoke(this,cea);
                GUIEventArgs cea2;
                OnClick(cea2);
                this->Click.Invoke(this,cea2);
                return true;
            }
            if(this->GetViewFlag(VIEWFLAG_MOUSEDOWN_B1STATE))
            {
                GUIMouseButtonEventArgs cea;
                cea.button = (int)event.button.button;
                cea.x = event.button.x - myBounds.x;
                cea.y = event.button.y - myBounds.y;
                cea.which = event.button.which;
                this->SetViewFlag(VIEWFLAG_MOUSEDOWN_B1STATE,false);
                OnMouseUp(cea);
                this->MouseUp.Invoke(this,cea);
                GUIEventArgs cea2;
                OnClick(cea2);
                this->Click.Invoke(this,cea2);
                this->value -= step;
                if(this->value < min) this->value=min;
                if(this->value >= max) this->value=min;
                OnValueChanged(cea2);
                this->ValueChanged.Invoke(this,cea2);
                return true;
            }
            if(this->GetViewFlag(VIEWFLAG_MOUSEDOWN_B2STATE))
            {
                GUIMouseButtonEventArgs cea;
                cea.button = (int)event.button.button;
                cea.x = event.button.x - myBounds.x;
                cea.y = event.button.y - myBounds.y;
                cea.which = event.button.which;
                this->SetViewFlag(VIEWFLAG_MOUSEDOWN_B2STATE,false);
                OnMouseUp(cea);
                this->MouseUp.Invoke(this,cea);
                GUIEventArgs cea2;
                OnClick(cea2);
                this->Click.Invoke(this,cea2);
                this->value += step;
                if(this->value > max) this->value=max;
                OnValueChanged(cea2);
                this->ValueChanged.Invoke(this,cea2);
                return true;
            }

            return false;
        }
        return View::OnEvent(event,myBounds,visibleBounds);
    }
      
}

#endif