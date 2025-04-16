#include "TessesFramework/Common.hpp"
#include "TessesFramework/Streams/NetworkStream.hpp"
#include <atomic>
#include <csignal>
#if defined(_WIN32)
#include <windows.h>
#undef min
#include <cstdio>
#elif defined(__SWITCH__)
extern "C" {
#include <switch.h>
}
#elif defined(GEKKO)
#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <fat.h>
#include <ogc/pad.h>


#if defined(HW_RVL)
#if defined(TESSESFRAMEWORK_USE_WII_SOCKET)
#include <wiisocket.h>
#endif
#include <wiiuse/wpad.h>
#endif

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

#endif

#if defined(TESSESFRAMEWORK_ENABLE_THREADING)
#include "TessesFramework/Threading/Mutex.hpp"
#endif

namespace Tesses::Framework
{
    
    #if defined(TESSESFRAMEWORK_ENABLE_THREADING) && (defined(GEKKO) || defined(__SWITCH__))
    namespace Threading
    {
        extern void JoinAllThreads();
        extern void LookForFinishedThreads();
    }
    #endif
    volatile static bool isRunningSig=true;
    volatile static std::atomic<bool> isRunning;
    volatile static std::atomic<bool> gaming_console_events=true;
    void TF_ConnectToSelf(uint16_t port)
    {
        Tesses::Framework::Streams::NetworkStream ns("127.0.0.1",port,false,false,false);
        
    }
    bool TF_IsRunning()
    {
        return isRunning;
    }
    static void _sigInt(int c)
    {
        isRunningSig=false;
    }
    void TF_RunEventLoop()
    {
        while(isRunning)
        {
            TF_RunEventLoopItteration();
        }
    }
    #if defined(__SWITCH__)
    bool initedConsole=false;
    PadState default_pad;        
    #endif
    void TF_RunEventLoopItteration()
    {
        #if defined(TESSESFRAMEWORK_ENABLE_THREADING) && (defined(GEKKO) || defined(__SWITCH__))
        Tesses::Framework::Threading::LookForFinishedThreads();
        #endif
    
        if(!isRunningSig) isRunning=false;
        #if defined(GEKKO)
        if(gaming_console_events)
        {
            PAD_ScanPads();
            if(PAD_ButtonsDown(0) & PAD_BUTTON_START) isRunning=false;
        }
        #elif defined(__SWITCH__)
        if(gaming_console_events)
        {
            if(!appletMainLoop()) isRunning=false;
            
            padUpdate(&default_pad);

            u64 kDown = padGetButtonsDown(&default_pad);

            if (kDown & HidNpadButton_Plus)
                isRunning=false;


            if(initedConsole)
            consoleUpdate(NULL);


        }
        #endif
        
    }
    
    void TF_Quit()
    {
        isRunning=false;
        #if defined(TESSESFRAMEWORK_ENABLE_THREADING) && (defined(GEKKO) || defined(__SWITCH__))
        Tesses::Framework::Threading::JoinAllThreads();
        #endif
    }
    void TF_Init()
    {
        #if defined(_WIN32)
        system(" ");
        #endif
        
        isRunning=true;
        #if defined(_WIN32)
            WSADATA wsaData;
            int iResult;

// Initialize Winsock
iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
if (iResult != 0) {
    printf("WSAStartup failed: %d\n", iResult);
    return;
}
        #elif defined(GEKKO)
        fatInitDefault();
        VIDEO_Init();
        PAD_Init();
        #if defined(HW_RVL)
        #if defined(TESSESFRAMEWORK_USE_WII_SOCKET)
        wiisocket_init();
        #endif
        WPAD_Init();
        #endif
        #elif defined(__SWITCH__)
        padConfigureInput(1, HidNpadStyleSet_NpadStandard);
        padInitializeDefault(&default_pad);
        socketInitializeDefault();

        // Initialize the default gamepad (which reads handheld mode inputs as well as the first connected controller)
        #else
        signal(SIGPIPE,SIG_IGN);
        signal(SIGINT,_sigInt);
        #endif
 
    }
    bool TF_GetConsoleEventsEnabled()
    {
        return gaming_console_events;
    }
    void TF_SetConsoleEventsEnabled(bool flag)
    {
        gaming_console_events=flag;
    }
    void TF_InitConsole()
    {
        #if defined(GEKKO)
        rmode = VIDEO_GetPreferredMode(NULL);

        // Allocate memory for the display in the uncached region
        xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

        // Initialise the console, required for printf
        console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
        //SYS_STDIO_Report(true);

        // Set up the video registers with the chosen mode
        VIDEO_Configure(rmode);

        // Tell the video hardware where our display memory is
        VIDEO_SetNextFramebuffer(xfb);

        // Make the display visible
        VIDEO_SetBlack(false);
        // Flush the video register changes to the hardware
        VIDEO_Flush();

        // Wait for Video setup to complete
        VIDEO_WaitVSync();
        if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();


        // The console understands VT terminal escape codes
        // This positions the cursor on row 2, column 0
        // we can use variables for this with format codes too
        // e.g. printf ("\x1b[%d;%dH", row, column );
        printf("\x1b[2;0H");
        #elif defined(__SWITCH__)
        consoleInit(NULL);
        initedConsole=true;
        #endif
    }
    void TF_InitWithConsole()
    {
        TF_Init();
        TF_InitConsole();
        #if defined(TESSESFRAMEWORK_LOGTOFILE)
        printf("Enabled Logging\n");
        #endif
    }
    #if defined(TESSESFRAMEWORK_LOGTOFILE)
    #if defined(TESSESFRAMEWORK_ENABLE_THREADING)
    Tesses::Framework::Threading::Mutex log_mtx;
    #endif
    void TF_Log(std::string txtToLog)
    {
        #if defined(TESSESFRAMEWORK_ENABLE_THREADING)
        log_mtx.Lock();
        #endif
        FILE* f = fopen("TF_Log.log","a");
        fprintf(f,"%s\n",txtToLog.c_str());
        fclose(f);
        #if defined(TESSESFRAMEWORK_ENABLE_THREADING)
        log_mtx.Unlock();
        #endif
    }
    #endif
}