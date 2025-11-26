#include "TessesFramework/Platform/Process.hpp"
#include "TessesFramework/Http/HttpUtils.hpp"
#include "TessesFramework/Platform/Environment.hpp"
#include <iostream>

#if defined(_WIN32)
extern "C" {
#include <windows.h>
}
#include "TessesFramework/Filesystem/VFSFix.hpp"
#include "TessesFramework/Text/StringConverter.hpp"
using namespace Tesses::Framework::Text::StringConverter;
static void escape_windows_args(std::string& str, std::vector<std::string> args)
{
    bool first = true;
    for (auto item : args)
    {
        if (first)
        {
            str.push_back('\"');
            first = false;
        }
        else {
            str.append(" \"");
        }

        for (auto c : item)
        {
            if (c == '"') str.append("\\\"");
            else str.push_back(c);
        }

        str.push_back('\"');
    }
}



#else
#include <unistd.h>
#include <sys/wait.h>


#endif



namespace Tesses::Framework::Platform {

    class ProcessData : public HiddenFieldData {
        public:
            //TODO: Implement for WIN32
            #if defined(_WIN32)
            STARTUPINFOW si;
            PROCESS_INFORMATION pi;

            HANDLE stdin_strm;
            HANDLE stdout_strm;
            HANDLE stderr_strm;

             static int __stdcall KillGraceFully(HWND hndle, LPARAM arg)
            {
                auto pd=static_cast<ProcessData*>((void*)arg);
                DWORD curProc;
                GetWindowThreadProcessId(hndle, &curProc);
                if (curProc == pd->pi.dwProcessId)
                {
                    PostMessage(hndle, WM_CLOSE, 0, 0);

                    return true;
                }
                return false;
            }
            

            #elif defined(GEKKO) || defined(__PS2__) || defined(__SWITCH__) || !defined(TESSESFRAMEWORK_ENABLE_PROCESS)
            #else
            int stdin_strm;
            int stdout_strm;
            int stderr_strm;
            pid_t pid;
            #endif
            
            
            ProcessData() {
                //TODO: Implement for WIN32
                #if defined(_WIN32)
                stdin_strm = NULL;
                stdout_strm = NULL;
                stderr_strm = NULL;

               

                #elif defined(GEKKO) || defined(__PS2__) || defined(__SWITCH__) || !defined(TESSESFRAMEWORK_ENABLE_PROCESS)
                
                #else
                this->stdin_strm=-1;
                this->stdout_strm = -1;
                this->stderr_strm=-1;
                #endif
            }
    };
    class ProcessStream : public Tesses::Framework::Streams::Stream {
        #if defined(_WIN32)
        HANDLE strm;
        bool writing;
        bool eos;
        #elif defined(GEKKO) || defined(__PS2__) || defined(__SWITCH__) || !defined(TESSESFRAMEWORK_ENABLE_PROCESS)
        #else
        int strm;
        
        bool writing;
        bool eos;
        #endif
        public:
            #if defined(_WIN32)
            ProcessStream(HANDLE strm, bool writing)
            {
                this->strm = strm;
                this->writing = writing;
                this->eos = false;
            }
            #elif defined(GEKKO) || defined(__PS2__) || defined(__SWITCH__) || !defined(TESSESFRAMEWORK_ENABLE_PROCESS)
            #else
            ProcessStream(int strm, bool writing)
            {
                this->strm = strm;
                this->writing = writing;
                this->eos=false;
            }
            #endif
            bool EndOfStream()
            {
                //TODO: Implement for WIN32
                #if defined(_WIN32)
                return this->strm == NULL || eos;
                #elif defined(GEKKO) || defined(__PS2__) || defined(__SWITCH__) || !defined(TESSESFRAMEWORK_ENABLE_PROCESS)
                return true;
                #else
                return this->strm < 0 || eos;
                #endif
            }
            bool CanRead()
            {
                //TODO: Implement for WIN32
                #if defined(_WIN32)
                return !writing && this->strm != NULL;
                #elif defined(GEKKO) || defined(__PS2__) || defined(__SWITCH__) || !defined(TESSESFRAMEWORK_ENABLE_PROCESS)
                return false;
                #else
                return !writing && this->strm > -1;
                #endif
            }
            bool CanWrite()
            {
                //TODO: Implement for WIN32
                #if defined(_WIN32)

                return writing && this->strm != NULL;
                #elif defined(GEKKO) || defined(__PS2__) || defined(__SWITCH__) || !defined(TESSESFRAMEWORK_ENABLE_PROCESS)
                return false;
                #else
                return writing && this->strm > -1;
                #endif
            }

            size_t Read(uint8_t* buff, size_t sz)
            {
                //TODO: Implement for WIN32
                #if defined(_WIN32)
                if (this->strm == NULL || this->eos && writing) return 0;

                DWORD dataR = (DWORD)sz;
                if (!ReadFile(this->strm, buff, dataR, &dataR, NULL))
                    return 0;
                if (dataR == 0) {
                    this->eos = true;
                }
                return (size_t)dataR;
                #elif defined(GEKKO) || defined(__PS2__) || defined(__SWITCH__) || !defined(TESSESFRAMEWORK_ENABLE_PROCESS)
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
                if (this->strm == NULL || !writing) return 0;
                DWORD dataW=(DWORD)sz;
                if (!WriteFile(this->strm, buff, dataW, &dataW, NULL))
                    return 0;
                
                return (size_t)dataW;
                #elif defined(GEKKO) || defined(__PS2__) || defined(__SWITCH__) || !defined(TESSESFRAMEWORK_ENABLE_PROCESS)
                return 0;
                #else
                if(this->strm < 0 || !writing) return 0;
                auto r = write(this->strm,buff,sz);
                if(r == -1) return 0;
                return (size_t)r;
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
    bool Process::HasExited()
    {
        if (this->exited) return true;
        ProcessData* p = this->hidden.GetField<ProcessData*>();

        #if defined(_WIN32)
        if (WaitForSingleObject(p->pi.hProcess, 0) == WAIT_OBJECT_0)
        {
            DWORD ec = 0;
            GetExitCodeProcess(p->pi.hProcess,&ec);
            this->exitCode = (int)ec;
            this->exited = true;
            return true;
        }
       #elif defined(GEKKO) || defined(__PS2__) || defined(__SWITCH__) || !defined(TESSESFRAMEWORK_ENABLE_PROCESS)
       return true;
       #else       
        int r;
        if (waitpid(this->hidden.GetField<ProcessData*>()->pid, &r, WNOHANG) != -1)
        {
            this->exited = true;
            this->exitCode = r;
            return r;
        }
        #endif
        return false;
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
    void Process::CloseStdInNow()
    {
        ProcessData* p = this->hidden.GetField<ProcessData*>();
        
        #if defined(_WIN32)
        if (p->stdin_strm != NULL)
        {
            CloseHandle(p->stdin_strm);
            p->stdin_strm = NULL;
        }
        #elif defined(GEKKO) || defined(__PS2__) || defined(__SWITCH__) || !defined(TESSESFRAMEWORK_ENABLE_PROCESS)
               
        #else
        if (p->stdin_strm > -1)
        {
            close(p->stdin_strm);
            p->stdin_strm = -1;
        }
        #endif

    }
    Process::~Process()
    {
        
        ProcessData* p = this->hidden.GetField<ProcessData*>();
       
        #if defined(_WIN32)
        if(!this->exited)
        Kill(SIGTERM);
        if (p->stdin_strm != NULL)
            CloseHandle(p->stdin_strm);

        if (p->stdout_strm != NULL)
            CloseHandle(p->stdout_strm);

        if (p->stderr_strm != NULL)
            CloseHandle(p->stderr_strm);
        

        CloseHandle(p->pi.hProcess);
        CloseHandle(p->pi.hThread);
        #elif defined(GEKKO) || defined(__PS2__) || defined(__SWITCH__) || !defined(TESSESFRAMEWORK_ENABLE_PROCESS)
               
        #else
        if (!this->exited)
        {
            Kill(SIGTERM);
            WaitForExit();
        }
        
        if (p->stdin_strm != -1)
            close(p->stdin_strm);
        if (p->stdout_strm != -1)
            close(p->stdout_strm);
        if (p->stderr_strm != -1)
            close(p->stderr_strm);
        #endif  
    }

    

    bool Process::Start()
    {
        ProcessData* p = 
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


        ZeroMemory(&p->si, sizeof(p->si));
        p->si.cb = sizeof(p->si);
        ZeroMemory(&p->pi, sizeof(p->pi));
        std::u16string u16_name;
        std::u16string u16_args;
        std::string args;
        escape_windows_args(args,this->args);
        UTF16::FromUTF8(u16_name,this->name);
        UTF16::FromUTF8(u16_args, args);
        std::u16string env = {};
        
        for (auto envItem : envs)
        {
            auto partOld = envItem.first + "=" + envItem.second;
            std::u16string part = {};

            UTF16::FromUTF8(part,partOld);
            env.append(part);
            env.push_back(0);
        }
        env.push_back(0);

        std::u16string workDir = {};

        if (!this->workingDirectory.empty())
            UTF16::FromUTF8(workDir, this->workingDirectory);
        
        SECURITY_ATTRIBUTES attr;
        attr.nLength = sizeof(attr);
        attr.lpSecurityDescriptor = NULL;
        attr.bInheritHandle = true;
        p->si.hStdInput = NULL;
        p->si.hStdOutput = NULL;

        p->si.hStdError = NULL;

        p->stdin_strm = NULL;
        p->stdout_strm = NULL;
        p->stderr_strm = NULL;
        if (this->redirectStdIn || this->redirectStdOut || this->redirectStdErr)
        {
            p->si.dwFlags |= STARTF_USESTDHANDLES;
            if (!this->redirectStdIn)
            {
                p->si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
            }
            if (!this->redirectStdOut)
            {
                
                p->si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
            }
            if (!this->redirectStdOut)
            {
                p->si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
            }
        }

        if (this->redirectStdIn)
        {
            if (!CreatePipe(&p->si.hStdInput, &p->stdin_strm, &attr,0)) return false;

            SetHandleInformation(p->stdin_strm, HANDLE_FLAG_INHERIT, 0);
        }

        if (this->redirectStdOut)
        {
            if (!CreatePipe(&p->stdout_strm, &p->si.hStdOutput, &attr, 0))
            {
                if (this->redirectStdIn)
                {
                    CloseHandle(p->stdin_strm);
                    CloseHandle(p->si.hStdInput);
                }
                return false;
            }

            SetHandleInformation(p->stdout_strm, HANDLE_FLAG_INHERIT, 0);
        }
        if (this->redirectStdErr)
        {
            if (!CreatePipe(&p->stderr_strm, &p->si.hStdError, &attr, 0))
            {
                if (this->redirectStdIn)
                {
                    CloseHandle(p->stdin_strm);
                    CloseHandle(p->si.hStdInput);
                }
                if (this->redirectStdOut)
                {
                    CloseHandle(p->stdout_strm);
                    CloseHandle(p->si.hStdOutput);
                }
                return false;
            }

            SetHandleInformation(p->stderr_strm, HANDLE_FLAG_INHERIT, 0);
        }
        
        if (!CreateProcessW((LPCWSTR)u16_name.c_str(), (LPWSTR)u16_args.data(), NULL, NULL, (this->redirectStdIn || this->redirectStdOut || this->redirectStdErr), CREATE_UNICODE_ENVIRONMENT, (LPVOID)env.c_str(), workDir.empty() ? (LPCWSTR)NULL : (LPCWSTR)workDir.c_str(), &(p->si), &(p->pi)))
        {
            if (this->redirectStdIn)
            {
                CloseHandle(p->stdin_strm);
                CloseHandle(p->si.hStdInput);
            }
            if (this->redirectStdOut)
            {
                CloseHandle(p->stdout_strm);
                CloseHandle(p->si.hStdOutput);
            }
            if (this->redirectStdErr)
            {
                CloseHandle(p->stderr_strm);
                CloseHandle(p->si.hStdError);
            }
            return false;
        }

        /*
            BOOL
WINAPI
CreateProcessW(
    _In_opt_ LPCWSTR lpApplicationName,
    _Inout_opt_ LPWSTR lpCommandLine,
    _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
    _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
    _In_ BOOL bInheritHandles,
    _In_ DWORD dwCreationFlags,
    _In_opt_ LPVOID lpEnvironment,
    _In_opt_ LPCWSTR lpCurrentDirectory,
    _In_ LPSTARTUPINFOW lpStartupInfo,
    _Out_ LPPROCESS_INFORMATION lpProcessInformation
    );
    */
        return true;
        #elif defined(GEKKO) || defined(__PS2__) || defined(__SWITCH__) || !defined(TESSESFRAMEWORK_ENABLE_PROCESS)
               
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
            envp[env2.size()]=NULL;

            for(size_t i = 0; i < args.size();i++)
            {
                argv[i] = (char*)args[i].c_str();
            }
            for(size_t i = 0; i < env2.size();i++)
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
            if(!this->workingDirectory.empty())
                Tesses::Framework::Filesystem::VFSPath::SetAbsoluteCurrentDirectory(this->workingDirectory);
            execvpe(this->name.c_str(),argv,envp);
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
        if (signal != SIGKILL && signal != SIGTERM) std::cout << "WARN: We terminated the process" << std::endl;

        if (signal == SIGTERM)
        {
            auto win = this->hidden.GetField<ProcessData*>();
            if (EnumWindows(ProcessData::KillGraceFully, (LPARAM)(void*)win))
            {
                if (WaitForSingleObject(win->pi.hProcess, 60000) != WAIT_OBJECT_0)
                {
                    PostThreadMessage(win->pi.dwThreadId, WM_QUIT, 0, 0);
                }
            }
            else {
                PostThreadMessage(win->pi.dwThreadId, WM_QUIT, 0, 0);
            }
        }
        else
        TerminateProcess(this->hidden.GetField<ProcessData*>()->pi.hProcess,-1);
        
        #elif defined(GEKKO) || defined(__PS2__) || defined(__SWITCH__) || !defined(TESSESFRAMEWORK_ENABLE_PROCESS)
               
        #else
        kill(this->hidden.GetField<ProcessData*>()->pid,signal);
        #endif
    }

    int Process::WaitForExit()
    {
        if (this->exited) return this->exitCode;
        #if defined(_WIN32)
        auto p = this->hidden.GetField<ProcessData*>();
        WaitForSingleObject(p->pi.hProcess, INFINITE);
        DWORD ret=0;
        GetExitCodeProcess(p->pi.hThread, &ret);
        this->exitCode = (int)ret;
        this->exited = true;
        return (int)ret;

        #elif defined(GEKKO) || defined(__PS2__) || defined(__SWITCH__) || !defined(TESSESFRAMEWORK_ENABLE_PROCESS)
               
        return -1;
        #else
        int r;
        if (waitpid(this->hidden.GetField<ProcessData*>()->pid, &r, 0) != -1)
        {
            this->exited = true;
            this->exitCode = r;
            return r;
        }
        return -1;
        #endif
    }

    std::shared_ptr<Tesses::Framework::Streams::Stream> Process::GetStdinStream()
    {
        if (this->exited) return nullptr;
        #if defined(GEKKO) || defined(__PS2__) || defined(__SWITCH__) || !defined(TESSESFRAMEWORK_ENABLE_PROCESS)
               
        return nullptr;
        #else 
        return std::make_shared<ProcessStream>(this->hidden.GetField<ProcessData*>()->stdin_strm,true);
        #endif
    }
    std::shared_ptr<Tesses::Framework::Streams::Stream> Process::GetStdoutStream()
    {

        if (this->exited) return nullptr;
        #if defined(GEKKO) || defined(__PS2__) || defined(__SWITCH__) || !defined(TESSESFRAMEWORK_ENABLE_PROCESS)
               return nullptr;
        #else 
        return std::make_shared<ProcessStream>(this->hidden.GetField<ProcessData*>()->stdout_strm,false);
        #endif
    }
    std::shared_ptr<Tesses::Framework::Streams::Stream> Process::GetStderrStream()
    {

        if (this->exited) return nullptr;
        #if defined(GEKKO) || defined(__PS2__) || defined(__SWITCH__) || !defined(TESSESFRAMEWORK_ENABLE_PROCESS)
        return nullptr;
        #else 
        return std::make_shared<ProcessStream>(this->hidden.GetField<ProcessData*>()->stderr_strm,false);
        #endif
    }

}
