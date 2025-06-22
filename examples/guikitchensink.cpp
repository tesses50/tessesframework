
#if defined(TESSESFRAMEWORK_ENABLE_SDL2)

#define SDL_MAIN_HANDLED
#include "TessesFramework/SDL2/GUI.hpp"
#include "TessesFramework/SDL2/Views/ButtonView.hpp"
#include "TessesFramework/SDL2/Views/AbsoluteView.hpp"
#include "TessesFramework/SDL2/Views/LabelView.hpp"
#include "TessesFramework/SDL2/Views/TextListView.hpp"
#include "TessesFramework/Filesystem/LocalFS.hpp"
#include "TessesFramework/SDL2/Views/ProgressView.hpp"
#include "TessesFramework/SDL2/Views/CheckView.hpp"
#include "TessesFramework/SDL2/Views/EditTextView.hpp"
#include <iostream>
using namespace Tesses::Framework;
using namespace Tesses::Framework::SDL2;
#endif




int main(int argc,char** argv)
{
    #if defined(TESSESFRAMEWORK_ENABLE_SDL2)

    TF_Init();

    std::vector<std::pair<SDL_Color,std::string>> colors={
        std::pair<SDL_Color,std::string>({.r=255,.g=0,.b=128,.a=255},"Magenta"),
        std::pair<SDL_Color,std::string>({.r=255,.g=0,.b=0,.a=255}, "Red"),
        std::pair<SDL_Color,std::string>({.r=0,.g=255,.b=0,.a=255}, "Green"),
        std::pair<SDL_Color,std::string>({.r=0,.g=0,.b=255,.a=255}, "Blue"),
        std::pair<SDL_Color,std::string>({.r=255,.g=255,.b=0,.a=255}, "Yellow"),
        std::pair<SDL_Color,std::string>({.r=143, .g=188, .b=143,.a=255},"Dark sea Green"),
        std::pair<SDL_Color,std::string>({.r=63, .g=253, .b=170,.a=255},"Aqua")
    };
    
    bool darkMode=true;
    size_t color_index=0;

    GUIPalette pal0(darkMode,colors[color_index % colors.size()].first,20);
    TF_LOG("Create pallete");
    GUIWindow window("My Window Title",1280,720,SDL_WINDOW_RESIZABLE,pal0);
    TF_LOG("Created GUIWindow success");

    Views::LabelView lbl("A random label\nThat spans lines.");

    Views::ButtonView btn("Dark Mode");
    Views::ButtonView btn2(colors[0].second);
    Views::ProgressView progress(42.42);

    Views::CheckView cv(false,"Checkbox");
    Views::CheckView cv2(false,"Another Checkbox");
    Views::EditTextView edit("Enter some text");


    Views::TextListView list;
    /*for(auto item : Tesses::Framework::Filesystem::LocalFS.EnumeratePaths((std::string)"/usr/bin"))
    {
        list.items.push_back(item.GetFileName());
    }*/
    


    //Views::LabelView labelView("My Sweet Label");

    Views::AbsoluteView abs;
    abs.Add({.x=0,.y=0,.w=400,.h=64},&lbl,false);
    abs.Add({.x=32,.y=64,.w=200,.h=50},&btn,false);
    abs.Add({.x=248,.y=64,.w=200,.h=50},&cv,false);

    abs.Add({.x=32,.y=134,.w=200,.h=50},&btn2,false);

    abs.Add({.x=248,.y=134,.w=200,.h=50},&cv2,false);
    

    abs.Add({.x=32,.y=190,.w=200,.h=30},&progress,false);

    abs.Add({.x=32,.y=230,.w=320,.h=240},&list,false);
    
    abs.Add({.x=32,.y=478,.w=300,.h=200},&edit,false);

    window.SetView(&abs,false);
    window.SDLEvent += std::make_shared<FunctionalEvent<View*,GUISDLEventEventArgs&>>([&window,&lbl](View* sender, GUISDLEventEventArgs& e)->void {
        std::string sdl2_event = "SDL_Event: " + std::to_string(e.event.type);
        TF_LOG(sdl2_event);
        if(e.event.type == SDL_EventType::SDL_WINDOWEVENT)
        {
            if(e.event.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                std::string text ="A random label\nThat spans lines, WindowSize: "
                +std::to_string(e.event.window.data1) + "x" + std::to_string(e.event.window.data2) + ".";
                lbl.SetText(text);
            }
        }
    });
    btn.Click += std::make_shared<FunctionalEvent<View*,GUIEventArgs&>>([&window,&darkMode,&color_index,&colors,&btn](View* sender, GUIEventArgs& e)->void{
        darkMode = !darkMode;
        btn.SetText(darkMode ? "Light Mode" : "Dark Mode");
        GUIPalette palette(darkMode,colors[color_index % colors.size()].first,20);
        window.SetPalette(palette);
    });
    //"A random label\nThat spans lines."

    btn2.Click += std::make_shared<FunctionalEvent<View*,GUIEventArgs&>>([&window,&darkMode,&color_index,&colors,&btn2](View* sender, GUIEventArgs& e)->void{
        color_index++;
        btn2.SetText(colors[color_index % colors.size()].second);
        
        GUIPalette palette(darkMode,colors[color_index % colors.size()].first,20);
        window.SetPalette(palette);
    });
    
    

    TF_RunEventLoop();

    TF_Quit();
    #endif
    return 0;
}
