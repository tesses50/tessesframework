#pragma once
#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include "../GUI.hpp"

namespace Tesses::Framework::SDL2::Views
{
    class CheckView : public View {
        protected:
            virtual void OnCheckChanged(GUIEventArgs& event);
            virtual void OnDraw(SDL_Renderer* renderer, SDL_Rect& r);
            virtual bool OnEvent(SDL_Event& event, SDL_Rect& myBounds, SDL_Rect& visibleBounds);
        
        public:
            CheckView(bool checked, std::string label);
            CheckView();

            virtual bool GetChecked();
            virtual void SetChecked(bool value);



            EventList<View*,GUIEventArgs&> CheckChanged;
    };
}
#endif