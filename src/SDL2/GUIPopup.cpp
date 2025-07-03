#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include "TessesFramework/SDL2/GUI.hpp"

namespace Tesses::Framework::SDL2 {
size_t GUIPopup::ViewCount()
{
    return this->GetView() != nullptr ? 1 : 0;
}
View* GUIPopup::GetViewAt(size_t index)
{
    if(index > 0) return nullptr;
        return this->GetView();
}
GUIPopup::GUIPopup() : GUIPopup(0,0,0,0)
{
    
}
GUIPopup::GUIPopup(SDL_Rect bounds)
{
    this->bounds = bounds;
}
GUIPopup::GUIPopup(int x, int y,int w, int h)
{
    this->bounds.x = x;
    this->bounds.y = y;
    this->bounds.w = w;
    this->bounds.h = h;
}
View* GUIContainerPopup::GetView()
{
    return this->child;
}
GUIContainerPopup::GUIContainerPopup() : GUIPopup()
{

}
GUIContainerPopup::GUIContainerPopup(SDL_Rect bounds) : GUIPopup(bounds)
{

}
GUIContainerPopup::GUIContainerPopup(int x, int y,int w, int h) : GUIPopup(x,y,w,h)
{

}

            
void GUIContainerPopup::SetView(View* view, bool owns)
{
    if(this->ownsChild && this->child != view)
        delete this->child;
    this->child = view;
    this->ownsChild=owns;
    this->AssignChildParentToThis(view);
}
GUIContainerPopup::~GUIContainerPopup()
{
    if(this->ownsChild)
        delete this->child;
}
GUIPopup::~GUIPopup()
{
    
}

void GUIPopup::Close()
{

    this->closed=true;
}
bool GUIPopup::IsClosed()
{
    return this->closed;
}

void GUIPopup::OnDraw(SDL_Renderer* renderer, SDL_Rect& r)
{
    auto win = this->GetWindow();
    SDL_SetRenderDrawColor(renderer,win->palette.background.r,win->palette.background.g,win->palette.background.b,win->palette.background.a);
    SDL_RenderFillRect(renderer,&r);
    auto view = GetView();
    if(view != nullptr)
            CallOnDraw(view,renderer,r);
}
bool GUIPopup::OnEvent(SDL_Event& event, SDL_Rect& myBounds, SDL_Rect& myVisibleBounds)
{
    auto view = GetView();
    if(view != nullptr)
    {
        if(CallOnEvent(view,event,myBounds,myVisibleBounds)) return true;
    }

    return View::OnEvent(event,myBounds,myVisibleBounds);
}

bool GUIPopup::IsActive()
{
    auto win = this->GetWindow();
    if(win == nullptr) return false;
    if(win->popups.empty()) return false;

    return win->popups.back() == this;
}
            
}
#endif