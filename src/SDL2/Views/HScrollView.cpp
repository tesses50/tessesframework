#if defined(TESSESFRAMEWORK_ENABLE_SDL2)

#include "TessesFramework/SDL2/Views/HScrollView.hpp"

namespace Tesses::Framework::SDL2::Views
{
    std::pair<int,int> HScrollView::PreferedMinSize()
    {
        return std::pair<int,int>(-1,32);
    }
    void HScrollView::OnValueChanged(GUIEventArgs& e)
    {

    }
    HScrollView::HScrollView() : HScrollView(0, 0, 100)
    {

    }
    HScrollView::HScrollView(uint64_t value, uint64_t min, uint64_t max,uint64_t step) : View()
    {
        this->value = value;
        this->min = min;
        this->max = max;

        this->step=step;
    }
    void HScrollView::OnDraw(SDL_Renderer* renderer, SDL_Rect& r)
    {
        auto win = this->GetWindow();
        auto leftIsHovering = this->GetViewFlag(VIEWFLAG_HOVER_B1STATE);
        auto isActive = this->GetViewFlag(VIEWFLAG_ISACTIVE);
        auto leftIsMouseDown = this->GetViewFlag(VIEWFLAG_MOUSEDOWN_B1STATE);

        auto middleIsHovering = this->GetViewFlag(VIEWFLAG_HOVER_STATE);
       
        auto middleIsMouseDown = this->GetViewFlag(VIEWFLAG_MOUSEDOWN_STATE);


        auto rightIsHovering = this->GetViewFlag(VIEWFLAG_HOVER_B2STATE);
       
        auto rightIsMouseDown = this->GetViewFlag(VIEWFLAG_MOUSEDOWN_B2STATE);

        SDL_Color& leftColor = win->palette.GetBorderColor(leftIsHovering,isActive,leftIsMouseDown);
        SDL_Color& leftMiddleColor = win->palette.GetBorderColor(leftIsHovering||middleIsHovering,isActive,leftIsMouseDown||middleIsMouseDown);
        SDL_Color& middleColor = win->palette.GetBorderColor(middleIsHovering,isActive,middleIsMouseDown);
        SDL_Color& rightMiddleColor = win->palette.GetBorderColor(rightIsHovering||middleIsHovering,isActive,rightIsMouseDown||middleIsMouseDown);
        SDL_Color& rightColor = win->palette.GetBorderColor(rightIsHovering,isActive,rightIsMouseDown);

        SDL_SetRenderDrawColor(renderer,leftColor.r,leftColor.g,leftColor.b,leftColor.a);

        for(int i = 0; i < win->palette.borderSize; i++)
        {
            SDL_RenderDrawLine(renderer,r.x+i,r.y,r.x+i,r.y+r.h); //horizontal


            SDL_RenderDrawLine(renderer,r.x,r.y+i,r.x+32,r.y+i); //horizontal

            SDL_RenderDrawLine(renderer,r.x,r.y+r.h-(1+i),r.x+32,r.y+r.h-(1+i)); //horizontal
        }
        SDL_SetRenderDrawColor(renderer,leftMiddleColor.r,leftMiddleColor.g,leftMiddleColor.b,leftMiddleColor.a);
        for(int i = 0; i < win->palette.borderSize; i++)
        {
            SDL_RenderDrawLine(renderer,(r.x+32)-i,r.y,(r.x+32)-i,r.y+r.h); //horizontal
        }
        SDL_SetRenderDrawColor(renderer,middleColor.r,middleColor.g,middleColor.b,middleColor.a);

        for(int i = 0; i < win->palette.borderSize; i++)
        {
            SDL_RenderDrawLine(renderer,r.x+33,r.y+i,r.x+(r.w-33),r.y+i); //horizontal
            SDL_RenderDrawLine(renderer,r.x+33,r.y+r.h-(1+i),r.x+(r.w-33),r.y+r.h-(1+i)); //horizontal
        }
        SDL_SetRenderDrawColor(renderer,rightColor.r,rightColor.g,rightColor.b,rightColor.a);

        for(int i = 0; i < win->palette.borderSize; i++)
        {
            SDL_RenderDrawLine(renderer,(r.x+r.w)-(i+1),r.y,(r.x+r.w)-(i+1),r.y+r.h); //horizontal

            SDL_RenderDrawLine(renderer,r.x+(r.w-32),r.y+i,r.x+r.w,r.y+i); //horizontal
            SDL_RenderDrawLine(renderer,r.x+(r.w-32),r.y+r.h-(1+i),r.x+r.w,r.y+r.h-(1+i)); //horizonal
        }
        SDL_SetRenderDrawColor(renderer,rightMiddleColor.r,rightMiddleColor.g,rightMiddleColor.b,rightMiddleColor.a);
        for(int i = 0; i < win->palette.borderSize; i++)
        {
            SDL_RenderDrawLine(renderer,(r.x+(r.w-32))+i,r.y,(r.x+(r.w-32))+i,r.y+r.h); //horizontal
        }
        


        SDL_SetRenderDrawColor(renderer,win->palette.accent.r,win->palette.accent.g,win->palette.accent.b,win->palette.accent.a);

        for(int i = 0; i < 4; i++)
        {
            int y1 = 15;
            int x1 = 8 + i;

            int y2 = 9;
            int x2 = 18 + i;

            SDL_RenderDrawLine(renderer,x1+r.x,y1+r.y,x2+r.x,y2+r.y);

            int y3 = 31-y1;
            int y4 = 31-y2;
           
            SDL_RenderDrawLine(renderer,x1+r.x,y3+r.y,x2+r.x,y4+r.y);

            int x3 = 31-x1;
            int x4 = 31-x2;

            
            SDL_RenderDrawLine(renderer,x3+r.x+(r.w-32),y1+r.y,x4+r.x+(r.w-32),y2+r.y);

            SDL_RenderDrawLine(renderer,x3+r.x+(r.w-32),y3+r.y,x4+r.x+(r.w-32),y4+r.y);
        }

        uint64_t width = (uint64_t)r.w - 66;
        uint64_t scrollSize = 4;
        uint64_t dif = max-min;
        if(dif > 0)
            scrollSize = width / dif;
        
        if(scrollSize < 4) scrollSize=4;

        
        double scroll = 0;
        if(dif > 0)
        scroll = (double)(value-min) / (double)dif;
        
        uint64_t scrollX = scroll * (width-scrollSize);



        if(scrollX > width-scrollSize) scrollX = width-scrollSize;
        SDL_Rect r2={.x=(int)scrollX+r.x+32,.y=r.y+win->palette.borderSize,.w=(int)scrollSize,.h = r.h-(win->palette.borderSize*2)};
        SDL_RenderFillRect(renderer,&r2);
    }
    bool HScrollView::OnEvent(SDL_Event& event, SDL_Rect& myBounds, SDL_Rect& visibleBounds)
    {
        if(event.type == SDL_MOUSEMOTION)
        {
            bool inside = event.motion.x >= visibleBounds.x && event.motion.x < visibleBounds.x+visibleBounds.w && event.motion.y >= visibleBounds.y && event.motion.y < visibleBounds.y+visibleBounds.h;
            bool hoverFlag = this->GetViewFlag(VIEWFLAG_HOVER_STATE) || this->GetViewFlag(VIEWFLAG_HOVER_B1STATE) || this->GetViewFlag(VIEWFLAG_HOVER_B2STATE);
            if(inside && !hoverFlag)
            {
                int x = event.motion.x - myBounds.x;
                if(x <= 32)
                {
                    //up btn
                    this->SetViewFlag(VIEWFLAG_HOVER_B1STATE,true);
                    this->SetViewFlag(VIEWFLAG_HOVER_B2STATE,false);
                    this->SetViewFlag(VIEWFLAG_HOVER_STATE,false);
                }
                else if(x >= myBounds.w-32)
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
                 int x = event.motion.x - myBounds.x;
                if(x <= 32)
                {
                    //up btn
                    this->SetViewFlag(VIEWFLAG_HOVER_B1STATE,true);
                    this->SetViewFlag(VIEWFLAG_HOVER_B2STATE,false);
                    this->SetViewFlag(VIEWFLAG_HOVER_STATE,false);
                }
                else if(x >= myBounds.w-32)
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
                int x = event.motion.x - myBounds.x;
                if(x <= 32)
                {
                    value = min;
                }
                else if(x >= myBounds.w-32)
                {
                    value=max;
                }
                else {
                    uint64_t width = (uint64_t)myBounds.w - 66;
                        uint64_t x2 = x-33;

                        double off = (double)x2 / (double)width;

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
                if(cea.x <= 32)
                {
                    //up btn
                    this->SetViewFlag(VIEWFLAG_MOUSEDOWN_B1STATE,true);
                }
                else if(cea.x >= myBounds.w-32)
                {
                    //down btn
                    this->SetViewFlag(VIEWFLAG_MOUSEDOWN_B2STATE,true);
                }
                else {
                    this->SetViewFlag(VIEWFLAG_MOUSEDOWN_STATE,true);
                    
                    uint64_t width = (uint64_t)myBounds.w - 66;
                        uint64_t x2 = cea.x-33;

                        double off = (double)x2 / (double)width;

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