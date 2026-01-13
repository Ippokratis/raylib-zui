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

    // clang-format off
    ZuiBeginVertical((Vector2){16, 16}, 8, tpink);
        ZuiFramePad(16.0F);
        ZuiNewLabel("Login info");
        
        ZuiOffset(0,8);
        ZuiBeginRow(4);
           ZuiNewLabel("Username:");
           ZuiNewFrame( tpink, 80, 18);
        ZuiEndRow();
        ZuiBeginRow(4);
           ZuiNewLabel("Password:");
            ZuiNewFrame(tpink, 80, 18);
            ZuiOffsetLast(2,0);
        ZuiEndRow();
            ZuiNewLabel("Login");
            ZuiOffsetLast(55,0);
        ZuiLabelBackgroundColor(tpink);
    ZuiEndVertical();

    ZuiBeginHorizontal((Vector2){236, 16}, 8, tpink);
        ZuiFrameGap(8.0F);
        ZuiFramePad(16.0F);
        ZuiNewLabel("File");
        ZuiNewLabel("Edit");
        ZuiNewLabel("Selection");
        ZuiNewLabel("Window");
        ZuiNewLabel("Help");
    ZuiEndHorizontal();

    ZuiBeginVertical((Vector2){16, 160}, 8, tpink);
        ZuiFramePad(16.0F);
        ZuiNewLabel("Save File");
        ZuiBeginRow(4);
            ZuiNewLabel("Filename:");
            ZuiNewFrame(tpink, 96, 18);
        ZuiEndRow();
        ZuiBeginRow(4);
            ZuiNewLabel("Location:");
            ZuiNewFrame(tpink, 96, 18);
            ZuiOffsetLast(2,0);
        ZuiEndRow();
        ZuiBeginRow(4);
            ZuiFrameGap(12.0F);
            ZuiNewLabel("Cancel");
            ZuiLabelBackgroundColor(tpink);
            ZuiNewLabel("Save");
            ZuiLabelBackgroundColor(tpink);
        ZuiEndRow();
    ZuiEndVertical();

    ZuiBeginVertical((Vector2){236, 80}, 4, tpink);
        ZuiFramePad(16.0F);
        ZuiFrameGap(4.0F);
       
        ZuiSpace(0, 2);
        uint32_t l = ZuiNewFrame(tpink, 112, 44); // Display
        ZuiItemSetAlign(l, ZUI_ALIGN_CENTER);
        ZuiSpace(0,8);

        ZuiBeginRow(4);
            ZuiNewMonoLabel("[7]");
            ZuiLabelBackgroundColor(tpink);
            ZuiNewMonoLabel("[8]");
            ZuiLabelBackgroundColor(tpink);
            ZuiNewMonoLabel("[9]");
            ZuiLabelBackgroundColor(tpink);
            ZuiNewMonoLabel("[/]");
            ZuiLabelBackgroundColor(tpink);
        ZuiEndRow() ;
            
        ZuiBeginRow(4);
            ZuiNewMonoLabel("[4]");
            ZuiLabelBackgroundColor(tpink);
            ZuiNewMonoLabel("[5]");
            ZuiLabelBackgroundColor(tpink);
            ZuiNewMonoLabel("[6]");
            ZuiLabelBackgroundColor(tpink);
            ZuiNewMonoLabel("[*]");
            ZuiLabelBackgroundColor(tpink);
        ZuiEndRow();
                   
        ZuiBeginRow(4);
            ZuiNewMonoLabel("[1]");
            ZuiLabelBackgroundColor(tpink);
            ZuiNewMonoLabel("[2]");
            ZuiLabelBackgroundColor(tpink);
            ZuiNewMonoLabel("[3]");
            ZuiLabelBackgroundColor(tpink);
            ZuiNewMonoLabel("[-]");
            ZuiLabelBackgroundColor(tpink);
        ZuiEndRow();
    
        ZuiBeginRow(4);
            ZuiNewMonoLabel("[0]");
            ZuiLabelBackgroundColor(tpink);
            ZuiNewMonoLabel("[.]");
            ZuiLabelBackgroundColor(tpink);
            ZuiNewMonoLabel("[=]");
            ZuiLabelBackgroundColor(tpink);
            ZuiNewMonoLabel("[+]");
            ZuiLabelBackgroundColor(tpink);
        ZuiEndRow();
    ZuiEndVertical();    

    Texture tex = LoadTexture("src/resources/circle.png");
    ZuiBeginVertical((Vector2){405, 80}, 8, tpink);
            ZuiBeginRow(4);
            ZuiNewTexture(tex);
            ZuiNewTexture(tex);
            ZuiNewTexture(tex);
            ZuiNewTexture(tex);
            ZuiNewTexture(tex);
        ZuiEndRow();
        ZuiBeginRow(4);
            ZuiOffset(8,0);
            ZuiNew9Slice(tex, 128, 128, 16, 16, 16, 16);
            ZuiNew3YSlice(tex, 128, 16, 16);
        ZuiEndRow();
        ZuiBeginRow(4);
            ZuiOffset(8,0);
            ZuiNew3XSlice(tex, 128, 16, 16);
            ZuiNewTexture(tex);
        ZuiEndRow();
    ZuiEndVertical();
    // clang-format on

    while (!WindowShouldClose())
    {
        ZuiUpdate();

        BeginDrawing();
        ClearBackground((Color){180, 180, 180, 255});

        ZuiRender();

        EndDrawing();
    }

    ZuiExit();

    CloseWindow();
    return 0;
}
