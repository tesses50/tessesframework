#pragma once
#include "TessesFramework/Filesystem/VFSFix.hpp"
#include "TessesFramework/Filesystem/LocalFS.hpp"
#include <optional>
namespace Tesses::Framework::Platform::Environment
{
    extern const char EnvPathSeperator;

    

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

