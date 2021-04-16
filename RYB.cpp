// Art Colors by Eric J. Jenislawski
// Version 0.2
// Copyright 2020, Eric J. Jenislawski.  Licensed under GNU General Public License version 3 or later.

#include "RYB.h"
#include <algorithm>
#include <cmath>

Color VecToCol(Vector3 in){
    Color out=(Color){255*in.x,255*in.y,255*in.z,255};
return out;
}

Vector3 ColToVec(Color c) {
    Vector3 out=(Vector3){(float)c.r/255.0f,(float)c.g/255.0f,(float)c.b/255.0f};
return out;
}

Color Saturate(Color in, float sat) {
    if (fabs(sat)<0.004) {return in;}  //Immediately return when sat is zero or so small no difference will result (less than 1/255)
    if ((in.r==0)&&(in.g==0)&&(in.b==0)) {return in;}  //Prevents division by zero trying to saturate black

    Color out;
    Vector3 clerp={(float)in.r/255.0f,(float)in.g/255.0f,(float)in.b/255.0f};

    if (sat>0.0) {
        Vector3 maxsat;
        float mx=std::max(std::max(in.r,in.g),in.b);
        mx/=255.0;
        maxsat=Vector3Scale(clerp,1.0/mx);
        clerp=Vector3Lerp(clerp,maxsat,sat);
    }
    if (sat<0.0) {
        Vector3 grayc;
        float avg=(float) (in.r+in.g+in.b);
        avg/=(3.0*255.0);
        grayc={avg,avg,avg};
        clerp=Vector3Lerp(clerp,grayc,-1.0*sat);
    }
    out={255*clerp.x,255*clerp.y,255*clerp.z,255};
return out;
}

Color ColorBlindTransform(Color in, int CBtype) {
//Types 0=normal, 1=Protanopia, 2=Deuteranopia, 3=Tritanopia, 4=Achromatopsia
//Matrices taken from https://gist.github.com/Lokno/df7c3bfdc9ad32558bb7

    Color out=BLACK;
    switch(CBtype) {
    case 0:
        return in;
    case 1:
        out.r=in.r*0.567+in.g*0.433+in.b*0.000;
        out.g=in.r*0.558+in.g*0.442+in.b*0.000;
        out.b=in.r*0.000+in.g*0.242+in.b*0.758;
        return out;
    case 2:
        out.r=in.r*0.625+in.g*0.375+in.b*0.000;
        out.g=in.r*0.700+in.g*0.300+in.b*0.000;
        out.b=in.r*0.000+in.g*0.300+in.b*0.700;
        return out;
    case 3:
        out.r=in.r*0.950+in.g*0.050+in.b*0.000;
        out.g=in.r*0.000+in.g*0.433+in.b*0.567;
        out.b=in.r*0.000+in.g*0.475+in.b*0.525;
        return out;
    case 4:
        out.r=in.r*0.299+in.g*0.587+in.b*0.114;
        out.g=in.r*0.299+in.g*0.587+in.b*0.114;
        out.b=in.r*0.299+in.g*0.587+in.b*0.114;
        return out;
    default:
        return in;
    }

return in;
}




Color Xform_RYB2RGB(int r, int y, int b) {
    float rin=(float)r/255.0;
    float yin=(float)y/255.0;
    float bin=(float)b/255.0;


    //The values defined here are where the magic happens.  You can experiment with changing the values and see if you find a better set.  If so, notify me on GitHub @ProfJski !
    //I have included a few alternative sets below

    //RYB corners in RGB values
    //Values arranged to approximate an artist's color wheel
    Vector3 CG000={0.0,0.0,0.0}; //Black
    Vector3 CG100={1.0,0.0,0.0}; //Red
    Vector3 CG010={0.9,0.9,0.0}; //Yellow = RGB Red+Green.  Still a bit high, but helps Yellow compete against Green.  Lower gives murky yellows.
    Vector3 CG001={0.0,0.36,1.0}; //Blue: Green boost of 0.36 helps eliminate flatness of spectrum around pure Blue
    Vector3 CG011={0.0,0.9,0.2}; //Green: A less intense green than {0,1,0}, which tends to dominate
    Vector3 CG110={1.0,0.6,0.0}; //Orange = RGB full Red, 60% Green
    Vector3 CG101={0.6,0.0,1.0}; //Purple = 60% Red, full Blue
    Vector3 CG111={1.0,1.0,1.0}; //White

/*
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
*/
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

    //Finding the appropriate values for the inverse transform was no easy task.  After some experimentation, I wrote a separate program that used
    //the calculus of variations to help tweak my guesses towards values that provided a closer round-trip conversion from RGB to RYB to RGB again.

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

    Color CRYB=Saturate(VecToCol(C),0.5);

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

Color Brighten(Color in, float bright) {
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

    f.r=std::max(0,255-c.r-d.r);
    f.g=std::max(0,255-c.g-d.g);
    f.b=std::max(0,255-c.b-d.b);

    float cd=ColorDistance(a,b);
    cd=4.0*blend*(1.0-blend)*cd;
    out=ColorMixLin(ColorMixLin(a,b,blend),f,cd);

    out.a=255;
return out;
}
