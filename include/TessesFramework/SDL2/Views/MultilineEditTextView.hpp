#pragma once
#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include "../GUI.hpp"

namespace Tesses::Framework::SDL2::Views
{
    class MultilineEditTextView : public View {
        protected:
            virtual void OnDraw(SDL_Renderer* renderer, SDL_Rect& r);
            virtual bool OnEvent(SDL_Event& event, SDL_Rect& myBounds, SDL_Rect& visibleBounds);
            std::string hint;
            std::vector<std::string> lines;
            SDL_Point topLeft={.x=0,.y=0};
            SDL_Point cursorPos={.x=0,.y=0};
            SDL_Point cursorEnd={.x=-1,.y=-1};
        public:
            MultilineEditTextView();
            MultilineEditTextView(std::string hint);
            virtual std::string GetHint();
            virtual void SetHint(std::string hint);
            virtual std::string GetText();
            virtual void SetText(std::string text);
            virtual void TypeText(std::string text);
            virtual std::pair<int,int> PreferedMinSize();
    };
}
#endif