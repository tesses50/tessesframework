#include "TessesFramework/Common.hpp"
#include "TessesFramework/Streams/NetworkStream.hpp"
#include <atomic>
#include <csignal>
#if defined(GEKKO)
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

namespace Tesses::Framework
{
    volatile static bool isRunningSig=true;
    volatile static std::atomic<bool> isRunning;
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
    void TF_RunEventLoopItteration()
    {
    
        if(!isRunningSig) isRunning=false;
        #if defined(GEKKO)
        PAD_ScanPads();
        if(PAD_ButtonsDown(0) & PAD_BUTTON_START) isRunning=false;
        #endif
    }
    
    void TF_Quit()
    {
        isRunning=false;
    }
    
    void TF_Init()
    {
    
        
        isRunning=true;
        #if defined(GEKKO)
        fatInitDefault();
        VIDEO_Init();
        PAD_Init();
        #if defined(HW_RVL)
        #if defined(TESSESFRAMEWORK_USE_WII_SOCKET)
        wiisocket_init();
        #endif
        WPAD_Init();
        #endif
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
        #else
        signal(SIGPIPE,SIG_IGN);
        signal(SIGINT,_sigInt);
        #endif
    }
}