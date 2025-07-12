#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include "TessesFramework/SDL2/Views/TabView.hpp"

namespace Tesses::Framework::SDL2::Views
{
    void TabView::OnDraw(SDL_Renderer* renderer, SDL_Rect& r)
    {
        auto view = this->GetViewAt(0);
        auto win =this->GetWindow();
        if(this->tabsVisible) 
        {
            
            int h = 
            win->monospaced_font->MaxHeight() + (win->palette.borderSize*3);
            int w = win->monospaced_font->MaxWidth() + (win->palette.borderSize*3);

            size_t noItems = this->items.size();
            size_t offset = 0;
            size_t width = noItems == 0 ? 0 : (r.w / noItems);
            if(noItems > 0 && (r.w / noItems) < w*5)
            {
                offset=this->firstTab;
                noItems = (r.w - (w*2)) / (w * 5);
                width = noItems == 0 ? 0 : ((r.w - (w*2)) / noItems);
                if(offset > items.size())
                    offset = items.size()-3;
                if((offset+noItems)>items.size()) noItems = items.size()-offset;
            }

            
            for(size_t i = 0; i < noItems; i++)
            {

                Clipper clipper(renderer,r);
                int yBottom = (h - 1)+r.y;
                //auto bc = win->palette.GetBorderColor(false,(i+offset)==this->selectedTab,false);
                std::string text = this->items[i+offset].first;
                int textW;
                int textH;
                win->monospaced_font->CalculateSize(text,textW,textH);
                



                /*if(i>0)
                {
                    //auto bc2 = win->palette.GetBorderColor(false,(i+offset)==this->selectedTab || ((i+offset)-1)==this->selectedTab,false);
                    //SDL_SetRenderDrawColor(renderer,bc2.r,bc2.g,bc2.b,bc2.a);
                   // SDL_RenderDrawLine(renderer,(width*i)+r.x,r.y,(width*i)+r.x,yBottom);
                }*/

                //SDL_SetRenderDrawColor(renderer,bc.r,bc.g,bc.b,bc.a);
                if((i+offset)==this->selectedTab)
                {
                    SDL_SetRenderDrawColor(renderer,win->palette.accent.r,win->palette.accent.g,win->palette.accent.b,win->palette.accent.a);
                    SDL_Rect r0={.x=(int)(width*i+1)+r.x,.y=r.y,.w=(int)width-2,.h=yBottom-r.y};
                    SDL_RenderFillRect(renderer,&r0);
                }
                //SDL_RenderDrawLine(renderer,(width*i+1)+r.x,yBottom,(width*(i+1)-2),yBottom);
                clipper.Clip({.x=(int)(width*i+1)+r.x,.y=r.y,.w=(int)width-2,.h=yBottom-r.y});
                int textX = ((width-2)/2) - (textW/2);
                int textY = ((yBottom-r.y)/2) - (textH/2);

                win->monospaced_font->Render(renderer,textX+r.x+(width*i+2),textY+r.y,text,((i+offset)==this->selectedTab) ? win->palette.background : win->palette.accent);
            }

            if(noItems != items.size())
            {
                auto greaterThanX=(r.w-w)+r.x;
                auto lessThanX = greaterThanX-w;
                int lgtX;
                int lgtY;
                win->monospaced_font->CalculateSize("<",lgtX,lgtY);
                lgtX = (w/2)-(lgtX/2);
                lgtY = (h/2)-(lgtY/2);
                win->monospaced_font->Render(renderer,lessThanX+lgtX,r.y+lgtY,"<",win->palette.accent);

                win->monospaced_font->CalculateSize(">",lgtX,lgtY);
                lgtX = (w/2)-(lgtX/2);
                lgtY = (h/2)-(lgtY/2);
                win->monospaced_font->Render(renderer,greaterThanX+lgtX,r.y+lgtY,">",win->palette.accent);
            }

            SDL_Rect viewBounds = {
                .x=r.x,
                .y=h+r.y,
                .w=r.w,
                .h=r.h-h
            };
            CallOnDraw(view,renderer,viewBounds);
        }
        else 
        {
            if(view != nullptr)  CallOnDraw(view,renderer,r);
        }
    }
    bool TabView::OnEvent(SDL_Event& event, SDL_Rect& myBounds, SDL_Rect& visibleBounds)
    {
        auto view = this->GetViewAt(0);
        auto win =this->GetWindow();
        if(this->tabsVisible) 
        {
            int h = 
            win->monospaced_font->MaxHeight() + (win->palette.borderSize*3);

            int w = win->monospaced_font->MaxWidth() + (win->palette.borderSize*3);
            int yBottom = (h - 1)+myBounds.y;
            size_t noItems = this->items.size();
            size_t offset = 0;
            
            size_t width = noItems == 0 ? 0 : (myBounds.w / noItems);
            if(noItems > 0 && (myBounds.w / noItems) < w*5)
            {
                offset=this->firstTab;
                noItems = (myBounds.w - (w*2)) / (w * 5);
                width = noItems == 0 ? 0 : ((myBounds.w - (w*2)) / noItems);
                if(offset > items.size())
                    offset = items.size()-3;
                if((offset+noItems)>items.size()) noItems = items.size()-offset;
            }

            int width_total = width*noItems;

            if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
            {
                if(event.button.x >= visibleBounds.x && event.button.x < visibleBounds.x+visibleBounds.w && event.button.y >= visibleBounds.y && event.button.y < visibleBounds.y+visibleBounds.h)
                {
            
                    if(event.button.y < myBounds.y+h && event.button.x < myBounds.x+width_total)
                    {
                        this->selectedTab = offset + ((event.button.x-myBounds.x) / width);
                        return true;
                    }
                    if(event.button.y < myBounds.y+h && event.button.x < myBounds.x+myBounds.w)
                    {
                        if(noItems != items.size())     
                        {
                            auto greaterThanX=myBounds.w-w+myBounds.x;
                            auto lessThanX = greaterThanX-w;


                            if(event.button.x >= lessThanX && event.button.x < greaterThanX)
                            {
                                firstTab--;
                                if(firstTab >= items.size())
                                    firstTab=0;
                            }
                            else if(event.button.x >= greaterThanX)
                            {
                                firstTab++;
                                if(firstTab >= items.size())
                                    firstTab = items.size()-1;
                            }
                        }
                    }
            
                }
            }


            SDL_Rect viewBounds = {
                .x=0,
                .y=h,
                .w=myBounds.w,
                .h=myBounds.h-h
            };

            SDL_Rect theirVisibleBounds = viewBounds;
                    theirVisibleBounds.x += visibleBounds.x;
                    theirVisibleBounds.y += visibleBounds.y;

            viewBounds.x += myBounds.x;
            viewBounds.y += myBounds.y;
            Clipper::ClipRect(theirVisibleBounds, visibleBounds);
            return CallOnEvent(view,event,viewBounds,theirVisibleBounds);
        }
        else 
        {
            if(view != nullptr) return CallOnEvent(view,event,myBounds,visibleBounds);
        }
        return View::OnEvent(event,myBounds,visibleBounds);
    }
       
      
    TabView::TabView() : TabView(true)
    {

    }
    TabView::TabView(bool tabsVisible) : ContainerView()
    {
        this->tabsVisible=tabsVisible;
        this->firstTab=0;
        this->selectedTab=0;
    }
            
            
    void TabView::AddTab(std::string name, View* view, bool owns)
    {
        for(auto& item : this->items)
        {
            if(item.second.first == view) return;
        }
        this->AssignChildParentToThis(view);
        this->items.push_back(std::pair<std::string,std::pair<View*,bool>>(name,std::pair<View*,bool>(view,owns)));
    }
    void TabView::RemoveTab(View* view)
    {
        for(auto index = this->items.begin(); index != this->items.end(); index++)
        {
            if(index->second.first == view)
            {
                if(index->second.second) delete index->second.first;
                this->items.erase(index);
                return;
            }
        }
    }
    void TabView::Clear()
    {
        for(auto& item : this->items)
        {
            if(item.second.second) delete item.second.first;
        }
        items.clear();
    }
    size_t TabView::ViewCount()
    {
        return this->items.empty() ? 0 : 1;
    }
    View* TabView::GetViewAt(size_t index)
    {
        if(this->items.empty()) return nullptr;
        if(this->selectedTab >= this->items.size()) return this->items.back().second.first;
        return this->items[this->selectedTab].second.first;
    }
    TabView::~TabView()
    {
        for(auto& item : this->items)
        {
            if(item.second.second) delete item.second.first;
        }
    }
}

#endif