#pragma once
#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include "../GUI.hpp"

namespace Tesses::Framework::SDL2::Views
{
    class VScrollView : public View {
        protected:
            virtual void OnValueChanged(GUIEventArgs& e);
            virtual void OnDraw(SDL_Renderer* renderer, SDL_Rect& r);
            virtual bool OnEvent(SDL_Event& event, SDL_Rect& myBounds, SDL_Rect& visibleBounds);
        public:
            VScrollView();
            VScrollView(uint64_t value, uint64_t min, uint64_t max,uint64_t step=1);
            uint64_t value;
            uint64_t min;
            uint64_t max;
            uint64_t step;
            EventList<View*,GUIEventArgs&> ValueChanged;

            std::pair<int,int> PreferedMinSize();
    };
}
#endif