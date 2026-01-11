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
    PortableAppConfig portable_config;

    namespace SpecialFolders 
    {
        VFSPath GetTemp()
        {

            if(portable_config.temp)
                return *portable_config.temp;
            return std::filesystem::temp_directory_path().string();
        }
        VFSPath GetHomeFolder()
        {

            if(portable_config.user)
                return *portable_config.user;

            #if defined(TESSESFRAMEWORK_ENABLE_PLATFORMFOLDERS) && !defined(SAGO_DISABLE)
            return sago::getHomeDir();
            #elif defined(__EMSCRIPTEN__)
            return (std::string)"/home/web_user";
            #elif defined(__ANDROID__)
            return (std::string)"/sdcard/TF_User";
            #else
            return (std::string)"/TF_User";
            #endif
        }
        VFSPath GetDownloads()
        {
            if(portable_config.downloads)
                return *portable_config.downloads;

            #if defined(TESSESFRAMEWORK_ENABLE_PLATFORMFOLDERS) && !defined(SAGO_DISABLE)
            return sago::getDownloadFolder();
            #elif defined(__ANDROID__)
            return (std::string)"/sdcard/Download";
            #else
            return GetHomeFolder() / "Downloads";
            #endif
        }
        VFSPath GetMusic()
        {

            if(portable_config.music)
                return *portable_config.music;
            #if defined(TESSESFRAMEWORK_ENABLE_PLATFORMFOLDERS) && !defined(SAGO_DISABLE)
            return sago::getMusicFolder();
            #elif defined(__ANDROID__)
            return (std::string)"/sdcard/Music";
            #else
            return GetHomeFolder() / "Music";
            #endif
        }
        VFSPath GetPictures()
        {

            if(portable_config.pictures)
                return *portable_config.pictures;
            #if defined(TESSESFRAMEWORK_ENABLE_PLATFORMFOLDERS) && !defined(SAGO_DISABLE)
            return sago::getPicturesFolder();
            #elif defined(__ANDROID__)
            return (std::string)"/sdcard/Pictures";
            #else
            return GetHomeFolder() / "Pictures";
            #endif
        }
        VFSPath GetVideos()
        {
            if(portable_config.videos)
                return *portable_config.videos;
            #if defined(TESSESFRAMEWORK_ENABLE_PLATFORMFOLDERS) && !defined(SAGO_DISABLE)
            return sago::getVideoFolder();
            #elif defined(__ANDROID__)
            return (std::string)"/sdcard/Movies";
            #else
            return GetHomeFolder() / "Videos";
            #endif
        }
        VFSPath GetDocuments()
        {

            if(portable_config.documents)
                return *portable_config.documents;
            #if defined(TESSESFRAMEWORK_ENABLE_PLATFORMFOLDERS) && !defined(SAGO_DISABLE)
            return sago::getDocumentsFolder();
            #elif defined(__ANDROID__)
            return (std::string)"/sdcard/Documents";
            #else
            return GetHomeFolder() / "Documents";
            #endif
        }
        VFSPath GetConfig()
        {

            if(portable_config.config)
                return *portable_config.config;
            #if defined(TESSESFRAMEWORK_ENABLE_PLATFORMFOLDERS) && !defined(SAGO_DISABLE)
            return sago::getConfigHome();
            #else
            return GetHomeFolder() / "Config";
            #endif
        }
        VFSPath GetDesktop()
        {
            if(portable_config.desktop)
                return *portable_config.desktop;
            #if defined(TESSESFRAMEWORK_ENABLE_PLATFORMFOLDERS) && !defined(SAGO_DISABLE)
            return sago::getDesktopFolder();
            #else
            return GetHomeFolder() / "Desktop";
            #endif
        }
        VFSPath GetState()
        {

            if(portable_config.state)
                return *portable_config.state;
            #if defined(TESSESFRAMEWORK_ENABLE_PLATFORMFOLDERS) && !defined(SAGO_DISABLE)
            return sago::getStateDir();
            #else
            return GetHomeFolder() / "State";
            #endif
        }
        VFSPath GetCache()
        {

            if(portable_config.cache)
                return *portable_config.cache;
            #if defined(TESSESFRAMEWORK_ENABLE_PLATFORMFOLDERS) && !defined(SAGO_DISABLE)
            return sago::getCacheDir();
            #else
            return GetHomeFolder() / "Cache";
            #endif
        }
        VFSPath GetData()
        {

            if(portable_config.data)
                return *portable_config.data;
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

        
        if(!realPath.relative) return realPath;
        if(LocalFS->FileExists(realPath)) return realPath.MakeAbsolute();
        const char* path = std::getenv("PATH");
        #if defined(_WIN32)
        const char* pathext = std::getenv("PATHEXT");
        auto pext = HttpUtils::SplitString(pathext,";");
        pext.push_back({});
        auto pathParts = HttpUtils::SplitString(path,";");
        for(auto item : pathParts)
        {

            auto newPath = LocalFS->SystemToVFSPath(item) / realPath;
            for(auto item2 : pext)
            {
                auto newPathExt = newPath + item2;
                if(LocalFS->FileExists(newPathExt)) return newPathExt;
            }
            if(LocalFS->FileExists(newPath)) return newPath;
        }
        return realPath;
        #else
        
        auto pathParts = HttpUtils::SplitString(path,":");
        for(auto item : pathParts)
        {
            auto newPath = LocalFS->SystemToVFSPath(item) / realPath;
            if(LocalFS->FileExists(newPath)) return newPath;
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
        #if defined(__ANDROID__)
            return "Android";
        #endif
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