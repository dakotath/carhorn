// coreEngine.c (C)2024 Dakota Thorpe.

// Standard Libs.
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Wii Specific.
#include <asndlib.h>
#include <wiiuse/wpad.h>
#include <gctypes.h>
#include <gccore.h>
#include <fat.h>

// Our #1 graphics library.
#include <grrlib.h>

/**
 * @author Dakota Thorpe
 * @paragraph APPINIT_P0 Initializes Wii Hardware.
*/
void CoreEngine_Init() {
    // Init audio.
    ASND_Init();
    
    // Init Graphics.
    GRRLIB_Init();

    // Init SD card.
    fatInitDefault();

    // debug Log.
    #ifdef DEBUG
        globalLog = fopen("globallog.log", "w");
        if(globalLog != NULL) canLog = true;
    #endif

    // Init Wii Remote.
    WPAD_Init();
    WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR); // Set the WPAD API IR format. Only call on startup.
}