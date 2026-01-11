#pragma once
#include "TessesFramework/Filesystem/VFSFix.hpp"
#include "TessesFramework/Filesystem/LocalFS.hpp"
#include <optional>
namespace Tesses::Framework::Platform::Environment
{
    extern const char EnvPathSeperator;

    struct PortableAppConfig {
        
        std::optional<Tesses::Framework::Filesystem::VFSPath> desktop;
        std::optional<Tesses::Framework::Filesystem::VFSPath> documents;
        std::optional<Tesses::Framework::Filesystem::VFSPath> music;
        std::optional<Tesses::Framework::Filesystem::VFSPath> pictures;
        std::optional<Tesses::Framework::Filesystem::VFSPath> videos;
        std::optional<Tesses::Framework::Filesystem::VFSPath> downloads;
        std::optional<Tesses::Framework::Filesystem::VFSPath> user;
        std::optional<Tesses::Framework::Filesystem::VFSPath> config;
        std::optional<Tesses::Framework::Filesystem::VFSPath> state;
        std::optional<Tesses::Framework::Filesystem::VFSPath> data;
        std::optional<Tesses::Framework::Filesystem::VFSPath> cache;
        std::optional<Tesses::Framework::Filesystem::VFSPath> temp;
     };

    extern PortableAppConfig portable_config;

    namespace SpecialFolders {
        Tesses::Framework::Filesystem::VFSPath GetTemp();
        Tesses::Framework::Filesystem::VFSPath GetHomeFolder();
        Tesses::Framework::Filesystem::VFSPath GetDownloads();
        Tesses::Framework::Filesystem::VFSPath GetMusic();
        Tesses::Framework::Filesystem::VFSPath GetPictures();
        Tesses::Framework::Filesystem::VFSPath GetVideos();
        Tesses::Framework::Filesystem::VFSPath GetDocuments();
        Tesses::Framework::Filesystem::VFSPath GetConfig();
        Tesses::Framework::Filesystem::VFSPath GetDesktop();
        Tesses::Framework::Filesystem::VFSPath GetState();
        Tesses::Framework::Filesystem::VFSPath GetCache();
        Tesses::Framework::Filesystem::VFSPath GetData();
    }

    std::string GetPlatform();

    Tesses::Framework::Filesystem::VFSPath GetRealExecutablePath(Tesses::Framework::Filesystem::VFSPath realPath);

    std::optional<std::string> GetVariable(std::string name);
    void SetVariable(std::string name, std::optional<std::string> var);

    void GetEnvironmentVariables(std::vector<std::pair<std::string,std::string>>& env);
}

