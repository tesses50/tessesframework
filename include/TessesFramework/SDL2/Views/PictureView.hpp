#pragma once
#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include "../GUI.hpp"

namespace Tesses::Framework::SDL2::Views
{
    class PictureView : public View {
        SDL_Texture* tex;
        bool ownsTex;
        SDL_Surface* surf;
        bool ownsSurf;
        protected:
            virtual void OnDraw(SDL_Renderer* renderer, SDL_Rect& r);
        public:
        PictureView();
        void SetPicture(SDL_Texture* tex, bool owns=true);
        void SetPicture(SDL_Surface* surface,bool owns=true);
        ~PictureView();
    };

}
#endif