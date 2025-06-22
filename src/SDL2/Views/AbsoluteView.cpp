#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include "TessesFramework/SDL2/Views/AbsoluteView.hpp"

namespace Tesses::Framework::SDL2::Views
{
        void AbsoluteView::OnDraw(SDL_Renderer* renderer, SDL_Rect& myRect)
        {
            Clipper clipper(renderer,myRect);
            SDL_Rect oldClip;
            bool isClipped=SDL_RenderIsClipEnabled(renderer);
            SDL_RenderGetClipRect(renderer,&oldClip);
            if(!isClipped) oldClip = myRect;

            for(auto& ctrl : this->views)
            {
                SDL_Rect childRect = ctrl.second;
                childRect.x += oldClip.x;
                childRect.y += oldClip.y;
                

                if(clipper.Clip(childRect))
                    CallOnDraw(ctrl.first.first,renderer,childRect);
            }
            
        }
        bool AbsoluteView::OnEvent(SDL_Event& event, SDL_Rect& myBounds, SDL_Rect& visibleBounds)
        {
            bool handled=false;
            
            for(auto& item : this->views)
            {
                SDL_Rect theirBounds = item.second;
                theirBounds.x += myBounds.x;
                theirBounds.y += myBounds.y;

                SDL_Rect theirVisibleBounds = item.second;
                theirVisibleBounds.x += visibleBounds.x;
                theirVisibleBounds.y += visibleBounds.y;
                Clipper::ClipRect(theirVisibleBounds, visibleBounds);

                
                if(CallOnEvent(item.first.first,event,theirBounds, theirVisibleBounds))
                    handled=true;
                if(handled && event.type != SDL_MOUSEBUTTONUP && event.type != SDL_MOUSEMOTION && event.type != SDL_FINGERUP)
                {
                    GUISDLEventEventArgs sdle;
                    sdle.event = event; this->SDLEvent.Invoke(this,sdle);
                    return true;
                }
            }


            if(handled) {GUISDLEventEventArgs sdle;
        sdle.event = event; this->SDLEvent.Invoke(this,sdle);return true;}
            return View::OnEvent(event,myBounds,visibleBounds);
        }
        size_t AbsoluteView::ViewCount()
        {
            return this->views.size();
        }
        View* AbsoluteView::GetViewAt(size_t index)
        {
            return this->views.at(index).first.first;
        }
        AbsoluteView::AbsoluteView() : ContainerView()
        {

        }
        void AbsoluteView::Add(SDL_Rect rect, View* view, bool owns)
        {
            for(auto& item : this->views)
            {
                if(item.first.first == view) return;
            }
            this->AssignChildParentToThis(view);
            this->views.push_back(std::pair<std::pair<View*,bool>,SDL_Rect>(std::pair<View*,bool>(view,owns),rect));
        }
        void AbsoluteView::Set(View* view, SDL_Rect rect)
        {
            for(auto& item : this->views)
            {
                if(item.first.first == view) 
                {
                    item.second = rect;
                    return;
                }
            }
        }
        void AbsoluteView::Remove(View* view)
        {
            for(auto index = this->views.begin(); index != this->views.end(); index++)
            {
                if(index->first.first == view)
                {
                    if(index->first.second) delete index->first.first;
                    this->views.erase(index);
                    return;
                }
            }
        }
        AbsoluteView::~AbsoluteView()
        {
            for(auto& item : this->views)
            {
                if(item.first.second) delete item.first.first;
            }
        }
}
#endif