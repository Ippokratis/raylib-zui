#define ZUI_DEBUG
#ifndef ZUI_CORE_IMPLEMENTATION
#define ZUI_CORE_IMPLEMENTATION
#include "zui_core.h"
#endif

int main(void)
{
    Color tpink = (Color){255, 0, 0, 16};
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(1000, 1000, "Raylib - ZUI");
    SetTargetFPS(60);

    ZuiInit();

    // clang-format off
    ZuiBeginFrame((Rectangle){16, 16, 200, 130}, tpink);
        ZuiFramePad(16.0);
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
        ZuiFrameGap(16.0);
        ZuiFramePad(16.0);
        ZuiBeginRow();
        ZuiNewLabel("File");
        ZuiNewLabel("Edit");
        ZuiNewLabel("Selection");
        ZuiNewLabel("Window");
        ZuiNewLabel("Help");
        ZuiEndRow();
    ZuiEndFrame();

    ZuiBeginFrame((Rectangle){16, 160, 200, 146}, tpink);
        ZuiFramePad(16.0);
        ZuiNewLabel("Save File");
        ZuiLabelAlignX(ZUI_ALIGN_X_CENTER);
        ZuiOffset(0,8);
        ZuiBeginRow();
            ZuiNewLabel("Filename:");
            ZuiNewFrame( tpink, 96, 18);
        ZuiEndRow();
        ZuiBeginRow();
            ZuiNewLabel("Location:");
            ZuiLabelOffset(4,0);
            ZuiNewFrame(tpink, 96, 18);
        ZuiEndRow();
        ZuiOffset(64,12);
        ZuiBeginRow();
            ZuiFrameGap(12.0);
            ZuiNewLabel("Cancel");
            ZuiLabelBackgroundColor(tpink);
            ZuiNewLabel("Save");
            ZuiLabelBackgroundColor(tpink);
        ZuiEndRow();
    ZuiEndFrame();

    ZuiBeginFrame((Rectangle){236, 80, 144, 198}, tpink);
        ZuiFramePad(16.0);
        ZuiFrameGap(4.0);
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

    // clang-format on

    while (!WindowShouldClose())
    {
        ZuiUpdate();

        BeginDrawing();
        ClearBackground((Color){180, 180, 180, 255});
        // DrawRectangle(214, 14, 23, 18, RED);
        ZuiRender();

        EndDrawing();
    }

    ZuiExit();

    CloseWindow();
    return 0;
}
