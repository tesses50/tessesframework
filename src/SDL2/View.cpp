#include "TessesFramework/SDL2/GUI.hpp"

#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
using namespace Tesses::Framework::Serialization::Json;
#include "TessesFramework/SDL2/Views/ButtonView.hpp"
#include "TessesFramework/SDL2/Views/CheckView.hpp"
#include "TessesFramework/SDL2/Views/LabelView.hpp"
namespace Tesses::Framework::SDL2 
{
    ContainerView::ContainerView() : View()
    {
        this->flags = 0;
    }
    ContainerView::ContainerView(std::string text) : View(text)
    {
        this->flags = 0;
    }
    View::View() : View((std::string){})
    {

    }
    View::View(std::string text)
    {
        this->text = text;
        this->flags=VIEWFLAG_TABSTOP;
    }
    void View::SetText(std::string text)
    {
        this->text=text;
    }
    std::string View::GetText()
    {
        return this->text;
    }
    void View::OnMouseUp(GUIMouseButtonEventArgs& click)
    {
        
    }
    void View::OnMouseDown(GUIMouseButtonEventArgs& click)
    {
        
    }
    void View::OnClick(GUIEventArgs& click)
    {
        
    }
    GUIWindow* View::GetWindow()
    {
        if(this->parent ==nullptr) return nullptr;
        return this->parent->GetWindow();
    }
    void View::OnDragDropFile(Tesses::Framework::Filesystem::VFSPath filePath,SDL_Rect myRect, SDL_Point dropLoc)
    {

    }
    void View::OnDragDropText(std::string text,SDL_Rect myRect, SDL_Point dropLoc)
    {

    }

    void View::OnEnter(GUIEventArgs& evt)
    {

    }
    void View::OnLeave(GUIEventArgs& evt)
    {

    }
           
   
    void View::OnDraw(SDL_Renderer* renderer, SDL_Rect& myRect)
    {

    }
    void View::OnSetParent(View* v)
    {
        
    }
            
    bool View::OnEvent(SDL_Event& event, SDL_Rect& myBounds, SDL_Rect& myVisibleBounds)
    {
        GUISDLEventEventArgs sdle;
        sdle.event = event;
        this->SDLEvent.Invoke(this,sdle);
        if(event.type == SDL_FINGERUP)
        {
            auto cord = this->GetCordFromEvent(event);
            if(cord.first >= myVisibleBounds.x && cord.first < myVisibleBounds.x+myVisibleBounds.w && cord.second >= myVisibleBounds.y && cord.second < myVisibleBounds.y+myVisibleBounds.h) {
            GUIEventArgs cea2;
            OnClick(cea2);
            this->Click.Invoke(this,cea2);
            return true;
            }
        }
        else if(event.type == SDL_MOUSEBUTTONDOWN)
        {
            if(event.button.x >= myVisibleBounds.x && event.button.x < myVisibleBounds.x+myVisibleBounds.w && event.button.y >= myVisibleBounds.y && event.button.y < myVisibleBounds.y+myVisibleBounds.h)
            {
                GUIMouseButtonEventArgs cea;
                cea.button = (int)event.button.button;
                cea.x = event.button.x - myVisibleBounds.x;
                cea.y = event.button.y - myVisibleBounds.y;
                cea.which = event.button.which;
                this->SetViewFlag(VIEWFLAG_MOUSEDOWN_STATE,true);
                OnMouseDown(cea);
                this->MouseDown.Invoke(this,cea);
            }
        }
        else if(event.type == SDL_MOUSEBUTTONUP)
        {
            if(this->GetViewFlag(VIEWFLAG_MOUSEDOWN_STATE))
            {
                GUIMouseButtonEventArgs cea;
                cea.button = (int)event.button.button;
                cea.x = event.button.x - myVisibleBounds.x;
                cea.y = event.button.y - myVisibleBounds.y;
                cea.which = event.button.which;
                this->SetViewFlag(VIEWFLAG_MOUSEDOWN_STATE,false);
                OnMouseUp(cea);
                this->MouseUp.Invoke(this,cea);
                GUIEventArgs cea2;
                OnClick(cea2);
                this->Click.Invoke(this,cea2);
                return true;
            }
            
        }
        else if(event.type == SDL_MOUSEMOTION)
        {
            bool inside = event.motion.x >= myVisibleBounds.x && event.motion.x < myVisibleBounds.x+myVisibleBounds.w && event.motion.y >= myVisibleBounds.y && event.motion.y < myVisibleBounds.y+myVisibleBounds.h;
            bool hoverFlag = this->GetViewFlag(VIEWFLAG_HOVER_STATE);
            if(inside && !hoverFlag)
            {
                this->SetViewFlag(VIEWFLAG_HOVER_STATE,true);
                GUIEventArgs e;
                this->OnEnter(e);
                this->Enter.Invoke(this,e);
            }
            else if(!inside && hoverFlag)
            {
                this->SetViewFlag(VIEWFLAG_HOVER_STATE,false);
                GUIEventArgs e;
                this->OnLeave(e);
                this->Leave.Invoke(this,e);
            }
        }
        return false;
    }

    
    std::string View::GetId()
    {
        return this->id;
    }
    void View::SetId(std::string id)
    {
        this->id = id;
    }
    View* View::FindViewById(std::string id)
    {
        if(this->GetId() == id) return this;
        return nullptr;
    }

    View* ContainerView::FindViewById(std::string id)
    {
        if(this->GetId() == id) return this;
        for(size_t i = 0; i <this->ViewCount(); i++)
        {
            auto v = this->GetViewAt(i);
            auto v2 = v->FindViewById(id);
            if(v2 != nullptr) return v2;
        }
        return nullptr;
    }
    std::pair<int,int> View::PreferedMinSize()
    {
        return std::pair<int,int>(-1,-1);
    }

    std::pair<int,int> View::GetCordFromEvent(SDL_Event& event)
    {
        switch(event.type)
        {
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                return std::pair<int,int>(event.button.x,event.button.y);
                break;
            case SDL_FINGERUP:
            case SDL_FINGERDOWN:
            {   
                GUIWindow* win = this->GetWindow(); 
                int w,h;
                SDL_GetWindowSize(win->window,&w,&h);
                return std::pair<int,int>(w*event.tfinger.x,h*event.tfinger.y);
            }
                break;
        }
        return std::pair<int,int>(-1,-1);

    }

    View::~View()
    {

    }
    
}
#endif
