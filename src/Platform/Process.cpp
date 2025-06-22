#include "TessesFramework/Platform/Process.hpp"
#include "TessesFramework/Http/HttpUtils.hpp"
#include "TessesFramework/Platform/Environment.hpp"


#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

namespace Tesses::Framework::Platform {

    class ProcessData {
        public:
            //TODO: Implement for WIN32
            #if defined(_WIN32)
            #elif defined(GEKKO) || defined(__PS2__) || defined(__SWITCH__)
            #else
            int stdin_strm;
            int stdout_strm;
            int stderr_strm;
            pid_t pid;
            #endif
            ProcessData() {
                //TODO: Implement for WIN32
                #if defined(_WIN32)
                
                #elif defined(GEKKO) || defined(__PS2__) || defined(__SWITCH__)
                
                #else
                this->stdin_strm=-1;
                this->stdout_strm = -1;
                this->stderr_strm=-1;
                #endif
            }
    };
    class ProcessStream : public Tesses::Framework::Streams::Stream {
        #if defined(_WIN32)
        #elif defined(GEKKO) || defined(__PS2__) defined(__SWITCH__)
        #else
        int strm;
        
        bool writing;
        bool shouldClose;
        bool eos;
        #endif
        public:
            #if defined(_WIN32)
            #elif defined(GEKKO) || defined(__PS2__) defined(__SWITCH__)
            #else
            ProcessStream(int strm, bool writing, bool shouldClose)
            {
                this->strm = strm;
                this->writing = writing;
                this->shouldClose=shouldClose;
                this->eos=false;
            }
            #endif
            bool EndOfStream()
            {
                //TODO: Implement for WIN32
                #if defined(_WIN32)
                return true;
                #elif defined(GEKKO) || defined(__PS2__) defined(__SWITCH__)
                return true;
                #else
                return this->strm < 0 || eos;
                #endif
            }
            bool CanRead()
            {
                //TODO: Implement for WIN32
                #if defined(_WIN32)
                return false;
                #elif defined(GEKKO) || defined(__PS2__) defined(__SWITCH__)
                return false;
                #else
                return !writing && this->strm > -1;
                #endif
            }
            bool CanWrite()
            {
                //TODO: Implement for WIN32
                #if defined(_WIN32)
                return false;
                #elif defined(GEKKO) || defined(__PS2__) defined(__SWITCH__)
                return false;
                #else
                return writing && this->strm > -1;
                #endif
            }

            size_t Read(uint8_t* buff, size_t sz)
            {
                //TODO: Implement for WIN32
                #if defined(_WIN32)
                return 0;
                #elif defined(GEKKO) || defined(__PS2__) defined(__SWITCH__)
                return 0;
                #else
                if(this->strm < 0 || this->eos && writing) return 0;

                auto r = read(this->strm,buff,sz);
                if(r == -1) return 0;
                if(r == 0 && sz != 0) { this->eos=true; return 0;}
                return (size_t)r;
                #endif
            }

            size_t Write(const uint8_t* buff, size_t sz)
            {
                //TODO: Implement for WIN32
                #if defined(_WIN32)
                return 0;
                #elif defined(GEKKO) || defined(__PS2__) defined(__SWITCH__)
                return 0;
                #else
                if(this->strm < 0 && !writing) return 0;
                auto r = write(this->strm,buff,sz);
                if(r == -1) return 0;
                return (size_t)r;
                #endif
            }



            ~ProcessStream()
            {
                //TODO: Implement for WIN32
                #if defined(_WIN32)
                #elif defined(GEKKO) || defined(__PS2__) defined(__SWITCH__)
                //do nothing                
                #else
                if(this->strm > -1 && this->shouldClose)
                close(this->strm);
                #endif
            }
    };


    Process::Process() : Process("",{},true)
    {
        
    }
    Process::Process(std::string name, std::vector<std::string> args,bool includeThisEnv) : Process(name,args,std::vector<std::pair<std::string,std::string>>(),includeThisEnv)
    {
        
    }
    Process::Process(std::string name, std::vector<std::string> args, std::vector<std::pair<std::string,std::string>> env,bool includeThisEnv)
    {
        this->name = name;
        this->args = args;
        this->env = env;
        this->includeThisEnv = includeThisEnv;
        this->hidden.AllocField<ProcessData>();
    }
    Process::Process(std::string name, std::vector<std::string> args, std::vector<std::string> env,bool includeThisEnv) : Process(name,args,std::vector<std::pair<std::string,std::string>>(),includeThisEnv)
    {
        this->env.resize(env.size());
        for(size_t i =0; i < env.size(); i++)
        {
            auto res=Http::HttpUtils::SplitString(env[i],"=",2);
            if(res.size() == 2)
            {
                this->env[i].first = res[0];
                this->env[i].second = res[1];
            }
            else if(res.size() == 1)
            {

                this->env[i].first = res[0];
                this->env[i].second = "";
            }
        }
    }
    Process::~Process()
    {

    }

    

    bool Process::Start()
    {
        auto p = 
            this->hidden.GetField<ProcessData*>();
        std::vector<std::pair<std::string,std::string>> envs;

        if(this->includeThisEnv)
            Environment::GetEnvironmentVariables(envs);
        
        for(auto itemNew : this->env)
        {
            bool has=false;
            for(auto& item : envs)
            {
                if(item.first == itemNew.first)
                {
                    item.second = itemNew.second;
                    has=true;
                    break;
                }
            }
            if(!has) envs.push_back(itemNew);
        }

        #if defined(_WIN32)
        return false;
        #elif defined(GEKKO) || defined(__PS2__) defined(__SWITCH__)
        return false;
        #else

        int strm_stdin[2];
        int strm_stdout[2];
        int strm_stderr[2];

        if(this->redirectStdIn)
        {
            if(pipe(strm_stdin) == -1) return false;
            p->stdin_strm = strm_stdin[1];
        }
        if(this->redirectStdOut)
        {
            if(pipe(strm_stdout) == -1) 
            {
                if(this->redirectStdIn) 
                {
                    close(strm_stdin[0]);
                    close(strm_stdin[1]);
                }
                return false;
            }

            p->stdout_strm = strm_stdout[0];
        }
        if(this->redirectStdErr)
        {
            if(pipe(strm_stderr) == -1) 
            {
                if(this->redirectStdIn) 
                {
                    close(strm_stdin[0]);
                    close(strm_stdin[1]);
                }
                if(this->redirectStdOut) 
                {
                    close(strm_stdout[0]);
                    close(strm_stdout[1]);
                }
                return false;
            }

            p->stderr_strm = strm_stderr[0];
        }

        auto pid=fork();
        if(pid == -1) {
            if(this->redirectStdIn)
            {
                close(strm_stdin[0]);
                close(strm_stdin[1]);
            }
            
            if(this->redirectStdOut)
            {
                close(strm_stdout[0]);
                close(strm_stdout[1]);
            }

            if(this->redirectStdErr)
            {
                close(strm_stderr[0]);
                close(strm_stderr[1]);
            }
            return false;
        }
        if(pid == 0)
        {
            std::vector<std::string> env2;
            env2.resize(envs.size());

            for(size_t i = 0; i < envs.size(); i++)
            {
                env2[i] = envs[i].first + "=" + envs[i].second;
            }

            char** argv = new char*[args.size()+1];
            argv[args.size()]=NULL;
            char** envp = new char*[env2.size()+1];
            envp[env.size()]=NULL;

            for(size_t i = 0; i < args.size();i++)
            {
                argv[i] = (char*)args[i].c_str();
            }
            for(size_t i = 0; i < env.size();i++)
            {
                envp[i] = (char*)env2[i].c_str();
            }
            if(this->redirectStdIn)
            {
                dup2(strm_stdin[0],0);
                close(strm_stdin[0]);
                close(strm_stdin[1]);
            }
            
            if(this->redirectStdOut)
            {
                dup2(strm_stdout[1],1);
                close(strm_stdout[0]);
                close(strm_stdout[1]);
            }

            if(this->redirectStdErr)
            {
                dup2(strm_stderr[1],2);
                close(strm_stderr[0]);
                close(strm_stderr[1]);
            }
            execve(this->name.c_str(),argv,envp);
            exit(1);
        }
        p->pid = pid;
        if(this->redirectStdIn)
        close(strm_stdin[0]);
        if(this->redirectStdOut)
        close(strm_stdout[1]);
        if(this->redirectStdErr)
        close(strm_stderr[1]);
        return true;
        #endif
    }

    void Process::Kill(int signal)
    {
        #if defined(_WIN32)
        #elif defined(GEKKO) || defined(__PS2__) defined(__SWITCH__)
        #else
        kill(this->hidden.GetField<ProcessData*>()->pid,signal);
        #endif
    }

    int Process::WaitForExit()
    {
        #if defined(_WIN32)
        #elif defined(GEKKO) || defined(__PS2__) defined(__SWITCH__)
        reutnr -1;
        #else
        int r;
        if(waitpid(this->hidden.GetField<ProcessData*>()->pid,&r,0) != -1)
            return r;
        return -1;
        #endif
    }

    Tesses::Framework::Streams::Stream* Process::GetStdinStream(bool closeUnderlying)
    {
        #if defined(_WIN32)
        return nullptr;
        #elif defined(GEKKO) || defined(__PS2__) defined(__SWITCH__)
        return nullptr;
        #else 
        return new ProcessStream(this->hidden.GetField<ProcessData*>()->stdin_strm,true,closeUnderlying);
        #endif
    }
    Tesses::Framework::Streams::Stream* Process::GetStdoutStream(bool closeUnderlying)
    {
        #if defined(_WIN32)
        return nullptr;
        #elif defined(GEKKO) || defined(__PS2__) defined(__SWITCH__)
        return nullptr;
        #else 
        return new ProcessStream(this->hidden.GetField<ProcessData*>()->stdout_strm,false,closeUnderlying);
        #endif
    }
    Tesses::Framework::Streams::Stream* Process::GetStderrStream(bool closeUnderlying)
    {
        #if defined(_WIN32)
        return nullptr;
        #elif defined(GEKKO) || defined(__PS2__) defined(__SWITCH__)
        return nullptr;
        #else 
        return new ProcessStream(this->hidden.GetField<ProcessData*>()->stderr_strm,false,closeUnderlying);
        #endif
    }

}
