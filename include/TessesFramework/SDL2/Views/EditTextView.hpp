#pragma once
#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include "../GUI.hpp"

namespace Tesses::Framework::SDL2::Views
{
    class EditTextView : public View {
        protected:
            virtual void OnDraw(SDL_Renderer* renderer, SDL_Rect& r);
            virtual bool OnEvent(SDL_Event& event, SDL_Rect& myBounds, SDL_Rect& visibleBounds);
            std::string hint;
            size_t cursorPos;
            size_t cursorEnd;
        public:
            EditTextView();
            EditTextView(std::string hint);
            virtual std::string GetHint();
            virtual void SetHint(std::string hint);
            virtual void SetText(std::string text);
            virtual void TypeText(std::string text);
            virtual std::pair<int,int> PreferedMinSize();
    };
}
#endif