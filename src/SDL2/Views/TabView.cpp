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