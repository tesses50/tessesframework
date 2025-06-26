#include "TessesFramework/Platform/Environment.hpp"
#include "TessesFramework/Http/HttpUtils.hpp"
#if defined(TESSESFRAMEWORK_ENABLE_PLATFORMFOLDERS)
#include "sago/platform_folders.h"
#endif
#if defined(_WIN32)
#include <windows.h>
#include "TessesFramework/Filesystem/VFSFix.hpp"
#include "TessesFramework/Text/StringConverter.hpp"
using namespace Tesses::Framework::Text::StringConverter;
#endif
 #if !defined(_WIN32)
        extern char** environ;
    #endif

using namespace Tesses::Framework::Filesystem;
namespace Tesses::Framework::Platform::Environment
{
    #if defined(_WIN32)
    const char EnvPathSeperator=';';
    #else
    const char EnvPathSeperator=':';
    #endif

    namespace SpecialFolders 
    {

        VFSPath GetHomeFolder()
        {
            #if defined(TESSESFRAMEWORK_ENABLE_PLATFORMFOLDERS) && !defined(SAGO_DISABLE)
            return sago::getHomeDir();
            #elif defined(__EMSCRIPTEN__)
            return (std::string)"/home/web_user";
            #else
            return (std::string)"/TF_User";
            #endif
        }
        VFSPath GetDownloads()
        {
            #if defined(TESSESFRAMEWORK_ENABLE_PLATFORMFOLDERS) && !defined(SAGO_DISABLE)
            return sago::getDownloadFolder();
            #else
            return GetHomeFolder() / "Downloads";
            #endif
        }
        VFSPath GetMusic()
        {
            #if defined(TESSESFRAMEWORK_ENABLE_PLATFORMFOLDERS) && !defined(SAGO_DISABLE)
            return sago::getMusicFolder();
            #else
            return GetHomeFolder() / "Music";
            #endif
        }
        VFSPath GetPictures()
        {
            #if defined(TESSESFRAMEWORK_ENABLE_PLATFORMFOLDERS) && !defined(SAGO_DISABLE)
            return sago::getPicturesFolder();
            #else
            return GetHomeFolder() / "Pictures";
            #endif
        }
        VFSPath GetVideos()
        {
            #if defined(TESSESFRAMEWORK_ENABLE_PLATFORMFOLDERS) && !defined(SAGO_DISABLE)
            return sago::getVideoFolder();
            #else
            return GetHomeFolder() / "Videos";
            #endif
        }
        VFSPath GetDocuments()
        {
            #if defined(TESSESFRAMEWORK_ENABLE_PLATFORMFOLDERS) && !defined(SAGO_DISABLE)
            return sago::getDocumentsFolder();
            #else
            return GetHomeFolder() / "Documents";
            #endif
        }
        VFSPath GetConfig()
        {
            #if defined(TESSESFRAMEWORK_ENABLE_PLATFORMFOLDERS) && !defined(SAGO_DISABLE)
            return sago::getConfigHome();
            #else
            return GetHomeFolder() / "Config";
            #endif
        }
        VFSPath GetDesktop()
        {
            #if defined(TESSESFRAMEWORK_ENABLE_PLATFORMFOLDERS) && !defined(SAGO_DISABLE)
            return sago::getDesktopFolder();
            #else
            return GetHomeFolder() / "Desktop";
            #endif
        }
        VFSPath GetState()
        {
            #if defined(TESSESFRAMEWORK_ENABLE_PLATFORMFOLDERS) && !defined(SAGO_DISABLE)
            return sago::getStateDir();
            #else
            return GetHomeFolder() / "State";
            #endif
        }
        VFSPath GetCache()
        {
            #if defined(TESSESFRAMEWORK_ENABLE_PLATFORMFOLDERS) && !defined(SAGO_DISABLE)
            return sago::getCacheDir();
            #else
            return GetHomeFolder() / "Cache";
            #endif
        }
        VFSPath GetData()
        {
            #if defined(TESSESFRAMEWORK_ENABLE_PLATFORMFOLDERS) && !defined(SAGO_DISABLE)
            return sago::getDataHome();
            #else
            return GetHomeFolder() / "Data";
            #endif
        }
    }

    VFSPath GetRealExecutablePath(VFSPath realPath)
    {
        using namespace Tesses::Framework::Http;
        
        if(!realPath.relative) return realPath.MakeAbsolute();
        if(LocalFS.FileExists(realPath)) return realPath.MakeAbsolute();
        const char* path = std::getenv("PATH");
        #if defined(_WIN32)
        const char* pathext = std::getenv("PATHEXT");
        auto pext = HttpUtils::SplitString(pathext,";");
        pext.push_back({});
        auto pathParts = HttpUtils::SplitString(path,";");
        for(auto item : pathParts)
        {

            auto newPath = LocalFS.SystemToVFSPath(item) / realPath;
            for(auto item2 : pext)
            {
                auto newPathExt = newPath + item2;
                if(LocalFS.FileExists(newPathExt)) return newPathExt;
            }
            if(LocalFS.FileExists(newPath)) return newPath;
        }
        return realPath;
        #else
        
        auto pathParts = HttpUtils::SplitString(path,":");
        for(auto item : pathParts)
        {
            auto newPath = LocalFS.SystemToVFSPath(item) / realPath;
            if(LocalFS.FileExists(newPath)) return newPath;
        }
        return realPath.MakeAbsolute();
        #endif
    }

    std::optional<std::string> GetVariable(std::string name)
    {
        auto res = std::getenv(name.c_str());
        if(res == nullptr) return std::nullopt;
        std::string value = res;
        return value;
    }
    void SetVariable(std::string name, std::optional<std::string> var)
    {
        if (var)
        #if defined(_WIN32)
        {
            std::u16string nameu16 = {};

            std::u16string varu16 = {};

            UTF16::FromUTF8(nameu16, name);
            UTF16::FromUTF8(varu16, var.value());
            SetEnvironmentVariableW((LPCWSTR)nameu16.c_str(),(LPCWSTR)varu16.c_str());
        }
        #else
                setenv(name.c_str(), var->c_str(),1);
        #endif
        else
        #if defined(_WIN32)
        {
            std::u16string nameu16 = {};

            UTF16::FromUTF8(nameu16, name);

            SetEnvironmentVariableW((LPCWSTR)nameu16.c_str(),NULL);
        }
        #else
        unsetenv(name.c_str());
        #endif
        
    }

   
    void GetEnvironmentVariables(std::vector<std::pair<std::string,std::string>>& env)
    {
        #if defined(_WIN32)
            auto environ0 = GetEnvironmentStringsW();
            auto envthing = environ0;
            while(*envthing)
            {
                std::u16string str = (const char16_t*)envthing;
                std::string stru8;
                UTF8::FromUTF16(stru8, str);
                auto items = Http::HttpUtils::SplitString(stru8, "=", 2);
                if(items.size() == 2)
                {
                    
                    env.push_back(std::pair<std::string,std::string>(items[0],items[1]));
                }
                else if(items.size() == 1)
                {
                    env.push_back(std::pair<std::string,std::string>(items[0],""));
                }
                envthing += str.size() + 1;
            }   
            FreeEnvironmentStringsW(environ0);
        #else
            for(char** envthing = environ; *envthing != NULL; envthing++)
            {
                //if(*envthing == NULL) break;
                auto items = Http::HttpUtils::SplitString(*envthing,"=",2);
                if(items.size() == 2)
                {
                    
                    env.push_back(std::pair<std::string,std::string>(items[0],items[1]));
                }
                else if(items.size() == 1)
                {
                    env.push_back(std::pair<std::string,std::string>(items[0],""));
                }

            }
        #endif
    }
    std::string GetPlatform()
    {
        #if defined(__SWITCH__)
            return "Nintendo Switch";
        #endif
        #if defined(__PS2__)
            return "PlayStation 2";
        #endif
        #if defined(GEKKO)
            #if defined(HW_RVL)
                return "Nintendo Wii";
            #endif
            return "Nintendo Gamecube";
        #endif
        #if defined(WIN32) || defined(_WIN32)
        return "Windows";
        #endif
        #if defined(linux)
        return "Linux";
        #endif

        #ifdef __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_MAC
        return "MacOS";
    #endif
    #if TARGET_OS_IOS
        return "iOS";
    #endif
    #if TARGET_OS_TV
        return "Apple TV";
    #endif

    #if TARGET_OS_WATCH
        return "Apple Watch";
    #endif

    #if __EMSCRIPTEN__
        return "WebAssembly";
    #endif

    return "Unknown Apple Device";
#endif

        return "Unknown";
    }
}