
#if defined(TESSESFRAMEWORK_ENABLE_SDL2)

#define SDL_MAIN_HANDLED
#include "TessesFramework/Platform/Environment.hpp"
#include "TessesFramework/Serialization/SQLite.hpp"
#include "TessesFramework/SDL2/GUI.hpp"
#include "TessesFramework/SDL2/Views/ButtonView.hpp"
#include "TessesFramework/SDL2/Views/AbsoluteView.hpp"
#include "TessesFramework/SDL2/Views/LabelView.hpp"
#include "TessesFramework/SDL2/Views/TextListView.hpp"
#include "TessesFramework/SDL2/Views/ProgressView.hpp"
#include "TessesFramework/SDL2/Views/CheckView.hpp"
#include "TessesFramework/SDL2/Views/EditTextView.hpp"


using namespace Tesses::Framework::Filesystem;
using namespace Tesses::Framework::SDL2;
using namespace Tesses::Framework::Serialization;
using namespace Tesses::Framework;
using namespace Tesses::Framework::Platform::Environment::SpecialFolders;


VFSPath DatabasePath()
{
    auto path = GetConfig() / "Tesses" / "Framework" / "Examples";
    LocalFS.CreateDirectory(path);
    return path / "databaseex.sqlite";
}

void CreateTable()
{
    SQLiteDatabase db(DatabasePath());
    db.Exec("CREATE TABLE IF NOT EXISTS names (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT);");
}
void ClearTable()
{
    SQLiteDatabase db(DatabasePath());
    db.Exec("DROP TABLE names;");
    db.Exec("CREATE TABLE IF NOT EXISTS names (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT);");

}

void LoadDB(std::vector<std::string>& items)
{
    SQLiteDatabase db(DatabasePath());

    for(auto item : db.Exec("SELECT name FROM names;"))
    {
        for(auto kvp : item)
        {
            if(kvp.first == "name" && kvp.second)
                items.push_back(kvp.second.value());
        }
    }

}
void Append(std::string item)
{
    SQLiteDatabase db(DatabasePath());
    db.Exec("INSERT INTO names (name) VALUES (" + SQLiteDatabase::Escape(item) + ");");
}

int main(int argc,char** argv)
{
    std::string json = "{\"Child\":{\"Items\":[{\"Bounds\":{\"Height\":50,\"Width\":440,\"X\":16,\"Y\":16},\"Hint\":\"Enter name\",\"Id\":\"name\",\"Type\":\"EditTextView\"},{\"Bounds\":{\"Height\":50,\"Width\":70,\"X\":470,\"Y\":17},\"Id\":\"addBtn\",\"Text\":\"Add\",\"Type\":\"ButtonView\"},{\"Bounds\":{\"Height\":50,\"Width\":70,\"X\":550,\"Y\":17},\"Id\":\"clearBtn\",\"Text\":\"Clear\",\"Type\":\"ButtonView\"},{\"Bounds\":{\"Height\":382,\"Width\":608,\"X\":16,\"Y\":82},\"Id\":\"listView\",\"Type\":\"TextListView\"}],\"Type\":\"AbsoluteView\"}}";

    TF_Init();
    CreateTable();
    GUIPalette pal(true,{.r=255,.g=0,.b=0,.a=255});
    
    GUIWindow window("Names",640,480,0,pal);
    window.SetView(Json::Json::Decode(json));

    auto name = dynamic_cast<Views::EditTextView*>(window.FindViewById("name"));
    auto addBtn = dynamic_cast<Views::ButtonView*>(window.FindViewById("addBtn"));
    auto clearBtn = dynamic_cast<Views::ButtonView*>(window.FindViewById("clearBtn"));

    auto listView = dynamic_cast<Views::TextListView*>(window.FindViewById("listView"));

    if(name == nullptr || addBtn == nullptr || clearBtn == nullptr || listView == nullptr) 
    {
        TF_Quit();
        return 1;
    }

    LoadDB(listView->items);
    addBtn->Click += std::make_shared<FunctionalEvent<View*,GUIEventArgs&>>([&](View* view,GUIEventArgs& e)->void {
        std::string text = name->GetText();
        if(!text.empty())
        {
            name->SetText("");
            Append(text);
            listView->items.push_back(text);
        }
    });
    

    clearBtn->Click += std::make_shared<FunctionalEvent<View*,GUIEventArgs&>>([&](View* view,GUIEventArgs& e)->void {
        listView->items.clear();
        ClearTable();
    });
     
    

    TF_RunEventLoop();
    TF_Quit();
}

#endif