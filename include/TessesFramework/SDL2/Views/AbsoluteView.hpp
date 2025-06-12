#pragma once
#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include "../GUI.hpp"

namespace Tesses::Framework::SDL2::Views
{
    class AbsoluteView : public ContainerView {
        std::vector<std::pair<std::pair<View*,bool>,SDL_Rect>> views;
        protected:
            void OnDraw(SDL_Renderer* renderer, SDL_Rect& myRect);
            bool OnEvent(SDL_Event& event, SDL_Rect& myBounds, SDL_Rect& visibleBounds);
        public:
            size_t ViewCount();
            View* GetViewAt(size_t index);
            AbsoluteView();
            void Add(SDL_Rect rect, View* view, bool owns=true);
            void Set(View* view, SDL_Rect rect);
            void Remove(View* view);
            ~AbsoluteView();
        
    };

    /* class AbsoluteContainer : public View {
        std::vector<std::pair<std::pair<View*,bool>,SDL_Rect>> views;
        public:
            void Add(SDL_Rect rect, View* view, bool owns=true);
            void Set(View* view, SDL_Rect rect);
            void Remove(View* view);
            
            ~AbsoluteContainer();
    };*/
}
#endif