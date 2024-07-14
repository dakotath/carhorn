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

#include "button_over_pcm.h"
#include "button_click_pcm.h"

#include "bip_zip.h"

#include "app_funcs.h"
#include "misc/utils.h"
#include "zip.h"

bool canPlayFile = false;
bool canHazNewPack = false;
char* selFileName;
char* selPackName;
int selPackAudio = 0;
int packMaxAudio = 0;

// Bottom features.
void extractBIP();

// Sample hover function.
void exampleHoverFunction(int argc, char **argv) {
    printf("Hovered!\n");
    playSfx(button_over_pcm, button_over_pcm_size, ASND_GetFirstUnusedVoice());
}

// Sample Click Function.
void exampleClickFunction(int argc, char **argv) {
    printf("Clicked!\n");
    playSfx(button_click_pcm, button_click_pcm_size, ASND_GetFirstUnusedVoice());
}

// On Play.
void onPlayCLicked(int argc, char** argv) {
    if(selFileName != NULL && selPackName != NULL) {
        // Load file into RAM.
        char* fPath = malloc(1024);
        sprintf(fPath, "sd:/carhorn/packs/%s/%s", selPackName, selFileName);

        // Open file
        FILE* memeFile = fopen(fPath, "rb");
        if(memeFile == NULL) {
            perror("Fuck. Problem. Can't open meme file.\n");
        } else {
            // Obtain file size.
            int fSize = 0;
            fseek(memeFile, 0, SEEK_END);
            fSize = ftell(memeFile);
            fseek(memeFile, 0, SEEK_SET);

            // Memory Allocation.
            char* memAlloc = (char*)malloc(fSize);

            // Read file into buffer.
            fread(memAlloc, fSize, 1, memeFile);

            // Attempt to play.
            playSfx(memAlloc, fSize, ASND_GetFirstUnusedVoice());

            // Free up and close.
            free(fPath);
            free(memAlloc);
            fclose(memeFile);
        }
    }
}

// right arrow
void __carhorn_onRightArrow(int argc, char** argv) {
    if(selPackAudio+2 <= packMaxAudio) selPackAudio++;
}

// left arrow
void __carhorn_onLeftArrow(int argc, char** argv) {
    if(selPackAudio >= 1) selPackAudio--;
}

// Select new pack.
void __carhorn_selectNewPack(int argc, char** argv) {
    playSfx(button_click_pcm, button_click_pcm_size, ASND_GetFirstUnusedVoice());
    canHazNewPack = true;
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

    // Extract BIP.
    extractBIP();

    // Variables.
    ir_t ir;
    GRRLIB_ttfFont* globalFont;

    // Font.
    globalFont = GRRLIB_LoadTTF(font_ttf, font_ttf_size);

    // Cursor
    GRRLIB_texImg* cursor = GRRLIB_LoadTexture(cursor_png);
    Size cSize;
    cSize.w = cursor->w;
    cSize.h = cursor->h;

    // Play btn.
    spritedbtn_t playBtn = CreateButton(0,0, "Play Snd", GetStdBtnOptions(onPlayCLicked, exampleHoverFunction, 20), GetStdBtnAssets(COL_GREEN, COL_WHITE, globalFont));
    Size playBtnSize = GetButtonSize(playBtn);
    playBtn.pnt.x = (SCREEN_WIDTH / 2) - (playBtnSize.w / 2);
    playBtn.pnt.y = (SCREEN_HEIGHT / 2) - (playBtnSize.h / 2);

    // Select new pack btn.
    spritedbtn_t selNewPackBtn = CreateButton(0,0, "Other Pack", GetStdBtnOptions(__carhorn_selectNewPack, exampleHoverFunction, 20), GetStdBtnAssets(COL_GOLD, COL_WHITE, globalFont));
    Size selNewPackBtnSize = GetButtonSize(selNewPackBtn);
    selNewPackBtn.pnt.x = SCREEN_WIDTH - selNewPackBtnSize.w;
    selNewPackBtn.pnt.y = SCREEN_HEIGHT - selNewPackBtnSize.h;

    // Test arrow 1.
    spritedbtn_t leftArrow = CreateButton(200,200, "", GetStdBtnOptions(__carhorn_onLeftArrow,exampleHoverFunction, 20), GetStdBtnAssets(COL_WHITE, COL_BLACK, globalFont));
    leftArrow.assets.btnTexture = GRRLIB_LoadTexture(std_arrow_left_png);
    leftArrow.assets.w = leftArrow.assets.btnTexture->w;
    leftArrow.assets.h = leftArrow.assets.btnTexture->h;

    // Test arrow 2.
    spritedbtn_t rightArrow = CreateButton(200-65,200, "", GetStdBtnOptions(__carhorn_onRightArrow,exampleHoverFunction, 20), GetStdBtnAssets(COL_WHITE, COL_BLACK, globalFont));
    rightArrow.assets.btnTexture = GRRLIB_LoadTexture(std_arrow_right_png);
    rightArrow.assets.w = rightArrow.assets.btnTexture->w;
    rightArrow.assets.h = rightArrow.assets.btnTexture->h;

    // Position arrows.

    /* Center Y */
    leftArrow.pnt.y = (SCREEN_HEIGHT / 2) - (leftArrow.assets.h / 2);
    rightArrow.pnt.y = (SCREEN_HEIGHT / 2) - (rightArrow.assets.h / 2);

    /* Place them at their sides. */
    leftArrow.pnt.x = 0;                                    // Left.
    rightArrow.pnt.x = SCREEN_WIDTH - rightArrow.assets.w;  // Right.
    
    // Exit button.
    spritedbtn_t exitBtn = CreateButton(0,1, "Exit", GetStdBtnOptions(onclickExit,exampleHoverFunction, 20), GetStdBtnAssets(COL_WHITE, COL_BLACK, globalFont));
    const Size exitBtnSize = GetButtonSize(exitBtn);
    exitBtn.pnt.y = SCREEN_HEIGHT - exitBtnSize.h; // Place exit button at bottom left.

    // Top title text.
    char* titleText = "CarHorn - Wii Soundboard App by Dakotath.";
    char* packText = malloc(1024);

    // Select pack.
    int packEntryCount = 0;
    char *selectedPack = SelectPackMenu();
    char** packEntrys = GetAudioNames(selectedPack, &packEntryCount);
    selPackName = selectedPack;
    packMaxAudio = packEntryCount;

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

        // sound count text.
        char* soundCountText = malloc(1024);
        sprintf(soundCountText, "Sounds: %d", packMaxAudio);
        GRRLIB_PrintfTTF(0,64, globalFont, soundCountText, 28, COL_NAVY);
        free(soundCountText);

        // Pack Text.
        sprintf(packText, "Selected Pack: %s.", selectedPack);
        selFileName = packEntrys[selPackAudio];
        GRRLIB_PrintfTTF(0,32, globalFont, packText, 28, COL_NAVY);

        // Selected sound text.
        char* soundText = malloc(1024);
        sprintf(soundText, "Sound: %s", packEntrys[selPackAudio]);
        int selText_w = GRRLIB_WidthTTF(globalFont, soundText, 25);
        int selText_x = (SCREEN_WIDTH / 2) - (selText_w / 2);
        GRRLIB_PrintfTTF(selText_x, SCREEN_HEIGHT-30, globalFont, soundText, 25, COL_PINK);
        free(soundText);

        // Play Button.
        renderSpritedButton(playBtn);
        checkButtonStatus(&playBtn, ir.x,ir.y, cSize, pressed);

        // Arrow.
        renderSpritedButton(leftArrow);
        renderSpritedButton(rightArrow);
        checkButtonStatus(&leftArrow, ir.x,ir.y, cSize, pressed);
        checkButtonStatus(&rightArrow, ir.x,ir.y, cSize, pressed);

        // Exit button.
        renderSpritedButton(exitBtn);
        checkButtonStatus(&exitBtn, ir.x,ir.y, cSize, pressed);

        // New pack btn.
        renderSpritedButton(selNewPackBtn);
        checkButtonStatus(&selNewPackBtn, ir.x,ir.y, cSize, pressed);

        // update.
        GRRLIB_DrawImg(ir.x,ir.y, cursor, 0, 1,1, 0xFFFFFFFF);
        GRRLIB_Render();

        // Check if user wants a new pack.
        if(canHazNewPack) {
            packEntryCount = 0;
            selectedPack = SelectPackMenu();
            packEntrys = GetAudioNames(selectedPack, &packEntryCount);
            selPackName = selectedPack;
            packMaxAudio = packEntryCount;
        }

        // Update button vars.
        canPlayFile = false;
        canHazNewPack = false;
    }
}

int exBIPCB(const char* file, void* arg) {
    char* exText = malloc(1024);
    sprintf(exText, "Extracted: %s", file);

    // Quickly load in our font.
    GRRLIB_ttfFont* globalFont;
    globalFont = GRRLIB_LoadTTF(font_ttf, font_ttf_size);

    // Print and render
    GRRLIB_PrintfTTF(0,0, globalFont, "Please wait, Extracting BIP:", 25, COL_WHITE);
    GRRLIB_PrintfTTF(0,32, globalFont, exText, 18, COL_WHITE);
    GRRLIB_Render();

    // Free mem.
    GRRLIB_FreeTTF(globalFont);
    free(exText);
    return 0;
}

void extractBIP() {
    // Save BIP to file.
    FILE* bipFile = fopen("bip.pack", "wb");
    if(bipFile == NULL) {
        perror("Fuck!, We can't open the BIP.\n");
        return;
    }

    // Write BIP.
    fwrite(bip_zip, bip_zip_size, 1, bipFile);

    // Close bip.
    fclose(bipFile);

    // Extract the package.
    zip_extract("bip.pack", "sd:/carhorn/packs/", exBIPCB, NULL);
}