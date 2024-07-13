// carhorn.c - (c)2024 Dakota Thorpe.

// Standard Libs.
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>

// Wii Specific.
#include <asndlib.h>
#include <gctypes.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <fat.h>

// Our #1 graphics library.
#include <grrlib.h>

// OGG Player lib.
#include "oggplayer.h"

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
#include "cursor_png.h"

#include "wwu_ogg.h"
#include "button_over_pcm.h"
#include "button_click_pcm.h"

// Variables.
ir_t ir;
GRRLIB_ttfFont* globalFont;

// Sample hover function.
void exampleHoverFunction(int argc, char **argv) {
    printf("Hovered!\n");
    playSfx(button_over_pcm, button_over_pcm_size, 3);
}

// Sample Click Function.
void exampleClickFunction(int argc, char **argv) {
    printf("Clicked!\n");
    playSfx(button_click_pcm, button_click_pcm_size, 3);
}

// Exit function.
void onclickExit(int argc, char** argv) {
    exit(0);
}

// Main code.
int main(int argc, char** argv)
{
    // Init.
    CoreEngine_Init();

    // Font.
    globalFont = GRRLIB_LoadTTF(font_ttf, font_ttf_size);

    // Cursor
    GRRLIB_texImg* cursor = GRRLIB_LoadTexture(cursor_png);
    Size cSize;
    cSize.w = cursor->w;
    cSize.h = cursor->h;

    // Test arrow 1.
    spritedbtn_t leftArrow = CreateButton(200,200, "", GetStdBtnOptions(exampleClickFunction,exampleHoverFunction, 20), GetStdBtnAssets(COL_WHITE, COL_BLACK, globalFont));
    leftArrow.assets.btnTexture = GRRLIB_LoadTexture(std_arrow_left_png);
    leftArrow.assets.w = leftArrow.assets.btnTexture->w;
    leftArrow.assets.h = leftArrow.assets.btnTexture->h;

    // Test arrow 2.
    spritedbtn_t rightArrow = CreateButton(200-65,200, "", GetStdBtnOptions(exampleClickFunction,exampleHoverFunction, 20), GetStdBtnAssets(COL_WHITE, COL_BLACK, globalFont));
    rightArrow.assets.btnTexture = GRRLIB_LoadTexture(std_arrow_right_png);
    rightArrow.assets.w = rightArrow.assets.btnTexture->w;
    rightArrow.assets.h = rightArrow.assets.btnTexture->h;

    // Position arrows.
    /* Center Y */
    leftArrow.pnt.y = (SCREEN_HEIGHT / 2) - (leftArrow.assets.h / 2);
    rightArrow.pnt.y = (SCREEN_HEIGHT / 2) - (rightArrow.assets.h / 2);
    /* Place them at their sides. */
    leftArrow.pnt.x = 0;                                 // Left.
    rightArrow.pnt.x = SCREEN_WIDTH - rightArrow.assets.w;  // Right.
    

    // Exit button.
    spritedbtn_t exitBtn = CreateButton(0,1, "Exit", GetStdBtnOptions(onclickExit,exampleHoverFunction, 20), GetStdBtnAssets(COL_WHITE, COL_BLACK, globalFont));
    const Size exitBtnSize = GetButtonSize(exitBtn);
    exitBtn.pnt.y = SCREEN_HEIGHT - exitBtnSize.h; // Place exit button at bottom left.

    // Top title text.
    char* titleText = "CarHorn - Wii Soundboard App by Dakotath.";
    char* soundText = "Sound: 69.pcm";

    // Main Loop.
    while(true)
    {
        // Scan for button presses.
        WPAD_ScanPads();
        WPAD_IR(WPAD_CHAN_0, &ir);

        s32 pressed = WPAD_ButtonsDown(WPAD_CHAN_0);

        // Check for home button press.
        if(pressed & WPAD_BUTTON_HOME)
        {
            break;
        }

        // Render UI text.
        GRRLIB_PrintfTTF(0,0, globalFont, titleText, 28, COL_LAVENDER);

        // Selected sound text.
        int selText_w = GRRLIB_WidthTTF(globalFont, soundText, 25);
        int selText_x = (SCREEN_WIDTH / 2) - (selText_w / 2);
        GRRLIB_PrintfTTF(selText_x, SCREEN_HEIGHT-30, globalFont, soundText, 25, COL_PINK);

        // Arrow.
        renderSpritedButton(leftArrow);
        renderSpritedButton(rightArrow);
        checkButtonStatus(&leftArrow, ir.x,ir.y, cSize, pressed);
        checkButtonStatus(&rightArrow, ir.x,ir.y, cSize, pressed);

        // Exit button.
        renderSpritedButton(exitBtn);
        checkButtonStatus(&exitBtn, ir.x,ir.y, cSize, pressed);

        // update.
        GRRLIB_DrawImg(ir.x,ir.y, cursor, 0, 1,1, 0xFFFFFFFF);
        GRRLIB_Render();
    }
}