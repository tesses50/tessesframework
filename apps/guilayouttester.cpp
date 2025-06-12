
#if defined(TESSESFRAMEWORK_ENABLE_SDL2)
#include "TessesFramework/SDL2/GUI.hpp"
#include "TessesFramework/SDL2/Views/ButtonView.hpp"
#include "TessesFramework/SDL2/Views/AbsoluteView.hpp"
#include "TessesFramework/SDL2/Views/LabelView.hpp"
#include "TessesFramework/SDL2/Views/TextListView.hpp"
#include "TessesFramework/Filesystem/LocalFS.hpp"
#include "TessesFramework/SDL2/Views/ProgressView.hpp"
#include "TessesFramework/SDL2/Views/CheckView.hpp"
#include "TessesFramework/TextStreams/StreamReader.hpp"
#include "TessesFramework/Streams/FileStream.hpp"
#include <iostream>
using namespace Tesses::Framework;
using namespace Tesses::Framework::SDL2;

using namespace Tesses::Framework::Streams;
using namespace Tesses::Framework::TextStreams;
using namespace Tesses::Framework::Serialization::Json;

void Load(GUIWindow& window, std::string jsonFile)
{
    FILE* f = fopen(jsonFile.c_str(),"rb");
    if(f != NULL)
    {
        FileStream strm(f,true,"rb");
        StreamReader r(&strm,false);
        auto text = r.ReadToEnd();
        try {
            auto obj = Json::Decode(text);
            window.SetView(obj);
        } catch(...)
        {

        }
    }
}
#endif





int main(int argc,char** argv)
{
    #if defined(TESSESFRAMEWORK_ENABLE_SDL2)
    if(argc < 2)
    {
        std::cout << "USAGE: " << argv[0] << " layout.json" << std::endl;
        return 0;
    }
    std::string jsonFile = argv[1];
    TF_Init();

    
    GUIPalette pal0(true,(SDL_Color){.r=255,.g=0,.b=0,.a=255},20);
    GUIWindow window("Press ALT+R to Reload",640,480,SDL_WINDOW_RESIZABLE,pal0);
    
    window.SDLEvent += std::make_shared<FunctionalEvent<View*,GUISDLEventEventArgs&>>([&window,jsonFile](View* sender, GUISDLEventEventArgs& e)->void{
        if(e.event.type == SDL_KEYUP && (uint64_t)(e.event.key.keysym.mod & SDL_Keymod::KMOD_ALT) != 0 && e.event.key.keysym.sym == SDLK_r)
        {
            Load(window,jsonFile);    
        }
    });
    
    Load(window,jsonFile);
    
    

    TF_RunEventLoop();

    TF_Quit();
    #endif
    return 0;
}