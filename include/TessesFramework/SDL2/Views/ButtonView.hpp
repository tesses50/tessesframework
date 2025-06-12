#pragma once
#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include "../GUI.hpp"

namespace Tesses::Framework::SDL2::Views
{
    class ButtonView : public View {
        protected:
            virtual void OnDraw(SDL_Renderer* renderer, SDL_Rect& r);
            virtual bool OnEvent(SDL_Event& event, SDL_Rect& myBounds, SDL_Rect& visibleBounds);
        
        public:
            ButtonView();
            ButtonView(std::string text);
            
    };
}
#endif