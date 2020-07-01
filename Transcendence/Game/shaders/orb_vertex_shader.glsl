#version 410 core

// The shader used in RayRasterizer comprises the following:
// -Width specification. The RayRasterizer draws a line in an arbitrary direction, then extends it width-wise.
// What we can do is to transform a pixel back to a given rotational frame using the rotation, then using the distance
// from the axis to get the width. Note, the C code uses a WidthCount and LengthCount to divide the ray into cells.
// We can use modulos and similar methods to determine which cell our pixel is in.
// -Length specification. There is an array of lengths in the C code; we can just pass the specified one directly into
// our VBO. It is specified as a number of length "cells" in the C code, but this is what is meant (?)
// -Start and end points. Since we're using OpenGL we can just generate the horizontal ray, and rotate the quad ourselves at
// the end. Simpler! No need to transform pixels back on a per-pixel basis; we handle this in vertex shader!

// Vertex shader should handle only the sizing and rotation aspects. All the magic happens in fragment shader.
// Note, it seems that numLengthCells is just the length of the ray in pixels...

layout(location = 0) in vec4 aPos;
layout(location = 1) in vec4 aSizeAndPosition;
layout(location = 2) in float aRotation;
layout(location = 3) in float fIntensity;
layout(location = 4) in float fOpacity;
layout(location = 5) in int iAnimation;
layout(location = 6) in int iStyle;
layout(location = 7) in int iDetail;
layout(location = 8) in int iDistortion;
layout(location = 9) in int iAnimationSeed;
layout(location = 10) in int iLifetime;
layout(location = 11) in int iCurrFrame;
layout(location = 12) in vec3 aPrimaryColor;
layout(location = 13) in vec3 aSecondaryColor;
layout(location = 14) in float fSecondaryOpacity;
layout(location = 15) in float aDepth;

uniform vec2 aCanvasAdjustedDimensions;

layout (location = 0) out vec2 quadPos;
layout (location = 1) flat out int animation;
layout (location = 2) flat out int style;
layout (location = 3) flat out int opacity;
layout (location = 4) flat out int distortion;
layout (location = 5) out float depth;
layout (location = 6) out float intensity;
layout (location = 7) out vec3 primaryColor;
layout (location = 8) out vec3 secondaryColor;
layout (location = 9) out float DELETEMEPLOX_waveCyclePos;
layout (location = 10) flat out int DELETEMEPLOX_colorTypes;
layout (location = 11) out float opacityAdj;
layout (location = 12) out vec2 quadSize;
layout (location = 13) flat out int detail;
layout (location = 14) out float orbRadius;
layout (location = 15) out float secondaryOpacity;
layout (location = 16) flat out int lifetime;
layout (location = 17) flat out int currFrame;
layout (location = 18) flat out int animationSeed;

uniform float current_tick;

mat4 rotationMatrix2D(float rotation)
{
    // Generates a rotation matrix that rotates a vector counterclockwise in the XY plane by the specified rotation
    float sterm = sin(rotation);
    float cterm = cos(rotation);
    vec4 v1 = vec4(cterm, -sterm, 0, 0);
    vec4 v2 = vec4(sterm, cterm, 0, 0);
    vec4 v3 = vec4(0, 0, 1, 0);
    vec4 v4 = vec4(0, 0, 0, 1);
    return mat4(v1, v2, v3, v4);
}

mat4 scalingMatrix2D(float scaleX, float scaleY)
{
    // Generates a rotation matrix that rotates a vector counterclockwise in the XY plane by the specified rotation
    vec4 v1 = vec4(scaleX, 0, 0, 0);
    vec4 v2 = vec4(0, scaleY, 0, 0);
    vec4 v3 = vec4(0, 0, 1, 0);
    vec4 v4 = vec4(0, 0, 0, 1);
    return mat4(v1, v2, v3, v4);
}

mat4 translationMatrix2D(float transX, float transY)
{
    // Generates a rotation matrix that rotates a vector counterclockwise in the XY plane by the specified rotation
    vec4 v1 = vec4(1, 0, 0, transX);
    vec4 v2 = vec4(0, 1, 0, transY);
    vec4 v3 = vec4(0, 0, 1, 0);
    vec4 v4 = vec4(0, 0, 0, 1);
    return mat4(v1, v2, v3, v4);
}

void main(void)
{
// Feeding uniforms into Kodelife...
// DO NOT TOUCH the first two when porting to Real OpenGL!
//    vec4 aSizeAndPosition = vec4(500.0, 500.0, 0.5, 0.5);
//    vec2 aCanvasAdjustedDimensions = vec2(500.0, 500.0);
//    float aRotation = 0.0;
//    float aRadius = 300.0; // in pixels - note that all orbs are circular
//    float fIntensity = 50.0;
//    float fOpacity = 1.0;
//    int iAnimation = 1;
//    int iStyle = 8;
//    int iDetail = 100;
//    int iDistortion = 40;
//    int iAnimationSeed = int(current_tick / 10.0);
//    int iLifetime = 50;
//    int iCurrFrame = int(mod(current_tick, iLifetime));
//    vec3 aPrimaryColor = vec3(0.4, 0.8, 0.8);
//    vec3 aSecondaryColor = vec3(0.0, 0.6, 1.0);
//    float aDepth = 0.0;

    vec2 aSize = vec2(aSizeAndPosition[0], aSizeAndPosition[1]);
    vec2 aPosOnCanvas = (vec2(aSizeAndPosition[2], aSizeAndPosition[3]) - vec2(0.5, 0.5)) * 2.0;
    
    vec4 final_pos = aPos * scalingMatrix2D(aSize[0] * 2.0, aSize[1] * 2.0) * rotationMatrix2D(aRotation) * scalingMatrix2D(1.0 / aCanvasAdjustedDimensions[0], 1.0 / aCanvasAdjustedDimensions[1]) * translationMatrix2D(aPosOnCanvas[0], -aPosOnCanvas[1]);

    quadPos = vec2(aPos[0], aPos[1]) * 2.0;
    // Note, the iVec values are casted to float but treated as int for some weird reason. This means that the
    // iVecs will contain bits equal to the float representation of their value, but are of type int. We should use intBitsToFloat
    // to fix this.
    animation = iAnimation;
    style = iStyle;
    distortion = iDistortion;
    detail = iDetail;
    depth = aDepth;
    intensity = fIntensity;
    //waveCyclePos = aFloatParams[1];
    opacityAdj = fOpacity;
    primaryColor = aPrimaryColor;
    secondaryColor = aSecondaryColor;
    quadSize = aSize;
    orbRadius = aSize[0] / 2.0;
    lifetime = iLifetime;
    currFrame = iCurrFrame;
    animationSeed = iAnimationSeed % 10000;
	secondaryOpacity = fSecondaryOpacity;
    gl_Position = final_pos;
    
}

