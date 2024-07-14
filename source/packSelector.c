// packSelector.c (C)2024 Dakota Thorpe.
// Menu to select sound pack.

// Standard Libs.
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h> 
#include <math.h>

// Wii Specific.
#include <asndlib.h>
#include <gctypes.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <fat.h>

// Our #1 graphics library.
#include <grrlib.h>

// core.
#include "rend/coreEngine.h"
#include "rend/audio.h"
#include "rend/buttons.h"
#include "misc/carhorn_defs.h"

// Built in Data.
#include "font_ttf.h"

#include "std_arrow_left_png.h"
#include "std_arrow_right_png.h"
#include "std_btn_png.h"
#include "wiibg_jpg.h"
#include "cursor_png.h"

#include "button_over_pcm.h"
#include "button_click_pcm.h"

#include "app_funcs.h"
#include "misc/utils.h"

// vars.
bool __packSel_Done = false;
char* __packSel_Pack;
int __packSel_maxPacks = 0;
int __packSel_selPack = 0;

// Sample hover function.
extern void exampleHoverFunction(int argc, char **argv);

// Sample Click Function.
extern void exampleClickFunction(int argc, char **argv);

// Exit function.
extern void onclickExit(int argc, char** argv);

// Next clicked.
void nextBtnClicked(int argc, char** argv) {
    playSfx(button_click_pcm, button_click_pcm_size, 3);
    __packSel_Done = true;
}

// Right arrow clicked.
void __packSel_onRightArrow(int argc, char** argv) {
    playSfx(button_click_pcm, button_click_pcm_size, 3);
    if(__packSel_selPack+2 <= __packSel_maxPacks) {
        __packSel_selPack++;
    }
}

// Right arrow clicked.
void __packSel_onLeftArrow(int argc, char** argv) {
    playSfx(button_click_pcm, button_click_pcm_size, 3);
    if(__packSel_selPack-1 >= 0) {
        __packSel_selPack--;
    }
}

// Menu.
char* SelectPackMenu() {
    // Variables.
    ir_t ir;
    GRRLIB_ttfFont* globalFont;

    // Font.
    globalFont = GRRLIB_LoadTTF(font_ttf, font_ttf_size);

    // Cursor
    GRRLIB_texImg* cursor = GRRLIB_LoadTexture(cursor_png);

    const char *dirPath = "sd:/carhorn/packs"; // Directory path to list
    char **folders = getFolders(dirPath, &__packSel_maxPacks);

    // BG img.
    GRRLIB_texImg* bgimg = GRRLIB_LoadTexture(wiibg_jpg);

    // Get cursor size.
    Size cSize;
    cSize.w = cursor->w;
    cSize.h = cursor->h;

    // Left arrow.
    spritedbtn_t leftArrow = CreateButton(200,200, "", GetStdBtnOptions(__packSel_onLeftArrow,exampleHoverFunction, 20), GetStdBtnAssets(COL_WHITE, COL_BLACK, globalFont));
    leftArrow.assets.btnTexture = GRRLIB_LoadTexture(std_arrow_left_png);
    leftArrow.assets.w = leftArrow.assets.btnTexture->w;
    leftArrow.assets.h = leftArrow.assets.btnTexture->h;

    // Right arrow.
    spritedbtn_t rightArrow = CreateButton(200-65,200, "", GetStdBtnOptions(__packSel_onRightArrow,exampleHoverFunction, 20), GetStdBtnAssets(COL_WHITE, COL_BLACK, globalFont));
    rightArrow.assets.btnTexture = GRRLIB_LoadTexture(std_arrow_right_png);
    rightArrow.assets.w = rightArrow.assets.btnTexture->w;
    rightArrow.assets.h = rightArrow.assets.btnTexture->h;

    // Exit button.
    spritedbtn_t exitBtn = CreateButton(0,1, "Exit", GetStdBtnOptions(onclickExit,exampleHoverFunction, 20), GetStdBtnAssets(COL_WHITE, COL_BLACK, globalFont));
    const Size exitBtnSize = GetButtonSize(exitBtn);
    exitBtn.pnt.y = SCREEN_HEIGHT - exitBtnSize.h; // Place exit button at bottom left.

    // Continue button.
    spritedbtn_t continueBtn = CreateButton(0,0, "Next", GetStdBtnOptions(nextBtnClicked,exampleHoverFunction, 20), GetStdBtnAssets(COL_WHITE, COL_BLACK, globalFont));
    const Size continueBtnSize = GetButtonSize(continueBtn);
    continueBtn.pnt.x = SCREEN_WIDTH - continueBtnSize.w;
    continueBtn.pnt.y = SCREEN_HEIGHT - continueBtnSize.h;

    // Position arrows.

    /* Center Y */
    leftArrow.pnt.y = (SCREEN_HEIGHT / 2) - (leftArrow.assets.h / 2);
    rightArrow.pnt.y = (SCREEN_HEIGHT / 2) - (rightArrow.assets.h / 2);

    /* Place them at their sides. */
    leftArrow.pnt.x = 0;                                    // Left.
    rightArrow.pnt.x = SCREEN_WIDTH - rightArrow.assets.w;  // Right.

    // Menu text.
    char* titleText = "Please Select a Pack:";

    // Function loop.
    while(!__packSel_Done) {
        // Scan for button presses.
        WPAD_ScanPads();
        WPAD_IR(WPAD_CHAN_0, &ir);

        s32 pressed = WPAD_ButtonsDown(WPAD_CHAN_0);

        // Draw BG img.
        GRRLIB_DrawImg(0,0, bgimg, 0, 1,1, COL_WHITE);

        // Title
        GRRLIB_PrintfTTF(0,0, globalFont, titleText, 28, COL_LAVENDER);

        // Pack text.
        char* packText = malloc(1024);
        sprintf(packText, "Avail Packs: %d.", __packSel_maxPacks);
        GRRLIB_PrintfTTF(0,32, globalFont, packText, 28, COL_LAVENDER);
        free(packText);

        // Selected Pack Text.
        char* selPackText = malloc(1024);
        sprintf(selPackText, "Selected Pack %d: %s", __packSel_selPack+1, folders[__packSel_selPack]);
        GRRLIB_PrintfTTF(0,64, globalFont, selPackText, 28, COL_NAVY);
        free(selPackText);

        // Render UI buttons.
        renderSpritedButton(exitBtn);
        renderSpritedButton(leftArrow);
        renderSpritedButton(rightArrow);
        renderSpritedButton(continueBtn);

        // Check the UI buttons.
        checkButtonStatus(&exitBtn, ir.x,ir.y, cSize, pressed);
        checkButtonStatus(&leftArrow, ir.x,ir.y, cSize, pressed);
        checkButtonStatus(&rightArrow, ir.x,ir.y, cSize, pressed);
        checkButtonStatus(&continueBtn, ir.x,ir.y, cSize, pressed);

        // Update.
        GRRLIB_DrawImg(ir.x,ir.y, cursor, 0, 1,1, 0xFFFFFFFF);
        GRRLIB_Render();
    }

    // Free mem.
    GRRLIB_FreeTexture(cursor);
    GRRLIB_FreeTexture(bgimg);

    // Free buttons.
    //FreeButton(leftArrow);
    //FreeButton(rightArrow);
    //FreeButton(exitBtn);
    //FreeButton(continueBtn);
    GRRLIB_FreeTexture(leftArrow.assets.btnTexture);

    __packSel_Done = false;
    return folders[__packSel_selPack];
}