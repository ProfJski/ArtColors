//ArtColors by Eric J. Jenislawski
//Version 0.1
//Subtractive Red-Yellow-Blue color mixing and palette selector
//Copyright 2020, Eric J. Jenislawski.  Licensed under GNU General Public License version 3 or later.
//The extremely useful RayLib and RayGUI libraries are by Ramon Santamaria (raysan5).


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

using namespace std;

Color V2Col(Vector3 in){
    Color out=(Color){255*in.x,255*in.y,255*in.z,255};
return out;
}

Vector3 col2vec(Color c) {
    Vector3 out=(Vector3){(float)c.r/255.0f,(float)c.g/255.0f,(float)c.b/255.0f};
    return out;
}

Color saturate(Color in, float sat) {
    if (sat==0.0) {return in;}
    if ((in.r==0)&&(in.g==0)&&(in.b==0)) {return in;}  //Prevents division by zero trying to saturate black

    Color out;
    Vector3 clerp={(float)in.r/255.0f,(float)in.g/255.0f,(float)in.b/255.0f};

    if (sat>0.0) {
        Vector3 maxsat;
        float mx=max(max(in.r,in.g),in.b);
        mx/=255.0;
        maxsat.x=clerp.x/mx;
        maxsat.y=clerp.y/mx;
        maxsat.z=clerp.z/mx;
        clerp.x=(1.0-sat)*clerp.x+sat*maxsat.x;
        clerp.y=(1.0-sat)*clerp.y+sat*maxsat.y;
        clerp.z=(1.0-sat)*clerp.z+sat*maxsat.z;
    }

    if (sat<0.0) {
        Vector3 grayc;
        float avg=(float) (in.r+in.g+in.b);
        avg/=(3.0*255.0);
        grayc={avg,avg,avg};
        clerp.x=(1.0+sat)*clerp.x-sat*grayc.x;
        clerp.y=(1.0+sat)*clerp.y-sat*grayc.y;
        clerp.z=(1.0+sat)*clerp.z-sat*grayc.z;
    }

    out={255*clerp.x,255*clerp.y,255*clerp.z,255};

return out;
}

Color Xform_RYB2RGB(int r, int y, int b) {
    float rin=(float)r/255.0;
    float yin=(float)y/255.0;
    float bin=(float)b/255.0;


    //RYB corners in RGB values
    //Values arranged to approximate an artist's color wheel
    Vector3 CG000={0.0,0.0,0.0}; //Black
    Vector3 CG100={1.0,0.0,0.0}; //Red
    Vector3 CG010={0.9,0.9,0.0}; //Yellow = RGB Red+Green.  Still a bit high, but helps Yellow compete against Green.  Lower gives murky yellows.
    Vector3 CG001={0.0,0.36,1.0}; //Blue: Green boost of 0.36 helps eliminate flatness of spectrum around pure Blue
    Vector3 CG011={0.0,0.75,0.3}; //Green: A less intense green than {0,1,0}, which tends to dominate
    Vector3 CG110={1.0,0.6,0.0}; //Orange = RGB full Red, 60% Green
    Vector3 CG101={0.6,0.0,1.0}; //Purple = 60% Red, full Blue
    Vector3 CG111={1.0,1.0,1.0}; //White

/*
    //RYB corners in RGB values
    //Unbalanced corners: Less even hue distribution
    Vector3 CG000={0.0,0.0,0.0}; //Black
    Vector3 CG100={1.0,0.0,0.0}; //Red
    Vector3 CG010={1.0,1.0,0.0}; //Yellow
    Vector3 CG001={0.0,0.0,1.0}; //Blue:
    Vector3 CG011={0.0,1.0,0.0}; //Green:
    Vector3 CG110={1.0,0.5,0.0}; //Orange
    Vector3 CG101={0.5,0.0,1.0}; //Purple
    Vector3 CG111={1.0,1.0,1.0}; //White
*/

    //Trilinear interpolation from RYB to RGB
    Vector3 C00,C01,C10,C11;
    C00=Vector3Add(Vector3Scale(CG000,1.0-rin),Vector3Scale(CG100,rin));
    C01=Vector3Add(Vector3Scale(CG001,1.0-rin),Vector3Scale(CG101,rin));
    C10=Vector3Add(Vector3Scale(CG010,1.0-rin),Vector3Scale(CG110,rin));
    C11=Vector3Add(Vector3Scale(CG011,1.0-rin),Vector3Scale(CG111,rin));

    Vector3 C0,C1;
    C0=Vector3Add(Vector3Scale(C00,1.0-yin),Vector3Scale(C10,yin));
    C1=Vector3Add(Vector3Scale(C01,1.0-yin),Vector3Scale(C11,yin));

    Vector3 C;
    C=Vector3Add(Vector3Scale(C0,1.0-bin),Vector3Scale(C1,bin));

    Color CRGB={255*C.x,255*C.y,255*C.z,255};

    return CRGB;
}

Color Xform_RGB2RYB(int r, int g, int b) {
    float rin=(float)r/255.0;
    float gin=(float)g/255.0;
    float bin=(float)b/255.0;

    //RGB corners in RYB values
    Vector3 CG000={0.0,0.0,0.0}; //Black
    Vector3 CG100={0.891,0.0,0.0}; //Red
    Vector3 CG010={0.0,0.714,0.374}; //Green = RYB Yellow + Blue
    Vector3 CG001={0.07,0.08,0.893}; //Blue:
    Vector3 CG011={0.0,0.116,0.313}; //Cyan = RYB Green + Blue.  Very dark to make the rest of the function work correctly
    Vector3 CG110={0.0,0.915,0.0}; //Yellow
    Vector3 CG101={0.554,0.0,0.1}; //Magenta =RYB Red + Blue.  Likewise dark.
    Vector3 CG111={1.0,1.0,1.0}; //White

    //Trilinear interpolation from RGB to RYB
    Vector3 C00,C01,C10,C11;
    C00=Vector3Add(Vector3Scale(CG000,1.0-rin),Vector3Scale(CG100,rin));
    C01=Vector3Add(Vector3Scale(CG001,1.0-rin),Vector3Scale(CG101,rin));
    C10=Vector3Add(Vector3Scale(CG010,1.0-rin),Vector3Scale(CG110,rin));
    C11=Vector3Add(Vector3Scale(CG011,1.0-rin),Vector3Scale(CG111,rin));

    Vector3 C0,C1;
    C0=Vector3Add(Vector3Scale(C00,1.0-gin),Vector3Scale(C10,gin));
    C1=Vector3Add(Vector3Scale(C01,1.0-gin),Vector3Scale(C11,gin));

    Vector3 C;
    C=Vector3Add(Vector3Scale(C0,1.0-bin),Vector3Scale(C1,bin));

    Color CRYB=saturate(V2Col(C),0.5);

    return CRYB;
}


Color ColorMix(Color a, Color b, float blend) {
    Color out;
    out.r=sqrt((1.0-blend)*(a.r*a.r)+blend*(b.r*b.r));
    out.g=sqrt((1.0-blend)*(a.g*a.g)+blend*(b.g*b.g));
    out.b=sqrt((1.0-blend)*(a.b*a.b)+blend*(b.b*b.b));
    out.a=(1.0-blend)*a.a+blend*b.a;

return out;
}

Color ColorMixLin(Color a, Color b, float blend) {
    Color out;
    out.r=(1.0-blend)*a.r+blend*b.r;
    out.g=(1.0-blend)*a.g+blend*b.g;
    out.b=(1.0-blend)*a.b+blend*b.b;
    out.a=(1.0-blend)*a.a+blend*b.a;

return out;
}

Color ColorInv(Color in) {
    Color out={255-in.r,255-in.g,255-in.b,255};
return out;
}

Color brightener(Color in, float bright) {
    if (bright==0.0) {return in;}

    Color out;
    if (bright>0.0) {
        out=ColorMix(in,WHITE,bright);
    }

    if (bright<0.0) {
        out=ColorMix(in,BLACK,-1.0*bright);
    }
return out;
}

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

    //Function based color spread around the wheel
    out.x=255*step2(deg);
    out.y=255*step2(deg-120);
    out.z=255*step2(deg-240);

    output=Xform_RYB2RGB(out.x,out.y,out.z);

return output;
}

Color ColorProduct(Color a, Color b) {
    Color out;
    out.r=(a.r*b.r)/255;
    out.g=(a.g*b.g)/255;
    out.b=(a.b*b.b)/255;
    out.a=255;

return out;
}

Color ColorProductSq(Color a, Color b) {
    Color out;
    out.r=255-sqrt( ( (255-a.r)*(255-a.r)+(255-b.r)*(255-b.r) )/2.0 );
    out.g=255-sqrt( ( (255-a.g)*(255-a.g)+(255-b.g)*(255-b.g) )/2.0 );
    out.b=255-sqrt( ( (255-a.b)*(255-a.b)+(255-b.b)*(255-b.b) )/2.0 );
    out.a=255;

return out;
}

Color ColorScale(Color in, float scale) {
    Color out;
    out.r=(float)in.r*scale;
    out.g=(float)in.g*scale;
    out.b=(float)in.b*scale;
    out.a=255;
return out;
}

float ColorDistance(Color a, Color b) {
    float out=(float)((a.r-b.r)*(a.r-b.r)+(a.g-b.g)*(a.g-b.g)+(a.b-b.b)*(a.b-b.b));
    out=sqrt(out)/(sqrt(3.0)*255); //scale to 0-1
return out;
}

Color ColorMixSub(Color a, Color b, float blend) {
    Color out;
    Color c,d,f;

    c=ColorInv(a);
    d=ColorInv(b);

    f.r=max(0,255-c.r-d.r);
    f.g=max(0,255-c.g-d.g);
    f.b=max(0,255-c.b-d.b);

    float cd=ColorDistance(a,b);
    cd=4.0*blend*(1.0-blend)*cd;
    out=ColorMixLin(ColorMixLin(a,b,blend),f,cd);

    out.a=255;
return out;
}

// MAIN

int main()
{
//Initialize Raylib
    InitWindow(1500, 900, "ArtColors: RYB mixer and palette selector");
    SetWindowPosition(600,50);

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

    int comboBoxActive = 0;
    int CanvasToggleGroup = 0;

    while (!WindowShouldClose()){

//Update

//TODO:
//Get something better for a color picker for any hue on the screen.
//Keep screen shot as method of export.
/*
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
    }
*/

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

        for (int i=0;i<360;i+=5) {
            col=map2((float)i);
            col=saturate(col,sat);
            col=brightener(col,brightness);
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

//Color Palette Selection
        DrawText("Palette Type",550,30,16,GRAY);
        comboBoxActive = GuiComboBox((Rectangle){ 550, 50, 150, 30 }, "Triadic;SplitComp;SqTetrad;RectTetrad;Analogous;Compl", comboBoxActive);
        PaletteDegrees.clear();
        switch (comboBoxActive)
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
            col=saturate(col,sat);
            col=brightener(col,brightness);
            DrawRectangle(CircleCenter.x+220*cos(DEG2RAD*(-deg+90)),CircleCenter.y+220*sin(DEG2RAD*(-deg+90)),10,10,col);
            DrawRectangle(550,200+30*s,40,20,col);
            snprintf(textout,sizeof textout,"%i %i %i",col.r,col.g,col.b);
            DrawText(textout,600,200+30*s,20,GRAY);

        }

/*
        //Alternative Color Picker
        picker=GuiSlider({950,50,510,20},NULL,NULL,picker,0,255);
        for (int i=0;i<255;i++) {
            for (int j=0;j<255;j++) {
                col=Xform_RYB2RGB((int)picker,i,j);
                col=saturate(col,sat);
                col=brightener(col,brightness);
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
                    PCol=saturate(col,i);
                    PCol=brightener(PCol,j);
                    DrawRectangle(1050+i*300+sep*s,250+j*100,15,15,PCol);
                }
            }
        }

/*
    // RYB-RGB function and inverse test pattern
    for (float i=0.0;i<360;i+=10.0) {
        col=map2(i);
        DrawRectangle(700+2*i,050,20,20,col);
        col=Xform_RGB2RYB(col.r,col.g,col.b);
        col=Xform_RYB2RGB(col.r,col.g,col.b);
        col=saturate(col,0.5);
        DrawRectangle(700+2*i,070,20,20,col);
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
            //mix_a_hook=false;
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
            //mix_b_hook=false;
        }
        else {
            mix_b=col;
        }

/*
        col=ColorMix(mix_a,mix_b,blendfactor);
        DrawRectangle(1000,500,50,50,col);
        snprintf(textout,sizeof textout,"%f",blendfactor);
        blendfactor=GuiSlider({950,450,255,20},"Blend",textout,blendfactor,0.0,1.0);
        snprintf(textout,sizeof textout,"%i %i %i",col.r,col.g,col.b);
        DrawText(textout,1070,500,20,GRAY);

        col=ColorMixLin(mix_a,mix_b,blendfactor);
        DrawRectangle(1000,600,50,50,col);
        snprintf(textout,sizeof textout,"%i %i %i",col.r,col.g,col.b);
        DrawText(textout,1070,600,20,GRAY);
*/

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
                            PCol=saturate(col,i);
                            PCol=brightener(PCol,j);
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
                PaletteFile<<to_string(10)<<"\r\n";
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
                PaletteFile<<to_string(10)<<"\r\n";
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
                PaletteFile<<to_string(10)<<"\r\n";
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
                PaletteFile<<to_string(10)<<"\r\n";
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

        ScreenShotButtonState=GuiButton((Rectangle){1300,50,70,35},"Screen Shot");
        if (ScreenShotButtonState) {
            TakeScreenshot("ArtColors-ScreenShot.png");
        }

/*
        HelpButtonState=GuiToggle((Rectangle){1400,50,30,35},"Help",HelpButtonState);
        if (HelpButtonState) {
            HelpBoxState=GuiMessageBox((Rectangle){500,500,200,200},"Help Info","Help! Help! Haaaalp!","OK");
            if (HelpBoxState==0||HelpBoxState==1) HelpButtonState=false;
        }
*/

        EndMode2D();
 //       DrawFPS(10,10);
        EndDrawing();
    }

    return 0;
}
