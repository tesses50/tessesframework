#pragma once
#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include "../GUI.hpp"

namespace Tesses::Framework::SDL2::Views
{
    class HStackView : public ContainerView {

            std::vector<std::pair<int,std::pair<View*,bool>>> items;
        protected:
            virtual void OnDraw(SDL_Renderer* renderer, SDL_Rect& r);
            virtual bool OnEvent(SDL_Event& event, SDL_Rect& myBounds, SDL_Rect& visibleBounds);
        public:
            HStackView();
            int spacing=0;

            void Add(int sz, View* view, bool owns=true);
            void Remove(View* view);
            void Clear();
            
            virtual size_t ViewCount();
            virtual View* GetViewAt(size_t index);

            virtual ~HStackView();
    };
}
#endif