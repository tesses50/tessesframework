#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include "TessesFramework/SDL2/GUI.hpp"
#include "TessesFramework/aardvark-fixed-regular.h"
#include "TessesFramework/SDL2/Views/ButtonView.hpp"
#include "TessesFramework/SDL2/Views/LabelView.hpp"
#include "TessesFramework/SDL2/Views/CheckView.hpp"
#include "TessesFramework/SDL2/Views/ProgressView.hpp"
#include "TessesFramework/SDL2/Views/TextListView.hpp"
#include "TessesFramework/SDL2/Views/AbsoluteView.hpp"
#include "TessesFramework/SDL2/ParseColor.hpp"
namespace Tesses::Framework::SDL2 
{
    void GUIWindow::MakeActive(View* view)
    {
        if(!view->GetViewFlag(VIEWFLAG_TABSTOP)) return;
        if(view->GetViewFlag(VIEWFLAG_ISACTIVE)) return;
        DeactivateAll(this);
        view->SetViewFlag(VIEWFLAG_ISACTIVE,true);
    }
    void GUIWindow::DeactivateAll(View* view)
    {
        view->SetViewFlag(VIEWFLAG_ISACTIVE,false);
        auto cv = dynamic_cast<ContainerView*>(view);
        if(cv != nullptr)
        {
            for(size_t i = 0; i < cv->ViewCount();i++)
            {
                DeactivateAll(cv->GetViewAt(i));
            }
        }
    }
    void GUIWindow::TabNext(View* view,TabNextResult& nr)
    {
        if(nr == TabNextResult::Done) 
            return;
        auto cv = dynamic_cast<ContainerView*>(view);
        if(cv != nullptr)
        {
            for(size_t i = 0; i < cv->ViewCount();i++)
            {
                TabNext(cv->GetViewAt(i),nr);
                if(nr == TabNextResult::Done) 
                    return;
            }
        }
        else
        {
            if(view->GetViewFlag(VIEWFLAG_ISACTIVE))
            {
                if(view->GetViewFlag(VIEWFLAG_INTERCEPT_TAB))
                {
                    SDL_Rect r;
                    r.x=0;
                    r.y=0;
                    r.w=0;
                    r.h=0;
                    SDL_Event event;
                    event.type = SDL_KEYDOWN;
                    event.key.keysym.mod = SDL_Keymod::KMOD_NONE;
                    event.key.keysym.scancode = SDL_SCANCODE_TAB;
                    event.key.keysym.sym = SDL_KeyCode::SDLK_TAB;
                    view->CallOnEvent(view,event,r,r);
                    nr = TabNextResult::Done;
                }
                view->SetViewFlag(VIEWFLAG_ISACTIVE,false);
                nr=  TabNextResult::TabNext;
            }
            else if(view->GetViewFlag(VIEWFLAG_TABSTOP) && nr == TabNextResult::TabNext)
            {
                view->SetViewFlag(VIEWFLAG_ISACTIVE,true);
                nr = TabNextResult::Done;
            }
        }
    }
    void GUIWindow::TabNext()
    {
        TabNextResult nr=TabNextResult::KeepGoing;
        TabNext(this,nr);
        if(nr != TabNextResult::Done)
        {
            nr = TabNextResult::TabNext;
            TabNext(this,nr);
        }
    }
    
    void GUIWindow::SetView(View* view,bool owns)
    {
        if(this->ownsChild && this->child != view)
            delete this->child;
        this->child = view;
        this->ownsChild = owns;
        if(view != nullptr)
            view->parent = this;
    }
    GUIWindow* GUIWindow::GetWindow()
    {
        return this;
    }
    size_t GUIWindow::ViewCount()
    {
        return this->child != nullptr ? 1 : 0;
    }
    View* GUIWindow::GetViewAt(size_t index)
    {
        if(index > 0) return nullptr;
        return this->child;
    }
    void GUIWindow::Draw()
    {
        int w,h;
        SDL_GetWindowSize(window,&w,&h);
        SDL_Rect r={.x=0,.y=0,.w=w,.h=h};
        OnDraw(renderer,r);
    }
    void GUIWindow::Event(SDL_Event& event)
    {
        int w,h;
        SDL_GetWindowSize(window,&w,&h);
        SDL_Rect r={.x=0,.y=0,.w=w,.h=h};
        OnEvent(event,r,r);
    }
    void GUIWindow::OnDraw(SDL_Renderer* renderer, SDL_Rect& r)
    {
        SDL_SetRenderDrawColor(renderer,this->palette.background.r,this->palette.background.g,this->palette.background.b,this->palette.background.a);
        SDL_RenderClear(renderer);
        if(this->child != nullptr)
            this->child->OnDraw(renderer,r);
        SDL_RenderPresent(renderer);
    }
    bool GUIWindow::OnEvent(SDL_Event& event, SDL_Rect& myBounds, SDL_Rect& myVisibleBounds)
    {
        
        if(event.type == SDL_EventType::SDL_KEYDOWN && event.key.keysym.sym ==SDL_KeyCode::SDLK_TAB)
        {
            GUISDLEventEventArgs sdle;
            sdle.event = event;
            this->SDLEvent.Invoke(this,sdle);
            TabNext();
            return true;
        }
        if(this->child != nullptr) {  GUISDLEventEventArgs sdle;
        sdle.event = event; this->SDLEvent.Invoke(this,sdle); return this->child->OnEvent(event,myBounds,myVisibleBounds);}
        return View::OnEvent(event,myBounds,myVisibleBounds);
    }
    GUIWindow::GUIWindow(std::string title, int w, int h, Uint32 flags, const GUIPalette& palette) : ContainerView(title)
    {
        this->window = SDL_CreateWindow(title.c_str(),SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,w,h,flags);
        this->renderer = SDL_CreateRenderer(this->window,-1,SDL_RENDERER_ACCELERATED);
        SDL_SetRenderDrawBlendMode(renderer,SDL_BlendMode::SDL_BLENDMODE_BLEND);
        this->child=nullptr;
        this->parent=nullptr;
        this->ownsChild=false;
        this->normal_font=nullptr;
        this->monospaced_font=nullptr;
        gui.windows.push_back(this);
        this->SetPalette(palette);
    }
    void GUIWindow::SetText(std::string text)
    {
        this->text = text;
        SDL_SetWindowTitle(this->window,text.c_str());
    }
    
    void GUIWindow::SetPalette(const GUIPalette& palette)
    {
        this->palette = palette;
        if(this->normal_font != nullptr)
            delete this->normal_font;
        if(this->monospaced_font != nullptr)
            delete this->monospaced_font;
        this->normal_font = new FontCache(this->renderer,this->palette.fontSize);
        this->monospaced_font = new FontCache(this->renderer,AARDVARKFIXEDREGULAR,AARDVARKFIXEDREGULAR_SIZE,this->palette.fontSize);
    }
            
    GUIWindow::~GUIWindow()
    {
        if(this->ownsChild)
            delete this->child;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        
        for(auto index = gui.windows.begin(); index < gui.windows.end(); index++)
        {
            if(*index == this) 
            {
                *index = nullptr;
                break;
            }
        }
    }
    void GUIWindow::SetView(Tesses::Framework::Serialization::Json::JToken item)
    {
        Tesses::Framework::Serialization::Json::JObject dict;
        if(Tesses::Framework::Serialization::Json::TryGetJToken(item,dict))
        {
            Tesses::Framework::Serialization::Json::JObject o;
            Tesses::Framework::Serialization::Json::JObject sz;
            Tesses::Framework::Serialization::Json::JObject pal0;
            std::string title;
            if(dict.TryGetValueAsType("Child",o))
            {
                this->SetView(CreateViewFromJson(o));
            }
            if(dict.TryGetValueAsType("Size",sz))
            {
                int w0,h0;


                SDL_GetWindowSize(this->window,&w0,&h0);
                int64_t w=w0;
                int64_t h=h0;

                sz.TryGetValueAsType("Width",w);
                sz.TryGetValueAsType("Height",h);

                SDL_SetWindowSize(this->window,(int)w,(int)h);
            }
            if(dict.TryGetValueAsType("Palette",pal0))
            {
                bool darkMode=true;
                SDL_Color accent={.r=255,.g=0,.b=0,.a=255};
                std::string _str;
                int64_t fontSize=20; 

                pal0.TryGetValueAsType("IsDarkMode",darkMode);
                pal0.TryGetValueAsType("FontSize",fontSize);
                if(pal0.TryGetValueAsType("Accent",_str))
                    TryParseSDLColor(_str,accent);
                
                GUIPalette pal(darkMode,accent,fontSize);
                if(pal0.TryGetValueAsType("Background",_str))
                    TryParseSDLColor(_str,pal.background);
                if(pal0.TryGetValueAsType("Border",_str))
                    TryParseSDLColor(_str,pal.border_color);
                if(pal0.TryGetValueAsType("BorderActive",_str))
                    TryParseSDLColor(_str,pal.border_active);
                if(pal0.TryGetValueAsType("BorderHover",_str))
                    TryParseSDLColor(_str,pal.border_hover);
                if(pal0.TryGetValueAsType("BorderHoverActive",_str))
                    TryParseSDLColor(_str,pal.border_hover_active);
                this->SetPalette(pal);
            }
            if(dict.TryGetValueAsType("Title",title) || dict.TryGetValueAsType("Text",title))
            {
                this->SetText(title);
            }
        }
    }

    View* GUIWindow::CreateViewFromJson(Tesses::Framework::Serialization::Json::JObject json)
    {
        std::string type;
        if(json.TryGetValueAsType("Type",type))
        {
            std::string id={};
            std::string text={};
            bool active=false;
            json.TryGetValueAsType("Id",id);
            json.TryGetValueAsType("Text",text);
            json.TryGetValueAsType("Active",active);
            if(type == "ButtonView")
            {
                auto btn = new Views::ButtonView(text);
                btn->SetId(id);
                btn->SetViewFlag(VIEWFLAG_ISACTIVE,active);
                return btn;
            }
            else if(type == "CheckView")
            {
                bool checked;
                json.TryGetValueAsType("Checked",checked);
                auto cv = new Views::CheckView(checked,text);
                cv->SetId(id);

                cv->SetViewFlag(VIEWFLAG_ISACTIVE,active);
                return cv;
            }
            else if(type == "LabelView")
            {
                auto lv = new Views::LabelView(text);
                lv->SetId(id);
                return lv;
            }
            else if(type == "ProgressView")
            {
                double v=0;
                json.TryGetValueAsType("Value",v);
                auto pv = new Views::ProgressView(v);
                pv->SetId(id);
                return pv;
            }
            else if(type == "TextListView")
            {
                std::vector<std::string> items;
                Tesses::Framework::Serialization::Json::JArray arr;
                if(json.TryGetValueAsType("Items",arr))
                {
                    std::string str;
                    for(auto item : arr)
                    {
                        if(Tesses::Framework::Serialization::Json::TryGetJToken(item,str)) items.push_back(str);
                    }
                }
                int64_t index=-1;
                int64_t first=0;
                json.TryGetValueAsType("SelectedIndex",index);
                json.TryGetValueAsType("FirstIndex",first);

                auto tlv = new Views::TextListView();

                tlv->SetViewFlag(VIEWFLAG_ISACTIVE,active);
                tlv->SetId(id);
                tlv->items = items;
                tlv->firstIndex = (size_t)first;
                tlv->selected = (int)index;
                return tlv;
            }
            else if(type == "AbsoluteView")
            {
                auto av = new Views::AbsoluteView();
                av->SetId(id);
                Tesses::Framework::Serialization::Json::JArray arr;
                if(json.TryGetValueAsType("Items",arr))
                {
                    for(auto item : arr)
                    {
                        Tesses::Framework::Serialization::Json::JObject dict;
                        if(Tesses::Framework::Serialization::Json::TryGetJToken(item,dict))
                        {
                            Tesses::Framework::Serialization::Json::JObject boundsDict;
                            SDL_Rect r={.x=0,.y=0,.w=200,.h=200};

                            if(dict.TryGetValueAsType("Bounds",boundsDict))
                            {
                                int64_t n;
                                if(boundsDict.TryGetValueAsType("X",n)) r.x = (int)n;
                                if(boundsDict.TryGetValueAsType("Y",n)) r.y = (int)n;
                                if(boundsDict.TryGetValueAsType("Width",n)) r.w = (int)n;
                                if(boundsDict.TryGetValueAsType("Height",n)) r.h = (int)n;
                            }

                            auto myO = CreateViewFromJson(dict);
                            if(myO != nullptr)
                            {
                                av->Add(r,myO);
                            }
                        }
                    }
                }
                return av;
            }
            else {
                GUIJsonViewNotFoundEventArgs e;
                e.destView == nullptr;
                e.jsonObject = json;
                e.typeString = type;
                
                JsonViewNotFound.Invoke(this,e);

                return e.destView;
            }
        }
        return nullptr;
    }

}
#endif