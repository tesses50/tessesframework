#pragma once
#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include <SDL2/SDL.h>
#include "FontCache.hpp"
#include "../Filesystem/VFSFix.hpp"
#include "../Filesystem/VFS.hpp"
#include "../Common.hpp"
#include "../Serialization/Json.hpp"

namespace Tesses::Framework::SDL2
{
    class GUIPalette {
        public:
            GUIPalette();
            GUIPalette(bool isDarkMode, SDL_Color accent,int fontSize=24);
            GUIPalette(SDL_Color accent, SDL_Color background, SDL_Color border_color, SDL_Color border_hover, SDL_Color border_active, SDL_Color border_hover_active, int fontSize=24);
            SDL_Color accent; //color is used for font when not over accent background
            SDL_Color background;
            SDL_Color border_color; //color is used for font when over accent background
            SDL_Color border_hover;
            SDL_Color border_active;
            SDL_Color border_hover_active;
            int fontSize;

            SDL_Color& GetBorderColor(bool isHovering, bool isActive, bool isMouseDown);
    };
    
    class GUIEventArgs
    {
        public:
            virtual std::string Type();
            virtual ~GUIEventArgs();
    };
    class View;
    class GUIMouseButtonEventArgs : public GUIEventArgs
    {
        public:
            Uint32 which;
            int x;
            int y;
            Uint8 button;
            std::string Type();
    };
    class GUIJsonViewNotFoundEventArgs : public GUIEventArgs
    {
        public:
            std::string Type();
            View* destView;
            Tesses::Framework::Serialization::Json::JObject jsonObject;
            std::string typeString;
    };
    class GUISDLEventEventArgs : public GUIEventArgs
    {
        public:
            std::string Type();
            SDL_Event event;
    };
    constexpr uint64_t VIEWFLAG_HOVER_STATE=(uint64_t)1 << 0;
    constexpr uint64_t VIEWFLAG_MOUSEDOWN_STATE =(uint64_t)1<<1;
    constexpr uint64_t VIEWFLAG_ISACTIVE=(uint64_t)1<<2;
    constexpr uint64_t VIEWFLAG_TABSTOP=(uint64_t)1<<3;
    constexpr uint64_t VIEWFLAG_INTERCEPT_TAB=(uint64_t)1<<4;
    constexpr uint64_t VIEWFLAG_CHECKED=(uint64_t)1<<5;
    class GUIPopup;
    class GUIWindow;
    class ContainerView;
     class View {
        protected:
            std::string text;
            std::string id;
            uint64_t flags;
        protected:
            
            View();
            View(std::string text);
            virtual void OnDraw(SDL_Renderer* renderer, SDL_Rect& r);
            virtual void OnDragDropFile(Tesses::Framework::Filesystem::VFSPath filePath,SDL_Rect myRect, SDL_Point dropLoc);
            virtual void OnDragDropText(std::string text,SDL_Rect myRect, SDL_Point dropLoc);
            virtual void OnEnter(GUIEventArgs& evt);
            virtual void OnLeave(GUIEventArgs& evt);
            virtual void OnMouseDown(GUIMouseButtonEventArgs& evt);
            virtual void OnMouseUp(GUIMouseButtonEventArgs& evt);
            virtual void OnClick(GUIEventArgs& evt);
            virtual bool OnEvent(SDL_Event& event, SDL_Rect& myBounds, SDL_Rect& visibleBounds);
            virtual void OnSetParent(View* v);

            void CallOnDraw(View* view, SDL_Renderer* renderer, SDL_Rect& myRect)
            {
                view->OnDraw(renderer,myRect);
            }
            bool CallOnEvent(View* view,SDL_Event& event, SDL_Rect& myBounds, SDL_Rect& visibleBounds)
            {
                return view->OnEvent(event,myBounds,visibleBounds);
            }
            

            ContainerView* parent;

        public:
            bool GetViewFlag(uint64_t flag)
            {
                return (this->flags & flag) != 0;
            }
            void SetViewFlag(uint64_t flag, bool value)
            {
                if(value)  
                    this->flags |= flag;
                else
                    this->flags &= ~flag; 
            }

            EventList<View*,GUIMouseButtonEventArgs&> MouseUp;
            EventList<View*,GUIMouseButtonEventArgs&> MouseDown;
            EventList<View*,GUIEventArgs&> Click;
            EventList<View*,GUIEventArgs&> Enter;
            EventList<View*,GUIEventArgs&> Leave;
            EventList<View*,GUISDLEventEventArgs&> SDLEvent;
            virtual ~View();
            friend class GUIWindow;
            virtual GUIWindow* GetWindow();
            virtual std::string GetText();
            virtual void SetText(std::string text);
            virtual void SetId(std::string id);
            virtual std::string GetId();
            virtual View* FindViewById(std::string id);
            friend class ContainerView;
    };
    class ContainerView : public View {
        public:
            virtual size_t ViewCount()=0;
            virtual View* GetViewAt(size_t index)=0;
            virtual View* FindViewById(std::string id);
        protected:
            ContainerView();
            ContainerView(std::string text);

            

            
            void AssignChildParentToThis(View* view)
            {
                if(view != nullptr)
                {
                    view->parent = this;
                    view->OnSetParent(this);
                    
                }   
            }
    };
    enum class TabNextResult {
        KeepGoing,
        TabNext,
        Done
    };
    class GUIPopup : public ContainerView {
        View* child;
        bool ownsChild;
        protected:
            void OnDraw(SDL_Renderer* renderer, SDL_Rect& myRect);
            bool OnEvent(SDL_Event& event, SDL_Rect& myBounds, SDL_Rect& visibleBounds);
        public: 

            size_t ViewCount();
            View* GetViewAt(size_t index);
            GUIPopup();
            GUIPopup(SDL_Rect bounds);
            GUIPopup(int x, int y,int w, int h);
            SDL_Rect bounds;
            
            void SetView(View* view, bool owns=true);
            ~GUIPopup();
            
            friend class GUIWindow;
    };
    class GUIWindow : public ContainerView
    {
        std::vector<GUIPopup*> popups;
        View* child;
        bool ownsChild;
        SDL_Window* window;
        SDL_Renderer* renderer;
        void Event(SDL_Event& event);
        void Draw();
        void DeactivateAll(View* view);
        void TabNext(View* view,TabNextResult& nr);
        
        protected:
            void OnDraw(SDL_Renderer* renderer, SDL_Rect& myRect);
            bool OnEvent(SDL_Event& event, SDL_Rect& myBounds, SDL_Rect& visibleBounds);
            
        public:
            EventList<View*,GUIJsonViewNotFoundEventArgs&> JsonViewNotFound;
            size_t ViewCount();
            View* GetViewAt(size_t index);
            FontCache* normal_font;
            FontCache* monospaced_font;
            GUIPalette palette;
            GUIWindow(std::string title, int w, int h, Uint32 flags, const GUIPalette& palette);
            void SetPalette(const GUIPalette& palette);
            void SetView(View* view,bool owns=true);
            
            void ShowPopup(GUIPopup* popup);
            void ShowPopup(GUIPopup& popup);

            void MakeActive(View* view);
            void TabNext();
            GUIWindow* GetWindow();
           
            ~GUIWindow();

            friend class GUI;
            friend class GUIPopup;
            
            void SetText(std::string text);

            void SetView(Tesses::Framework::Serialization::Json::JToken json);

            View* CreateViewFromJson(Tesses::Framework::Serialization::Json::JObject json);
    };  

    
    class GUI {
        std::vector<GUIWindow*> windows;
        public:
        void Update();
        friend class GUIWindow;
    };
    extern GUI gui;

    class Clipper {
        SDL_Rect theRect;
        SDL_Renderer* renderer;
        bool isClipped;
        public:
            Clipper(SDL_Renderer* renderer, SDL_Rect& myRect);
            bool Clip(SDL_Rect rect);
            ~Clipper();
            static void ClipRect(SDL_Rect& child, SDL_Rect& parent);
    };
    

    
}
#endif