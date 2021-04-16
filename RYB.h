// Art Colors by Eric J. Jenislawski
// Version 0.2
// Copyright 2020, Eric J. Jenislawski.  Licensed under GNU General Public License version 3 or later.
//
// This header file provides stand-alone functions for RYB-to-RGB conversion and RGB-to-RYB
// As well as Subtractive RYB-space color mixing
// Art Colors was created with the Raylib library by Ramon Santamaria (raysan 5) https://github.com/raysan5/raylib
//
// If you wish to use this header file (RYB.h) and its source file (RYB.cpp) by themselves, and you are not using RayLib
// Simply comment out the include statements for the raylib.h and raymath.h files below.  The ifndef section following will
// supply the few functions needed from those libraries.

#ifndef RYB_H_INCLUDED
#define RYB_H_INCLUDED
//#include "raylib.h"
//#include "raymath.h"

#ifndef RAYLIB_H
//We don't have RayLib and RayMath, simply define the few elements we need

typedef struct Vector3 {
    float x;
    float y;
    float z;
} Vector3;

typedef struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} Color;

Color WHITE{255,255,255,255};
Color BLACK{0,0,0,255};

Vector3 Vector3Lerp(Vector3 v1, Vector3 v2, float amount)
{
    Vector3 result = { 0 };

    result.x = v1.x + amount*(v2.x - v1.x);
    result.y = v1.y + amount*(v2.y - v1.y);
    result.z = v1.z + amount*(v2.z - v1.z);

    return result;
}

Vector3 Vector3Add(Vector3 v1, Vector3 v2)
{
    Vector3 result = { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
    return result;
}

Vector3 Vector3Scale(Vector3 v, float scalar)
{
    Vector3 result = { v.x*scalar, v.y*scalar, v.z*scalar };
    return result;
}


#endif // RAYLIB_H

Color VecToCol(Vector3 in);
Vector3 ColToVec(Color c);
Color Saturate(Color in, float sat);
Color ColorBlindTransform(Color in, int CBtype);
Color Xform_RYB2RGB(int r, int y, int b);
Color Xform_RGB2RYB(int r, int g, int b);
Color ColorMix(Color a, Color b, float blend);
Color ColorMixLin(Color a, Color b, float blend);
Color ColorInv(Color in);
Color Brighten(Color in, float bright);
float ColorDistance(Color a, Color b);
Color ColorMixSub(Color a, Color b, float blend);


#endif // RYB_H_INCLUDED
