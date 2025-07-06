
#if defined(TESSESFRAMEWORK_ENABLE_SDL2)

#include "TessesFramework/Streams/MemoryStream.hpp"
#include "TessesFramework/Http/HttpClient.hpp"
#include "TessesFramework/SDL2/Stream.hpp"
#include "TessesFramework/SDL2/GUI.hpp"
#include "TessesFramework/SDL2/Views/ButtonView.hpp"
#include "TessesFramework/SDL2/Views/AbsoluteView.hpp"
#include "TessesFramework/SDL2/Views/LabelView.hpp"
#include "TessesFramework/SDL2/Views/ScrollableTextListView.hpp"
#include "TessesFramework/Filesystem/LocalFS.hpp"
#include "TessesFramework/SDL2/Views/ProgressView.hpp"
#include "TessesFramework/SDL2/Views/CheckView.hpp"
#include "TessesFramework/SDL2/Views/EditTextView.hpp"
#include "TessesFramework/SDL2/Views/PictureView.hpp"
#include "TessesFramework/SDL2/Views/VScrollView.hpp"
#include "TessesFramework/SDL2/Views/HScrollView.hpp"
#include "TessesFramework/SDL2/Views/VStackView.hpp"
#include "TessesFramework/SDL2/Views/HStackView.hpp"
#include "TessesFramework/SDL2/Views/DropDownView.hpp"
#include "TessesFramework/SDL2/SDL_Headers.hpp"

#include <iostream>
using namespace Tesses::Framework;
using namespace Tesses::Framework::SDL2;

#endif


void LoadImage(Views::PictureView& view)
{
    using namespace Tesses::Framework::Streams;
    using namespace Tesses::Framework::Http;
    MemoryStream strm(true);
    
    DownloadToStreamSimple("https://s.ytimg.com/vi/lItUxNQnzME/maxresdefault.jpg",strm);
    strm.Seek(0L,SeekOrigin::Begin);
    auto res = RwopsFromStream(&strm,false);
    view.SetPicture(IMG_Load_RW(res,1),true);
}

int main(int argc,char** argv)
{
    #if defined(TESSESFRAMEWORK_ENABLE_SDL2)

    TF_Init();

    //std::cout << GUI_EXPAND_N(argc) << std::endl;
    

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

   
    GUIPalette pal0(darkMode,colors[color_index % colors.size()].first,20,2);
    TF_LOG("Create pallete");
    GUIWindow* window = new GUIWindow("My Window Title",1280,720,SDL_WINDOW_RESIZABLE,pal0);
    TF_LOG("Created GUIWindow success");

    Views::LabelView lbl("A random label\nThat spans lines.");

    Views::ButtonView btn("Dark Mode");
    Views::ButtonView btn2(colors[0].second);
    Views::ButtonView btn3("Popup");
    Views::ButtonView btn4("Window");
    Views::ProgressView progress(42.42);

    Views::CheckView cv(false,"Checkbox");
    Views::CheckView cv2(false,"Another Checkbox");
    Views::EditTextView edit("Enter some text");
    Views::DropDownView ddv;
    ddv.GetItems()={
        "Al Gore",
        "Demi Lovato",
        "Steve Ballmer"
    };


    Views::ScrollableTextListView list;
    for(int i = 0; i < 100; i++)
    {
        list.items.push_back(std::to_string(i));
    }
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

    Views::VScrollView vscroll(0,0,10);
    Views::HScrollView hscroll(0,0,10);

    Views::LabelView lbl2("ScrollPos");

    vscroll.ValueChanged += std::make_shared<FunctionalEvent<View*,GUIEventArgs&>>(
        [&](View* sender,GUIEventArgs& e)->void {
            lbl2.SetText("ScrollPos: " + std::to_string(vscroll.value));
        }
    );

    //Views::PictureView img;
    //LoadImage(img);

    abs.Add({.x=1280-42,.y=2,.w=32,.h=720-20},&vscroll,false);

    abs.Add({.x=720,.y=720-100,.w=200,.h=100},&lbl2,false);


    Views::VStackView vstack;
    vstack.Add(GUI_MIN,&edit,false);
    vstack.Add(GUI_MIN,&btn3,false);
    vstack.Add(32,&ddv,false);
    vstack.Add(GUI_MIN,&btn4,false);

    vstack.Add(GUI_EXPAND,&btn,false);

    

    vstack.Add(GUI_EXPAND_N(10),&list,false);
    vstack.Add(GUI_EXPAND,&btn2,false);

    vstack.Add(GUI_MIN,&hscroll,false);

    Views::HStackView hstack;
    hstack.Add(GUI_EXPAND,&vstack,false);
    hstack.Add(GUI_MIN,&vscroll,false);
    
    window->SetView(&hstack,false);
    //window.SetView(&abs,false);
    window->SDLEvent += std::make_shared<FunctionalEvent<View*,GUISDLEventEventArgs&>>([&window,&lbl](View* sender, GUISDLEventEventArgs& e)->void {
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
        window->SetPalette(palette);
    });
    //"A random label\nThat spans lines."

    btn2.Click += std::make_shared<FunctionalEvent<View*,GUIEventArgs&>>([&window,&darkMode,&color_index,&colors,&btn2](View* sender, GUIEventArgs& e)->void{
        color_index++;
        btn2.SetText(colors[color_index % colors.size()].second);
        
        GUIPalette palette(darkMode,colors[color_index % colors.size()].first,20);
        window->SetPalette(palette);
    });

    Views::LabelView lbl3;

    GUIContainerPopup popup(42,42,300,200);
    popup.SetView(&lbl3,false);
    
    btn3.Click += std::make_shared<FunctionalEvent<View*,GUIEventArgs&>>([&](View* sender, GUIEventArgs& e)->void{
        lbl3.SetText(edit.GetText());
        window->ShowPopup(popup);
    });

    //window("My Window Title",1280,720,SDL_WINDOW_RESIZABLE,pal0);
     btn4.Click += std::make_shared<FunctionalEvent<View*,GUIEventArgs&>>([&](View* sender, GUIEventArgs& e)->void{
        new GUIWindow("My Second Window",640,480,0,pal0);
    });
    

    TF_RunEventLoop();

    TF_Quit();
    #endif
    return 0;
}
