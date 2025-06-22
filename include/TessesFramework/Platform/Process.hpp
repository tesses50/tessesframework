#include <vector>
#include <string>
#include "TessesFramework/Streams/Stream.hpp"
#include "TessesFramework/HiddenField.hpp"

namespace Tesses::Framework::Platform {
class Process {
    private:
        HiddenField hidden;
    public:
        std::string name;
        std::vector<std::string> args;
        std::vector<std::pair<std::string,std::string>> env;
        bool includeThisEnv;
        bool redirectStdIn=false;
        bool redirectStdOut=false;
        bool redirectStdErr=false;
        //YOU ARE RESPONSABLE FOR FREEING THIS STREAM
        Tesses::Framework::Streams::Stream* GetStdinStream(bool closeUnderlying=true);
        //YOU ARE RESPONSABLE FOR FREEING THIS STREAM
        Tesses::Framework::Streams::Stream* GetStdoutStream(bool closeUnderlying=true);
        //YOU ARE RESPONSABLE FOR FREEING THIS STREAM
        Tesses::Framework::Streams::Stream* GetStderrStream(bool closeUnderlying=true);

        Process();
        Process(std::string name, std::vector<std::string> args,bool includeThisEnv=true);
        Process(std::string name, std::vector<std::string> args, std::vector<std::pair<std::string,std::string>> env,bool includeThisEnv=false);
        Process(std::string name, std::vector<std::string> args, std::vector<std::string> env,bool includeThisEnv=false);
        

        bool Start();

        void Kill(int signal);

        int WaitForExit();

        ~Process();
};
}