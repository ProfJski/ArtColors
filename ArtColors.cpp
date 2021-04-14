//ArtColors by Eric J. Jenislawski
//Version 0.2
//Subtractive Red-Yellow-Blue color mixing and palette selector
//Copyright 2020, Eric J. Jenislawski.  Licensed under GNU General Public License version 3 or later.
//The extremely useful RayLib and RayGUI libraries are by Ramon Santamaria (raysan5) https://github.com/raysan5/


#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <iostream>
#include <cstdio> //for snprintf
#include <cmath> //for fabs
#include <vector> // for C++ vector
#include <fstream> //for saving palettes
#include <string> // for to_string() for saving palettes
#include <algorithm>
#include "RYB.h"

using namespace std;


//Full color from 300 degrees to 60 degrees.  No color on [120,240].  Smoothstep transition in between.
float step2(float deg) {
    float out=0.0;
    float sc=0.0;
    while (deg<0.0) { deg+=360.0;}
    while (deg>360.0) { deg-=360.0;}

    if (deg<=60.0) {
        out=1.0;
    }
    else if ( (deg>60.0)&&(deg<=120.0) ) {
        sc=(deg-60.0)/60.0;
        out=1.0-2.0*sc/sqrt(1.0+3.0*sc*sc);
    }
    else if ( (deg>120.0) && (deg<=240.0) ) {
        out=0.0;
    }
    else if ( (deg>240.0) && (deg<=300.0) ) {
        sc=(deg-240.0)/60.0;
        out=2.0*sc/sqrt(1.0+3.0*sc*sc);
    }
    else if ( (deg>300.0) && (deg<=360.0) ) {
        out=1.0;
    }

return out;
}

Color map2(float deg) {
    Vector3 out;
    Color output;

    //Function-based color spread around the wheel
    out.x=255*step2(deg);
    out.y=255*step2(deg-120);
    out.z=255*step2(deg-240);

    output=Xform_RYB2RGB(out.x,out.y,out.z);

return output;
}



// MAIN

int main()
{
//Initialize Raylib
    InitWindow(1500, 900, "ArtColors: RYB mixer and palette selector");
    SetWindowPosition(400,50);

    Camera2D camera = { 0 };
    camera.target = (Vector2){ 0, 0 };
    camera.offset = (Vector2){ 0, 0 };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    SetTargetFPS(30);

    Color col=BLUE;
    float degrees=0.0;
    float brightness=0.0;
    float sat=0.0;
    float myred=0.0,myyellow=0.0,myblue=0.0;
    float myred2=0.0,myyellow2=0.0,myblue2=0.0;
    char textout[20];
    Color BackgroundColor = BLACK;

    vector<float> PaletteDegrees;

    Vector2 CircleCenter={300,250};
    Vector2 MousePosition;

    Color mix_a, mix_b,pixcol_a,pixcol_b;
    bool mix_a_hook=false,mix_b_hook=false;

    bool HelpButtonState=false;
    int HelpBoxState = 0;
    bool TextExportButtonState=false;
    bool TextExportButtonState2=false;
    bool TextExportButtonState3=false;
    bool TextExportButtonState4=false;
    bool TextExportButtonState5=false;
    bool ScreenShotButtonState=false;
    bool DrawInverseColors=false;

    int PaletteSelectorResult = 0;
    int CanvasToggleGroup = 0;

    bool ColorPickerActive=false;
    int ColorblindMode=0;
    bool RenderForColorDeficiency=false;

    while (!WindowShouldClose()){

//Update

//TODO:
//Get something better for a color picker for any hue on the screen.
//Keep screen shot as method of export.
    if(ColorPickerActive) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            MousePosition=GetMousePosition();
            cout<<"Mouse X:"<<MousePosition.x<<" Y="<<MousePosition.y<<endl;
            Image screenshot=GetScreenData();
            Color* sscols=new Color[1500*900];
            sscols=GetImageData(screenshot);
            pixcol_a=sscols[1500*((int)MousePosition.y)+(int)MousePosition.x];
            printf("R=%i, G=%i, B=%i \n",pixcol_a.r,pixcol_a.g,pixcol_a.b);
            UnloadImage(screenshot);
            delete[] sscols;
            mix_a_hook=true;
            ColorPickerActive=false;
        }
        else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            MousePosition=GetMousePosition();
            cout<<"Mouse X:"<<MousePosition.x<<" Y="<<MousePosition.y<<endl;
            Image screenshot=GetScreenData();
            Color* sscols=new Color[1500*900];
            sscols=GetImageData(screenshot);
            pixcol_b=sscols[1500*((int)MousePosition.y)+(int)MousePosition.x];
            printf("R=%i, G=%i, B=%i \n",pixcol_b.r,pixcol_b.g,pixcol_b.b);
            UnloadImage(screenshot);
            delete[] sscols;
            mix_b_hook=true;
            ColorPickerActive=false;
        }
    }


//Draw

        BeginDrawing();
        ClearBackground(BackgroundColor);
        BeginMode2D(camera);

        DrawText("Canvas Color",10,30,16,GRAY);
        CanvasToggleGroup = GuiToggleGroup((Rectangle){ 10, 50, 50, 25 }, "BLACK;WHITE", CanvasToggleGroup);
        switch (CanvasToggleGroup)
        {
        case 0:
            BackgroundColor=BLACK;
            break;
        case 1:
            BackgroundColor=WHITE;
            break;
        default:
            BackgroundColor=BLACK;
        }

        for (int i=0;i<360;i+=2) {
            col=map2((float)i);
            col=Saturate(col,sat);
            col=Brighten(col,brightness);
            DrawCircleSector(CircleCenter,200,i,i+5,6,col);
            //DrawCircleSector({1000,350},200,i,i+5,6,(Color){RYBCol.x,RYBCol.y,RYBCol.z,255});
        }

        degrees=GuiSlider({150,500,360,20},NULL,NULL,degrees,0.0,359.0);
        snprintf(textout,sizeof textout,"%f",degrees);
        DrawText(textout,520,500,20,GRAY);
        DrawText("Hue:",100,500,20,GRAY);

        brightness=GuiSlider({150,550,360,20},NULL,NULL,brightness,-1.0,1.0);
        if (fabs(brightness)<0.05) {brightness=0.0;}
        snprintf(textout,sizeof textout,"%f",brightness);
        DrawText(textout,520,550,20,GRAY);
        DrawText("Tint/Shade:",20,550,20,GRAY);

        sat=GuiSlider({150,600,360,20},NULL,NULL,sat,-1.0,1.0);
        if (fabs(sat)<0.05) {sat=0.0;}
        snprintf(textout,sizeof textout,"%f",sat);
        DrawText(textout,520,600,20,GRAY);
        DrawText("Saturate:",40,600,20,GRAY);

        ColorPickerActive=GuiToggle({150,640,80,20},"Pick Color",ColorPickerActive);
        DrawText("Left click on Color A, right click on B",250,640,16,GRAY);

//Color Palette Selection
        DrawText("Palette Type",550,30,16,GRAY);
        PaletteSelectorResult = GuiComboBox((Rectangle){ 550, 50, 150, 30 }, "Triadic;SplitComp;SqTetrad;RectTetrad;Analogous;Compl", PaletteSelectorResult);
        PaletteDegrees.clear();
        switch (PaletteSelectorResult)
        {
        case 0:
            PaletteDegrees.push_back(degrees);
            PaletteDegrees.push_back(degrees+120.0);
            PaletteDegrees.push_back(degrees+240.0);
            break;
        case 1:
            PaletteDegrees.push_back(degrees);
            PaletteDegrees.push_back(degrees+150.0);
            PaletteDegrees.push_back(degrees+210.0);
            break;
        case 2:
            PaletteDegrees.push_back(degrees);
            PaletteDegrees.push_back(degrees+90.0);
            PaletteDegrees.push_back(degrees+180.0);
            PaletteDegrees.push_back(degrees+270.0);
            break;
        case 3:
            PaletteDegrees.push_back(degrees);
            PaletteDegrees.push_back(degrees+120.0);
            PaletteDegrees.push_back(degrees+180.0);
            PaletteDegrees.push_back(degrees+300.0);
            break;
        case 4:
            PaletteDegrees.push_back(degrees);
            PaletteDegrees.push_back(degrees+15.0);
            PaletteDegrees.push_back(degrees+30.0);
            PaletteDegrees.push_back(degrees+45.0);
            break;
        case 5:
            PaletteDegrees.push_back(degrees);
            PaletteDegrees.push_back(degrees+180.0);
            break;
        }

//Draw Indicators Around Color Wheel
        for (unsigned int s=0;s<PaletteDegrees.size();s++) {
            float deg=PaletteDegrees[s];
            col=map2(deg);
            col=Saturate(col,sat);
            col=Brighten(col,brightness);
            DrawRectangle(CircleCenter.x+220*cos(DEG2RAD*(-deg+90)),CircleCenter.y+220*sin(DEG2RAD*(-deg+90)),10,10,col);
            DrawRectangle(550,200+30*s,40,20,col);
            snprintf(textout,sizeof textout,"%i %i %i",col.r,col.g,col.b);
            DrawText(textout,600,200+30*s,20,GRAY);

        }

/*
        //Alternative Color Picker
        float picker=GuiSlider({950,50,510,20},NULL,NULL,picker,0,255);
        for (int i=0;i<255;i++) {
            for (int j=0;j<255;j++) {
                col=Xform_RYB2RGB((int)picker,i,j);
                col=Saturate(col,sat);
                col=Brighten(col,brightness);
                DrawRectangle(1000+i,100+j,5,5,col);
            }
        }
*/


//Palettizer

        GuiGroupBox((Rectangle){ 800,150,615,215 }, "Swatches");
        for (unsigned int s=0;s<PaletteDegrees.size();s++) {
            col=map2(PaletteDegrees[s]);
            Color PCol;
            int sep=60/PaletteDegrees.size();

            for (float i=-0.8;i<1.0;i+=0.2) {
                for (float j=-0.8;j<=0.8;j+=0.2) {
                    PCol=Saturate(col,i);
                    PCol=Brighten(PCol,j);
                    if (RenderForColorDeficiency) {
                        PCol=ColorBlindTransform(PCol,ColorblindMode);
                    }
                    DrawRectangle(1050+i*300+sep*s,250+j*100,15,15,PCol);
                }
            }
        }


/*
    // RYB-RGB function and inverse test pattern
    //A round-trip conversion from RGB to RYB to RGB.  Helps show where trilinear interpretation values can be improved
    //Uncomment this section to draw a color bar beneath the save buttons and above the color swatches

    for (float i=0.0;i<360;i+=10.0) {
        col=map2(i);
        DrawRectangle(700+2*i,90,20,20,col);
        col=Xform_RGB2RYB(col.r,col.g,col.b);
        col=Xform_RYB2RGB(col.r,col.g,col.b);
        col=Saturate(col,0.5);
        DrawRectangle(700+2*i,110,20,20,col);
    }
*/

//Custom blender

        DrawText("Custom Color Blender",100,750,16,GRAY);

        myred=GuiSlider({100,780,255,20},"Red",NULL,myred,0.0,255.0);
        col=Xform_RYB2RGB(myred,0,0);
        DrawRectangle(360,780,20,20,col);

        myyellow=GuiSlider({100,810,255,20},"Yellow",NULL,myyellow,0.0,255.0);
        col=Xform_RYB2RGB(0,myyellow,0);
        DrawRectangle(360,810,20,20,col);

        myblue=GuiSlider({100,840,255,20},"Blue",NULL,myblue,0.0,255.0);
        col=Xform_RYB2RGB(0,0,myblue);
        DrawRectangle(360,840,20,20,col);

        col=Xform_RYB2RGB(myred,myyellow,myblue);
        DrawRectangle(400,780,100,100,col);

        snprintf(textout,sizeof textout,"%i %i %i",col.r,col.g,col.b);
        DrawText(textout,520,780,20,GRAY);
//        mix_a=col;
        if (mix_a_hook==true) {
            mix_a=pixcol_a;
//            mix_a_hook=false;
        }
        else {
            mix_a=col;
        }


        myred2=GuiSlider({700,780,255,20},"Red",NULL,myred2,0.0,255.0);
        col=Xform_RYB2RGB(myred2,0,0);
        DrawRectangle(960,780,20,20,col);

        myyellow2=GuiSlider({700,810,255,20},"Yellow",NULL,myyellow2,0.0,255.0);
        col=Xform_RYB2RGB(0,myyellow2,0);
        DrawRectangle(960,810,20,20,col);

        myblue2=GuiSlider({700,840,255,20},"Blue",NULL,myblue2,0.0,255.0);
        col=Xform_RYB2RGB(0,0,myblue2);
        DrawRectangle(960,840,20,20,col);

        col=Xform_RYB2RGB(myred2,myyellow2,myblue2);
        DrawRectangle(1000,780,100,100,col);

        snprintf(textout,sizeof textout,"%i %i %i",col.r,col.g,col.b);
        DrawText(textout,1120,780,20,GRAY);

//        mix_b=col;
        if (mix_b_hook==true) {
            mix_b=pixcol_b;
//            mix_b_hook=false;
        }
        else {
            mix_b=col;
        }


        DrawText("Additive",700,410,12,GRAY);
        DrawText("Quadratic:",700,430,12,GRAY);

        DrawText("Additive",700,480,12,GRAY);
        DrawText("Linear:",700,500,12,GRAY);

        DrawText("Subtractive:",700,560,12,GRAY);

        DrawInverseColors=GuiToggle((Rectangle){700,620,10,10}," ",DrawInverseColors);
        DrawText("Inverse:",700,640,12,GRAY);

        for (float i=0.0;i<1.1;i+=0.1) {
            col=ColorMix(mix_a,mix_b,i);
            DrawRectangle(800+(600*i),400,60,50,col);
            snprintf(textout,sizeof textout,"%i %i %i",col.r,col.g,col.b);
            DrawText(textout,800+(600*i),450,10,GRAY);

            col=ColorMixLin(mix_a,mix_b,i);
            DrawRectangle(800+(600*i),470,60,50,col);
            snprintf(textout,sizeof textout,"%i %i %i",col.r,col.g,col.b);
            DrawText(textout,800+(600*i),520,10,GRAY);

            col=ColorMixSub(mix_a,mix_b,i);
            DrawRectangle(800+(600*i),540,60,50,col);
            snprintf(textout,sizeof textout,"%i %i %i",col.r,col.g,col.b);
            DrawText(textout,800+(600*i),590,10,GRAY);

            if (DrawInverseColors) {
                col=ColorInv(col);
                DrawRectangle(800+(600*i),620,60,30,col);
                snprintf(textout,sizeof textout,"%i %i %i",col.r,col.g,col.b);
                DrawText(textout,800+(600*i),660,10,GRAY);
            }
        }

//Export and Help Buttons
        TextExportButtonState=GuiButton((Rectangle){800,50,130,35},"Save Swatches to .pal");
        if (TextExportButtonState) {
            ofstream PaletteFile;
            PaletteFile.open("ArtColor-Swatches.pal");
            if (PaletteFile.is_open()) {
                PaletteFile<<"JASC-PAL\r\n";
                PaletteFile<<"0100\r\n";
                PaletteFile<<to_string(PaletteDegrees.size()+72*PaletteDegrees.size())<<"\r\n";
                for (unsigned int s=0;s<PaletteDegrees.size();s++) {     //This part should match Palettizer loops.  TODO: make a subroutine for both.
                    col=map2(PaletteDegrees[s]);
                    Color PCol;
                    for (float i=-0.8;i<1.0;i+=0.2) {
                        for (float j=-0.8;j<=0.8;j+=0.2) {
                            PCol=Saturate(col,i);
                            PCol=Brighten(PCol,j);
                            PaletteFile<<to_string(PCol.r)<<" "<<to_string(PCol.g)<<" "<<to_string(PCol.b)<<"\r\n";
                        }
                    }
                }
                PaletteFile.close();
                cout<<"Palette file saved."<<endl;
            }
            else {
                cout<<"File creation failed."<<endl;
            }
        }

        DrawText("Save Color Bars:",950,30,16,GRAY);
        TextExportButtonState2=GuiButton((Rectangle){950,50,50,35},"Quad+");
        if (TextExportButtonState2) {
            ofstream PaletteFile;
            PaletteFile.open("ArtColor-AddQuadBars.pal");
            if (PaletteFile.is_open()) {
                PaletteFile<<"JASC-PAL\r\n";
                PaletteFile<<"0100\r\n";
                PaletteFile<<to_string(11)<<"\r\n";
                for (float i=0.0;i<1.1;i+=0.1) {
                    col=ColorMix(mix_a,mix_b,i);
                    PaletteFile<<to_string(col.r)<<" "<<to_string(col.g)<<" "<<to_string(col.b)<<"\r\n";
                }
                PaletteFile.close();
                cout<<"Additive Quadratic bars palette saved."<<endl;
            }
            else {
                cout<<"File creation failed."<<endl;
            }
        }

        TextExportButtonState3=GuiButton((Rectangle){1020,50,50,35},"Lin+");
        if (TextExportButtonState3) {
            ofstream PaletteFile;
            PaletteFile.open("ArtColor-AddLinBars.pal");
            if (PaletteFile.is_open()) {
                PaletteFile<<"JASC-PAL\r\n";
                PaletteFile<<"0100\r\n";
                PaletteFile<<to_string(11)<<"\r\n";
                for (float i=0.0;i<1.1;i+=0.1) {
                    col=ColorMixLin(mix_a,mix_b,i);
                    PaletteFile<<to_string(col.r)<<" "<<to_string(col.g)<<" "<<to_string(col.b)<<"\r\n";
                }
                PaletteFile.close();
                cout<<"Additive Linear bars palette saved."<<endl;
            }
            else {
                cout<<"File creation failed."<<endl;
            }
        }

        TextExportButtonState4=GuiButton((Rectangle){1090,50,50,35},"Subtract");
        if (TextExportButtonState4) {
            ofstream PaletteFile;
            PaletteFile.open("ArtColor-SubBars.pal");
            if (PaletteFile.is_open()) {
                PaletteFile<<"JASC-PAL\r\n";
                PaletteFile<<"0100\r\n";
                PaletteFile<<to_string(11)<<"\r\n";
                for (float i=0.0;i<1.1;i+=0.1) {
                    col=ColorMixSub(mix_a,mix_b,i);
                    PaletteFile<<to_string(col.r)<<" "<<to_string(col.g)<<" "<<to_string(col.b)<<"\r\n";
                }
                PaletteFile.close();
                cout<<"Subtractive mixing bars palette saved."<<endl;
            }
            else {
                cout<<"File creation failed."<<endl;
            }
        }

        if (DrawInverseColors) {
        TextExportButtonState5=GuiButton((Rectangle){1160,50,50,35},"Inv. Sub");
        if (TextExportButtonState5) {
            ofstream PaletteFile;
            PaletteFile.open("ArtColor-Inverse.pal");
            if (PaletteFile.is_open()) {
                PaletteFile<<"JASC-PAL\r\n";
                PaletteFile<<"0100\r\n";
                PaletteFile<<to_string(11)<<"\r\n";
                for (float i=0.0;i<1.1;i+=0.1) {
                    col=ColorMixSub(mix_a,mix_b,i);
                    col=ColorInv(col);
                    PaletteFile<<to_string(col.r)<<" "<<to_string(col.g)<<" "<<to_string(col.b)<<"\r\n";
                }
                PaletteFile.close();
                cout<<"Inverse bars palette saved."<<endl;
            }
            else {
                cout<<"File creation failed."<<endl;
            }
        }
        }


        DrawText("Colorblind mode:",1240,780,16,GRAY);
        ColorblindMode = GuiComboBox((Rectangle){ 1240, 820, 150, 35 }, "Normal;Protanopia;Deuteranopia;Tritanopia;Achromatopsia", ColorblindMode);
        if (ColorblindMode==0) RenderForColorDeficiency=false; else RenderForColorDeficiency=true;


        ScreenShotButtonState=GuiButton((Rectangle){1300,50,70,35},"Screen Shot");



        HelpButtonState=GuiToggle((Rectangle){1380,50,50,35},"Help",HelpButtonState);
        if (HelpButtonState) {
            string helpmessage="Choose a palette by moving the Hue / Brightness / Saturation sliders and selecting your palette type by cycling clicking the Palette Type button.\n";
            helpmessage+="This will update the color swatches on the left.\n";
            helpmessage+="You can blend by selecting them using the Pick Color button, then clicking anywhere in the program window\n";
            helpmessage+="Left click selects color A for blending.  Click the button again, and right click on what you want for color B.\n";
            helpmessage+="You can take the colors from the color wheel or use the RYB blending areas below to mix your own colors then select them.\n";
            helpmessage+="Use the save buttons above the swatches to export palette values for them or for the 10 values for various blends and inverse colors using the buttons above the swatches.\n";
            helpmessage+="All triplets of numbers are RGB values.\n";
            HelpBoxState=GuiMessageBox((Rectangle){200,500,1000,300},"Help Info",helpmessage.c_str(),"OK");
            if (HelpBoxState==0||HelpBoxState==1) HelpButtonState=false;
        }


        EndMode2D();
 //       DrawFPS(10,10);
        EndDrawing();

        if (ScreenShotButtonState) {
            TakeScreenshot("ArtColors-ScreenShot.png");
        }

    }

    return 0;
}
