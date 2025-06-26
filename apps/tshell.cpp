#include "TessesFramework/TessesFramework.hpp"
#include "TessesFramework/Platform/Process.hpp"
using namespace Tesses::Framework;
using namespace Tesses::Framework::Platform;
using namespace Tesses::Framework::TextStreams;
using namespace Tesses::Framework::Filesystem;

void split_command(std::string cmd, std::vector<std::string>& args)
{
    bool inStr=false;
    std::string cur={};
    auto flush=[&]()->void {
        if(cur.empty()) return;
        args.push_back(cur);
        cur={};
    };
    for(size_t i = 0; i < cmd.size(); i++)
    {
        if(inStr)
        {
            if(cmd[i] == '\"') inStr=false;
            else if(cmd[i] == '\\') 
            {
                i++;
                if(i < cmd.size())
                {
                    cmd.push_back(cmd[i]);
                }
            }
            else cur.push_back(cmd[i]);
        }
        else 
        {
            if(cmd[i] == ' ') flush();
            else if(cmd[i] == '\"') {
                inStr=true;
            }
            else {
                cur.push_back(cmd[i]);
            }
        }

    }
    flush();
}

int main(int argc,char** argv)
{
    TF_Init();

    

    while(true)
    {
        std::cout << VFSPath::GetAbsoluteCurrentDirectory().ToString()  << "$ ";


        std::string text;
        StdIn().ReadLine(text);
        std::vector<std::string> args;
        split_command(text,args);
        if(args.empty()) continue;
        if(args[0] == "exit") break;
        else if(args[0] == "echo") { if(args.size() > 1) std::cout << args[1] << std::endl;}
        else if(args[0] == "cd") {
            if(args.size() < 2)
            {
                VFSPath::SetAbsoluteCurrentDirectory(Environment::SpecialFolders::GetHomeFolder());
            }
            else {
                VFSPath::SetAbsoluteCurrentDirectory(args[1]);
            }
        }
        else if(args[0] == "printargs")
        {
            for(size_t i = 1; i < args.size(); i++)
            {
                std::cout << "\"" << Http::HttpUtils::Replace(args[i],"\"","\\\"") << "\"" << std::endl;
            }
        }
        else if (args[0] == "sigtermtest")
        {
            if (args.size() < 2) continue;
            std::vector<std::string> args2(args.begin() + 1, args.end());

            auto path = Environment::GetRealExecutablePath(args2[0]);
            Platform::Process p(path.ToString(), args2);
            if (p.Start())
            {
                std::cout << "Press enter to sigterm" << std::endl;
                StdIn().ReadLine();
                p.Kill(SIGTERM);
            }
        }
        else if(args[0] == "rstdi")
        {
            if(args.size()<3) continue;
            std::vector<std::string> args2(args.begin()+2,args.end());
            auto f = LocalFS.OpenFile(args[1],"rb");
            if(f != nullptr)
            {
                auto path = Environment::GetRealExecutablePath(args2[0]);
                Platform::Process p(path.ToString(),args2);
                p.redirectStdIn=true;
                if(p.Start())
                {
                    auto strm = p.GetStdinStream();
                    f->CopyTo(strm);
                    delete strm;
                    p.CloseStdInNow();
                    p.WaitForExit();
                }
                delete f;
            }
        }
        else {
            auto path = Environment::GetRealExecutablePath(args[0]);
            Platform::Process p(path.ToString(),args);
            if (p.Start())
                p.WaitForExit();
            else
                std::cout << "Failed To Run Process: " << path.ToString() << std::endl;
        }
    }

    TF_Quit();
}