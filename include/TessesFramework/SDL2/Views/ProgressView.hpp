#pragma once
#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include "../GUI.hpp"

namespace Tesses::Framework::SDL2::Views
{
    class ProgressView : public View 
    {
        protected:
            virtual void OnDraw(SDL_Renderer* renderer, SDL_Rect& r);
        public:
            ProgressView();
            ProgressView(double value);
            double value;
    };
};
#endif