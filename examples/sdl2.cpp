
#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include <SDL2/SDL.h>
#endif

int main(int argc,char** argv)
{
    #if defined(TESSESFRAMEWORK_ENABLE_SDL2)

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow("My SDL Window",0,0,320,240,0);

    SDL_Renderer* renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    while(true)
    {
        SDL_Event event;
        if(SDL_PollEvent(&event) && event.type == SDL_QUIT) break;
        SDL_SetRenderDrawColor(renderer,255,0,0,255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyRenderer(renderer);

    SDL_DestroyWindow(window);
    #endif
    return 0;
}