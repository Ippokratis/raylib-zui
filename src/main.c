#define ZUI_DEBUG
#ifndef ZUI_IMPLEMENTATION
#define ZUI_IMPLEMENTATION
#include "zui.h"
#endif

int main(void)
{
    Color tpink = (Color){255, 0, 0, 16};
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(1000, 1000, "Raylib - ZUI");
    SetTargetFPS(60);

    ZuiInit();
    Texture tex = LoadTexture("src/resources/circle.png");

    // clang-format off
    ZuiBeginFrame((Rectangle){16, 16, 200, 130}, tpink);
        ZuiFramePad(16.0F);
        ZuiNewLabel("Login info");
        ZuiLabelAlignX(ZUI_ALIGN_X_CENTER);
        ZuiOffset(0,8);
        ZuiBeginRow();
           ZuiNewLabel("Username:");
           ZuiNewFrame( tpink, 80, 18);
        ZuiEndRow();
        ZuiBeginRow();
           ZuiNewLabel("Password:");
            ZuiLabelOffset(4,0);
            ZuiNewFrame(tpink, 80, 18);
        ZuiEndRow();
        ZuiNewLabel("Login");
        ZuiLabelAlignX(ZUI_ALIGN_X_RIGHT);
        ZuiLabelOffset(-12,8);
        ZuiLabelBackgroundColor(tpink);
    ZuiEndFrame();

    ZuiBeginFrame((Rectangle){236, 16, 300, 47}, tpink);
        ZuiFrameGap(16.0F);
        ZuiFramePad(16.0F);
        ZuiBeginRow();
        ZuiNewLabel("File");
        ZuiNewLabel("Edit");
        ZuiNewLabel("Selection");
        ZuiNewLabel("Window");
        ZuiNewLabel("Help");
        ZuiEndRow();
    ZuiEndFrame();

    ZuiBeginFrame((Rectangle){16, 160, 200, 146}, tpink);
        ZuiFramePad(16.0F);
        ZuiNewLabel("Save File");
        ZuiLabelAlignX(ZUI_ALIGN_X_CENTER);
        ZuiOffset(0,8);
        ZuiBeginRow();
            ZuiNewLabel("Filename:");
            ZuiNewFrame(tpink, 96, 18);
        ZuiEndRow();
        ZuiBeginRow();
            ZuiNewLabel("Location:");
            ZuiLabelOffset(4,0);
            ZuiNewFrame(tpink, 96, 18);
        ZuiEndRow();
        ZuiOffset(64,12);
        ZuiBeginRow();
            ZuiFrameGap(12.0F);
            ZuiNewLabel("Cancel");
            ZuiLabelBackgroundColor(tpink);
            ZuiNewLabel("Save");
            ZuiLabelBackgroundColor(tpink);
        ZuiEndRow();
    ZuiEndFrame();

    ZuiBeginFrame((Rectangle){236, 80, 144, 198}, tpink);
        ZuiFramePad(16.0F);
        ZuiFrameGap(4.0F);
        ZuiNewFrame(tpink, 112, 44);
        ZuiOffset(0,24);
        ZuiBeginRow();
            ZuiNewMonoLabel("[7]");
            ZuiLabelBackgroundColor(tpink);
            ZuiNewMonoLabel("[8]");
            ZuiLabelBackgroundColor(tpink);
            ZuiNewMonoLabel("[9]");
            ZuiLabelBackgroundColor(tpink);
            ZuiNewMonoLabel("[/]");
            ZuiLabelBackgroundColor(tpink);
        ZuiEndRow();
        ZuiBeginRow();
            ZuiNewMonoLabel("[4]");
            ZuiLabelBackgroundColor(tpink);
            ZuiNewMonoLabel("[5]");
            ZuiLabelBackgroundColor(tpink);
            ZuiNewMonoLabel("[6]");
            ZuiLabelBackgroundColor(tpink);
            ZuiNewMonoLabel("[*]");
            ZuiLabelBackgroundColor(tpink);
        ZuiEndRow();
         ZuiBeginRow();
            ZuiNewMonoLabel("[1]");
            ZuiLabelBackgroundColor(tpink);
            ZuiNewMonoLabel("[2]");
            ZuiLabelBackgroundColor(tpink);
            ZuiNewMonoLabel("[3]");
            ZuiLabelBackgroundColor(tpink);
            ZuiNewMonoLabel("[-]");
            ZuiLabelBackgroundColor(tpink);
        ZuiEndRow();
        ZuiBeginRow();
            ZuiNewMonoLabel("[0]");
            ZuiLabelBackgroundColor(tpink);
            ZuiNewMonoLabel("[.]");
            ZuiLabelBackgroundColor(tpink);
            ZuiNewMonoLabel("[=]");
            ZuiLabelBackgroundColor(tpink);
            ZuiNewMonoLabel("[+]");
            ZuiLabelBackgroundColor(tpink);
        ZuiEndRow();
    ZuiEndFrame();

    ZuiBeginFrame((Rectangle){405, 80, 190, 220}, tpink);
        ZuiBeginRow();
            ZuiNewTexture(tex);
            ZuiNewTexture(tex);
            ZuiNewTexture(tex);
            ZuiNewTexture(tex);
            ZuiNewTexture(tex);
        ZuiEndRow();
        ZuiBeginRow();
            ZuiOffset(8,0);
            ZuiNew9Slice(tex, 128, 128, 16, 16, 16, 16);
            ZuiNew3YSlice(tex, 128, 16, 16);
        ZuiEndRow();
        ZuiBeginRow();
            ZuiOffset(8,0);
            ZuiNew3XSlice(tex, 128, 16, 16);
            ZuiNewTexture(tex);
        ZuiEndRow();
    ZuiEndFrame();
    // clang-format on

    while (!WindowShouldClose())
    {
        ZuiUpdate();

        BeginDrawing();
        ClearBackground((Color){180, 180, 180, 255});

        ZuiRender();

        EndDrawing();
    }

    ZuiPrintFullItemTree(true);
    ZuiExit();

    CloseWindow();
    return 0;
}
