#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include "TessesFramework/SDL2/GUI.hpp"
#include "TessesFramework/aardvark-fixed-regular.h"
#include "TessesFramework/SDL2/Views/ButtonView.hpp"
#include "TessesFramework/SDL2/Views/LabelView.hpp"
#include "TessesFramework/SDL2/Views/CheckView.hpp"
#include "TessesFramework/SDL2/Views/ProgressView.hpp"
#include "TessesFramework/SDL2/Views/TextListView.hpp"
#include "TessesFramework/SDL2/Views/AbsoluteView.hpp"
#include "TessesFramework/SDL2/Views/EditTextView.hpp"
#include "TessesFramework/SDL2/Views/PictureView.hpp"
#include "TessesFramework/SDL2/Views/ScrollableTextListView.hpp"
#include "TessesFramework/SDL2/Views/HScrollView.hpp"
#include "TessesFramework/SDL2/Views/VScrollView.hpp"
#include "TessesFramework/SDL2/Views/HStackView.hpp"
#include "TessesFramework/SDL2/Views/VStackView.hpp"
#include "TessesFramework/SDL2/Views/DropDownView.hpp"
#include "TessesFramework/SDL2/Views/TabView.hpp"
#include "TessesFramework/SDL2/ParseColor.hpp"

#if defined(__SWITCH__)
extern "C" {
#include <switch.h>
}
#endif

namespace Tesses::Framework::SDL2 
{
    GUIWindow::operator bool()
    {
        if(this == nullptr) return false;
        for(auto item : gui.windows)
            if(item == this) return true;
        return false;
    }
    void GUIWindow::MakeActive(View* view)
    {
        if(!view->GetViewFlag(VIEWFLAG_TABSTOP)) return;
        if(view->GetViewFlag(VIEWFLAG_ISACTIVE)) return;
        if(this->popups.empty())
        DeactivateAll(this);
        else 
        {
            DeactivateAll(this->popups.back());
        }
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
        for(auto popup : this->popups)
            popup->OnDraw(renderer,popup->bounds);
        SDL_RenderPresent(renderer);
    }
    bool GUIWindow::OnEvent(SDL_Event& event, SDL_Rect& myBounds, SDL_Rect& myVisibleBounds)
    {
        
        if(event.type == SDL_EventType::SDL_KEYDOWN && event.key.keysym.sym ==SDL_KeyCode::SDLK_TAB)
        {
            GUISDLEventEventArgs sdle;
            sdle.event = event;
            this->SDLEvent.Invoke(this,sdle);
            if(this->popups.empty())
                TabNext();
            else 
            {
                TabNextResult nr=TabNextResult::KeepGoing;
                TabNext(this->popups.back(),nr);
                if(nr != TabNextResult::Done)
                {
                    nr = TabNextResult::TabNext;
                    TabNext(this->popups.back(),nr);
                }
            }
            return true;
        }
        if(this->child != nullptr) {  
            GUISDLEventEventArgs sdle;
            sdle.event = event; this->SDLEvent.Invoke(this,sdle);
            retry:
            if(this->popups.empty())
            return this->child->OnEvent(event,myBounds,myVisibleBounds);
            else {
                if(event.type == SDL_MOUSEBUTTONDOWN)
                {
                    auto popup = this->popups.back();
                    if(event.button.x >= popup->bounds.x && event.button.x < popup->bounds.x+popup->bounds.w && event.button.y >= popup->bounds.y && event.button.y < popup->bounds.y+popup->bounds.h)
                    {
                        return popup->OnEvent(event,popup->bounds,popup->bounds);
                    }
                    else if(popup->closeIfClickOutside) {
                        popup->closed=true;
                        this->popups.pop_back();
                        goto retry;
                    }
                    return false;
                }
                else {
                    auto popup = this->popups.back();
                    return popup->OnEvent(event,popup->bounds,popup->bounds);
                    
                }
            }
        }
        retry2:
        if(!this->popups.empty())
        {
            
            if(event.type == SDL_MOUSEBUTTONDOWN)
            {
                auto popup = this->popups.back();
                if(event.button.x >= popup->bounds.x && event.button.x < popup->bounds.x+popup->bounds.w && event.button.y >= popup->bounds.y && event.button.y < popup->bounds.y+popup->bounds.h)
                {
                    return popup->OnEvent(event,popup->bounds,popup->bounds);
                }
                else if(popup->closeIfClickOutside)
                {
                    popup->closed=true;
                    this->popups.pop_back();
                    goto retry2;
                }
                return false;
            }
            else 
            {
                auto popup = this->popups.back();
                return popup->OnEvent(event,popup->bounds,popup->bounds);
                
            }
            
        }
        return View::OnEvent(event,myBounds,myVisibleBounds);
    }
    GUIWindow::GUIWindow(std::string title, int w, int h, Uint32 flags, const GUIPalette& palette) : ContainerView(title)
    {
        TF_LOG("About to create window");

        #if defined(__SWITCH__)
        auto nxwin = nwindowGetDefault();
        w = (int)nxwin->default_width;
        h = (int)nxwin->default_height;
        flags = 0;
        std::string onSwitch = "On switch: " + std::to_string(w) + "x" + std::to_string(h);
        TF_LOG(onSwitch);
      
        #endif
        
        this->window = SDL_CreateWindow(title.c_str(),SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,w,h,flags);
        std::string sdl_window_is_null = "SDL_Window is";

        if(this->window != nullptr)
            sdl_window_is_null.append(" not");
        
        sdl_window_is_null.append(" null");

        if(this->window == nullptr)
        {
            sdl_window_is_null.append(" reason ");
            sdl_window_is_null.append(SDL_GetError());
        }
        
        TF_LOG(sdl_window_is_null);
        /*#if defined(__SWITCH__) || defined(__PS2__) || defined(GEKKO)

        this->renderer = SDL_CreateRenderer(this->window,-1,SDL_RENDERER_SOFTWARE);
        #else*/
        TF_LOG("About to create renderer");
        this->renderer = SDL_CreateRenderer(this->window,-1,SDL_RENDERER_ACCELERATED);
        sdl_window_is_null = "SDL_Renderer is";

        if(this->renderer!= nullptr)
            sdl_window_is_null.append(" not");
        
        sdl_window_is_null.append(" null");

        if(this->renderer == nullptr)
        {
            sdl_window_is_null.append(" reason ");
            sdl_window_is_null.append(SDL_GetError());
        }
        
        TF_LOG(sdl_window_is_null);
        //#endif
        SDL_SetRenderDrawBlendMode(renderer,SDL_BlendMode::SDL_BLENDMODE_BLEND);
        TF_LOG("Renderer blend mode set");
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

    void GUIWindow::ShowPopup(GUIPopup* popup)
    {
        popup->closed=false;
        bool has = false;
        for(auto item : this->popups)
            if(item == popup) { has=true; break;}
        if(!has)
            this->popups.push_back(popup);

        
        
        AssignChildParentToThis(popup);

        auto v = popup->GetView();
        if(v != nullptr) this->MakeActive(v);

        while(!popup->IsClosed() && TF_IsRunning())
        {
            TF_RunEventLoopItteration();
        }
        if(!this->popups.empty() && this->popups.back() == popup) 
            this->popups.pop_back();
        popup->closed=true;
    }
    void GUIWindow::ShowPopup(GUIPopup& popup)
    {
        ShowPopup(&popup);
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
                    TryParseSDLColor(_str,pal.borderColor);
                if(pal0.TryGetValueAsType("BorderActive",_str))
                    TryParseSDLColor(_str,pal.borderActive);
                if(pal0.TryGetValueAsType("BorderHover",_str))
                    TryParseSDLColor(_str,pal.borderHover);
                if(pal0.TryGetValueAsType("BorderHoverActive",_str))
                    TryParseSDLColor(_str,pal.borderHoverActive);
                this->SetPalette(pal);
            }
            if(dict.TryGetValueAsType("Title",title) || dict.TryGetValueAsType("Text",title))
            {
                this->SetText(title);
            }
        }
    }
    SDL_Window* GUIWindow::GetSDLWindow()
    {
        return this->window;
    }
    SDL_Renderer* GUIWindow::GetSDLRenderer()
    {
        return this->renderer;
    }

    static int szStr2size(std::string sz)
    {
        if(sz.empty()) return GUI_MIN;
        if(sz == "min") return GUI_MIN;
        if(sz == "*") return GUI_EXPAND;
        if(sz[0] == '*') {
            return GUI_EXPAND_N(std::stoi(sz.substr(1)));
        }
        else {
            return std::stoi(sz);
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
            else if(type == "ScrollableTextListView")
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

                auto tlv = new Views::ScrollableTextListView();

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
            else if(type == "EditTextView")
            {
                auto etv = new Views::EditTextView();
                etv->SetId(id);
                etv->SetText(text);
                std::string hint;
                json.TryGetValueAsType("Hint",hint);
                etv->SetHint(hint);
                return etv;
            }
            else if(type == "PictureView")
            {
                auto pv = new Views::PictureView();
                pv->SetId(id);
                return pv;
            }
            else if(type == "DropDownView")
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

                auto tlv = new Views::DropDownView();

                tlv->SetViewFlag(VIEWFLAG_ISACTIVE,active);
                tlv->SetId(id);
                tlv->GetItems() = items;
                
                tlv->SetIndex((int)index);
                return tlv;
            }
            else if(type == "VScrollView")
            {
                int64_t value=0;
                int64_t min = 0;
                int64_t max = 10;
                json.TryGetValueAsType("Value",value);

                json.TryGetValueAsType("Min",min);

                json.TryGetValueAsType("Max",max);
                auto vscroll=new Views::VScrollView();
                vscroll->value = (uint64_t)value;
                vscroll->min = (uint64_t)min;
                vscroll->max = (uint64_t)max;
                vscroll->SetId(id);
                return vscroll;
            }
            else if(type == "HScrollView")
            {
                int64_t value=0;
                int64_t min = 0;
                int64_t max = 10;
                json.TryGetValueAsType("Value",value);

                json.TryGetValueAsType("Min",min);

                json.TryGetValueAsType("Max",max);
                auto hscroll=new Views::VScrollView();
                hscroll->value = (uint64_t)value;
                hscroll->min = (uint64_t)min;
                hscroll->max = (uint64_t)max;
                hscroll->SetId(id);
                return hscroll;
            }
            else if(type == "VStackView")
            {
                auto sv = new Views::VStackView();
                sv->SetId(id);
                Tesses::Framework::Serialization::Json::JArray arr;
                if(json.TryGetValueAsType("Items",arr))
                {
                    for(auto item : arr)
                    {
                        Tesses::Framework::Serialization::Json::JObject dict;
                        if(Tesses::Framework::Serialization::Json::TryGetJToken(item,dict))
                        {
                            std::string n="min";

                            dict.TryGetValueAsType("Size",n);


                            auto myO = CreateViewFromJson(dict);
                            if(myO != nullptr)
                            {
                                sv->Add(szStr2size(n),myO);
                            }
                        }
                    }
                }
                return sv;
            }
            else if(type == "HStackView")
            {
                auto sv = new Views::HStackView();
                sv->SetId(id);
                Tesses::Framework::Serialization::Json::JArray arr;
                if(json.TryGetValueAsType("Items",arr))
                {
                    for(auto item : arr)
                    {
                        Tesses::Framework::Serialization::Json::JObject dict;
                        if(Tesses::Framework::Serialization::Json::TryGetJToken(item,dict))
                        {
                            std::string n="min";

                            dict.TryGetValueAsType("Size",n);


                            auto myO = CreateViewFromJson(dict);
                            if(myO != nullptr)
                            {
                                sv->Add(szStr2size(n),myO);
                            }
                        }
                    }
                }
                return sv;
            }
            else if(type == "TabView")
            {
                //Item.TabLabel
                auto tv = new Views::TabView();
                tv->SetId(id);
                int64_t firstTab=0;
                int64_t curTab=0;
                bool tabsVisible=true;

                json.TryGetValueAsType("SelectedTab",curTab);

                json.TryGetValueAsType("FirstTab",firstTab);
                json.TryGetValueAsType("TabsVisible", tabsVisible);

                tv->firstTab = (int64_t)firstTab;
                tv->selectedTab = (int64_t)curTab;
                tv->tabsVisible=tabsVisible;


                Tesses::Framework::Serialization::Json::JArray arr;

                if(json.TryGetValueAsType("Items",arr))
                {
                    for(auto item : arr)
                    {
                        Tesses::Framework::Serialization::Json::JObject dict;
                        if(Tesses::Framework::Serialization::Json::TryGetJToken(item,dict))
                        {
                            std::string n="";

                            dict.TryGetValueAsType("TabText",n);


                            auto myO = CreateViewFromJson(dict);
                            if(myO != nullptr)
                            {
                                tv->AddTab(n,myO,true);
                            }
                        }
                    }
                }
                return tv;
            }
            else {
                GUIJsonViewNotFoundEventArgs e;
                e.destView = nullptr;
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