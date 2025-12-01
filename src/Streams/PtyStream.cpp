#include "TessesFramework/Streams/PtyStream.hpp"
#if !defined(GEKKO) && !defined(__PS2__) && !defined(_WIN32) && !defined(__SWITCH__) && !defined(__FreeBSD__) && defined(TESSESFRAMEWORK_ENABLE_PROCESS)
#if defined(__APPLE__)
#include <util.h>
#else
#include <pty.h>
#endif
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>


#endif
namespace Tesses::Framework::Streams {
    PtyStream::PtyStream(WindowSize windowSize,std::string filename, std::vector<std::string> args, std::vector<std::string> env)
    {
        #if !defined(GEKKO) && !defined(__PS2__) && !defined(_WIN32) && !defined(__SWITCH__) && !defined(__FreeBSD__) && defined(TESSESFRAMEWORK_ENABLE_PROCESS)
        this->wS = windowSize;
        this->eos=false;
        winsize sz;
        sz.ws_col =(unsigned short)windowSize.Columns;
        sz.ws_row = (unsigned short)windowSize.Rows;
        sz.ws_xpixel = (unsigned short)windowSize.Width;
        sz.ws_ypixel = (unsigned short)windowSize.Height;
        termios ios;
        cfmakeraw(&ios);

        pid= forkpty(&this->socket,NULL,&ios,&sz);
        if(pid == -1)
        {
            this->eos=true;
        }
        if(pid == 0)
        {
            char** argv = new char*[args.size()+1];
            argv[args.size()]=NULL;
            char** envp = new char*[env.size()+1];
            envp[env.size()]=NULL;

            for(size_t i = 0; i < args.size();i++)
            {
                argv[i] = (char*)args[i].c_str();
            }
            for(size_t i = 0; i < env.size();i++)
            {
                envp[i] = (char*)env[i].c_str();
            }

            if(execve(filename.c_str(),argv,envp) == -1)
            {
                perror("execve returned -1");
                exit(1);
            }
        }
        int flags = fcntl(this->socket, F_GETFL, 0);
        if(flags == -1) {
            perror("fcntl F_GETFL");
            this->eos=true;
            return;
        }
        flags |= O_NONBLOCK;

        flags=fcntl(this->socket,F_SETFL,flags);
        if(flags == -1) {
                    perror("fcntl F_SETFL");

            this->eos=true;
            return;
        }
        #endif
    }
    bool PtyStream::EndOfStream()
    {
        return this->eos;
    }
    bool PtyStream::CanRead()
    {
        return true;
    }
    bool PtyStream::CanWrite()
    {
        return true;
    }
    size_t PtyStream::Read(uint8_t* buff, size_t sz)
    {
        if(this->eos) return 0;
        #if !defined(GEKKO) && !defined(__PS2__) && !defined(_WIN32) && !defined(__SWITCH__) && !defined(__FreeBSD__) && defined(TESSESFRAMEWORK_ENABLE_PROCESS)
        auto res = read(this->socket, buff,sz);
        
        if(res == -1)
        {
            if(errno != EAGAIN && errno != EWOULDBLOCK)
            this->eos=true;
            return 0;
        }
        return (size_t)res;
        #else
        return 0;
        #endif
    }
    size_t PtyStream::Write(const uint8_t* buff, size_t sz)
    {
        #if !defined(GEKKO) && !defined(__PS2__) && !defined(_WIN32) && !defined(__SWITCH__) && !defined(__FreeBSD__) && defined(TESSESFRAMEWORK_ENABLE_PROCESS)
        auto res = write(this->socket, buff,sz);
        return res;
        #else
        return 0;
        #endif
    }
    WindowSize PtyStream::GetWindowSize()
    {
        return this->wS;
    }
    void PtyStream::Resize(WindowSize windowSize)
    {
        #if !defined(GEKKO) && !defined(__PS2__) && !defined(_WIN32) && !defined(__SWITCH__) && !defined(__FreeBSD__) && defined(TESSESFRAMEWORK_ENABLE_PROCESS)
        this->wS = windowSize;
        winsize sz;
        sz.ws_col =(unsigned short)windowSize.Columns;
        sz.ws_row = (unsigned short)windowSize.Rows;
        sz.ws_xpixel = (unsigned short)windowSize.Width;
        sz.ws_ypixel = (unsigned short)windowSize.Height;
        
        ioctl(this->socket,TIOCSWINSZ,&sz);
        #endif
    }
    PtyStream::~PtyStream()
    {
        Close();
    }
    void PtyStream::Close()
    {
        if(this->eos) return;
        this->eos=true; 
        #if !defined(GEKKO) && !defined(__PS2__) && !defined(_WIN32) && !defined(__SWITCH__) && !defined(__FreeBSD__) && defined(TESSESFRAMEWORK_ENABLE_PROCESS)
        close(this->socket);
       
        kill((pid_t)this->pid,SIGHUP);
        #endif
    }
}