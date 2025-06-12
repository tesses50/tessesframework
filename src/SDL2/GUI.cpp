#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include "TessesFramework/SDL2/GUI.hpp"
#include <iostream>
namespace Tesses::Framework::SDL2 
{
      GUI gui;
    void GUI::Update()
    {
        if(this->windows.empty()) return;
        for(auto index = this->windows.begin(); index != this->windows.end(); index++)
        {
            if(*index == nullptr)
            {
                this->windows.erase(index);
                index--;
            }
        }
        if(this->windows.empty()) return;

        
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            
            for(auto win : this->windows)
            {
                if(win == nullptr) continue;
                auto id = SDL_GetWindowID(win->window);
                switch(event.type)
                {
                    case SDL_EventType::SDL_WINDOWEVENT:
                        if(event.window.windowID == id)
                        {
                            win->Event(event);
                        }
                        break;
                    case SDL_EventType::SDL_MOUSEBUTTONDOWN:
                    case SDL_EventType::SDL_MOUSEBUTTONUP:
                        if(event.button.windowID == id)
                        {
                            win->Event(event);
                        }
                        break;
                    case SDL_EventType::SDL_MOUSEMOTION:
                        if(event.motion.windowID == id)
                        {
                            win->Event(event);
                        }
                        break;
                    case SDL_EventType::SDL_MOUSEWHEEL:
                        if(event.wheel.windowID == id)
                        {
                            win->Event(event);
                        }
                        break;
                    case SDL_EventType::SDL_KEYUP:
                    case SDL_EventType::SDL_KEYDOWN:
                        if(event.key.windowID == id)
                        {
                            win->Event(event);
                        }
                        break;
                    case SDL_EventType::SDL_TEXTEDITING:
                        if(event.edit.windowID == id)
                        {
                            win->Event(event);
                        }
                        break;
                    case SDL_EventType::SDL_TEXTEDITING_EXT:
                        if(event.editExt.windowID == id)
                        {
                            win->Event(event);
                        }
                        break;
                    case SDL_EventType::SDL_TEXTINPUT:
                        if(event.text.windowID == id)
                        {
                            win->Event(event);
                        }
                        break;
                    case SDL_EventType::SDL_DROPBEGIN:
                    case SDL_EventType::SDL_DROPCOMPLETE:
                    case SDL_EventType::SDL_DROPFILE:
                    case SDL_EventType::SDL_DROPTEXT:
                        if(event.drop.windowID == id)
                        {
                            win->Event(event);
                        }
                        break;
                    default:
                        win->Event(event);
                        break;
                }
                
            }
            
            if(event.type == SDL_QUIT)
            {
                TF_SetIsRunning(false);
                break;
            }
        }
        for(auto item : this->windows)
        {
            if(item != nullptr)
            item->Draw();
        }
    }

  
    Clipper::Clipper(SDL_Renderer* renderer, SDL_Rect& myRect)
    {
        this->isClipped=SDL_RenderIsClipEnabled(renderer);
        SDL_RenderGetClipRect(renderer,&this->theRect);
        if(!this->isClipped) this->theRect = myRect;
        this->renderer = renderer;
    }
    void Clipper::ClipRect(SDL_Rect& child, SDL_Rect& parent)
    {
        if(child.x < parent.x)
        {
            int rem = parent.x- child.x;
            child.x += rem;
            child.w -= rem;
        }
        if(child.y < parent.y)
        {
            int rem = parent.y- child.y;
            child.y += rem;
            child.h -= rem;
        }

         if((child.x + child.w) > (parent.x + parent.w))
            child.w = (parent.x + parent.w) - child.x;
                
        if((child.y + child.h) > (parent.y + parent.h))
            child.h = (parent.y + parent.h) - child.y;

    }
    bool Clipper::Clip(SDL_Rect rect)
    {
        ClipRect(rect,this->theRect);
        if(rect.w <= 0 || rect.h <= 0) return false;
                
        SDL_RenderSetClipRect(renderer, &rect);  
        return true;
    }
    Clipper::~Clipper()
    {
        if(isClipped) SDL_RenderSetClipRect(renderer, &this->theRect);
        else SDL_RenderSetClipRect(renderer, nullptr);
    }

    GUIPalette::GUIPalette()
    {
        this->fontSize=24;
    }
    SDL_Color& GUIPalette::GetBorderColor(bool isHovering, bool isActive, bool isMouseDown)
    {
        bool isHovering2=isHovering ^ isMouseDown;
        if(isHovering2 && isActive)
            return this->border_hover_active;
        if(isHovering2)
            return this->border_hover;
        if(isActive)
            return this->border_active;
        return this->border_color;
    }

    GUIPalette::GUIPalette(SDL_Color accent, SDL_Color background, SDL_Color border_color, SDL_Color border_hover, SDL_Color border_active, SDL_Color border_hover_active, int fontSize)
    {
        this->accent=accent;
        this->background = background;
        this->border_color=border_color;
        this->border_hover = border_hover;
        this->border_active = border_active;
        this->border_hover_active=border_hover_active;
        this->fontSize = fontSize;
    }

    std::string GUIEventArgs::Type()
    {
        return "Base";
    }
    GUIEventArgs::~GUIEventArgs()
    {

    }
    std::string GUIMouseButtonEventArgs::Type()
    {
         return "MouseButton";
    }
    std::string GUIJsonViewNotFoundEventArgs::Type()
    {
        return "JsonViewNotFound";
    }
    std::string GUISDLEventEventArgs::Type()
    {
        return "SDLEvent";
    }

    GUIPalette::GUIPalette(bool isDarkMode, SDL_Color accent,int fontSize)
    {
        this->accent = accent;
        this->fontSize = fontSize;

        if(isDarkMode)
        {
            this->background = {.r = 42,.g=42,.b=42,.a=255};
            this->border_color = {.r=0,.g=0,.b=0,.a=255};
            this->border_hover = {.r=92,.g=92,.b=92,.a=255};
            
            this->border_active = {.r=200,.g=200,.b=200,.a=255};
            this->border_hover_active = {.r=(uint8_t)(255-accent.r),.g=(uint8_t)(255-accent.g),.b=(uint8_t)(255-accent.b),.a=255};
        
        }
        else
        {
            this->background = {.r=239,.g=239,.b=239,.a=255};
            this->border_color = {.r=0,.g=0,.b=0,.a=255};
            
            this->border_active = {.r=92,.g=92,.b=92,.a=255};
            
            this->border_hover = {.r=200,.g=200,.b=200,.a=255};
            
            this->border_hover_active = {.r=(uint8_t)(255-accent.r),.g=(uint8_t)(255-accent.g),.b=(uint8_t)(255-accent.b),.a=255};
        }
    }
            
}
#endif