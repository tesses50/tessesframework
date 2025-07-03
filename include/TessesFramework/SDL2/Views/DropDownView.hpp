#pragma once
#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include "../GUI.hpp"
#include "ScrollableTextListView.hpp"

namespace Tesses::Framework::SDL2::Views
{
    class DropDownView : public View {
        
        GUIContainerPopup popup;
        ScrollableTextListView listView;
        bool hasSet=false;
        protected:
            virtual void OnDraw(SDL_Renderer* renderer, SDL_Rect& r);
            virtual bool OnEvent(SDL_Event& event, SDL_Rect& myBounds, SDL_Rect& visibleBounds);
        public:
            DropDownView();
            std::vector<std::string>& GetItems();
            void SetIndex(int index);
            int GetIndex();
    };
}
#endif