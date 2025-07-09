#pragma once
#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include "../GUI.hpp"

namespace Tesses::Framework::SDL2::Views
{
    class TabView : public ContainerView {
        
        protected:
            std::vector<std::pair<std::string,std::pair<View*,bool>>> items;
            virtual void OnDraw(SDL_Renderer* renderer, SDL_Rect& r);
            virtual bool OnEvent(SDL_Event& event, SDL_Rect& myBounds, SDL_Rect& visibleBounds);
       
        public:
            TabView();
            TabView(bool tabsVisible);
            bool tabsVisible;
            size_t firstTab;
            size_t selectedTab;
            
            void AddTab(std::string name, View* view, bool owns=true);
            void RemoveTab(View* view);
            void Clear();
            virtual size_t ViewCount();
            virtual View* GetViewAt(size_t index);
            ~TabView();
    };
}
#endif