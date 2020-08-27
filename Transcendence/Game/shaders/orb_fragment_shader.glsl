#version 410 core



// The shader used in RayRasterizer comprises the following:
// -Width specification. The RayRasterizer draws a line in an arbitrary direction, then extends it width-wise.
// What we can do is to transform a pixel back to a given rotational frame using the rotation, then using the distance
// from the axis to get the width. Note, the C code uses a WidthCount and LengthCount to divide the ray into cells.
// We can use modulos and similar methods to determine which cell our pixel is in.
// -Length specification. There is an array of lengths in the C code; we can just pass the specified one directly into
// our VBO. It is specified as a number of length "cells" in the C code, but this is what is meant (?)
// Following variables will be required:
// Direct from GeorgeCode: iColorTypes, iOpacityTypes, iWidthAdjType, iReshape, iTexture


// GeorgeCode defines incrementor as such:
//            Metric rStep = (Metric)iStep / (Metric)m_iSteps;
//            return m_rStart + (pow(rStep, m_rPower) * m_rRange);
// m_iSteps, m_rPower, m_rStart and m_rRange specified in incrementor. iStep is the input argument to GetAt(int iStep).

layout (location = 0) in vec2 quadPos;
layout (location = 1) flat in int animation; //
layout (location = 2) flat in int style; //
layout (location = 3) flat in int opacity;
layout (location = 4) flat in int distortion;
layout (location = 5) in float depth; //
layout (location = 6) in float intensity; //
layout (location = 7) in vec3 primaryColor; //
layout (location = 8) in vec3 secondaryColor; //
layout (location = 9) in float waveCyclePos; // not used here
layout (location = 10) flat in int colorTypes; // not used here
layout (location = 11) in float opacityAdj;
layout (location = 12) in vec2 quadSize;
layout (location = 13) flat in int detail;
layout (location = 14) in float orbRadius;
layout (location = 15) in float secondaryOpacity;
layout (location = 16) flat in int lifetime;
layout (location = 17) flat in int currFrame;
layout (location = 18) flat in int animationSeed;

uniform float current_tick;

out vec4 fragColor;

float pixelsDistanceFromCenter = length(quadPos) * (quadSize[0] / 2);

float PI = 3.14159;
float SHELL_EDGE_WIDTH_RATIO = 0.05;

float BLOB_WAVE_SIZE = 0.3;
float WAVY_WAVELENGTH_FACTOR = 1.0;
float JAGGED_AMPLITUDE = 0.45;
float JAGGED_WAVELENGTH_FACTOR = 0.33;
float WHIPTAIL_AMPLITUDE = 0.45;
float WHIPTAIL_WAVELENGTH_FACTOR = 1.0;
float WHIPTAIL_DECAY = 0.13;

// Copy of EAnimationTypes enum from SFXOrb.cpp
int animateNone =           0;
int animateDissipate =      1;
int animateExplode =        2;
int animateFade =           3;
int animateFlicker =        4;
int animateDim =            5;

// Copy of EOrbStyles enum from SFXOrb.cpp
int styleSmooth =           1;
int styleFlare =            2;
int styleCloud =            3;
int styleFireblast =        4;
int styleSmoke =            5;
int styleDiffraction =      6;
int styleFirecloud =        7;
int styleBlackHole =        8;
int styleLightning =        9;
int styleShell =            10;
int styleCloudshell =       11;
int styleFireCloudshell =   12;

//  Classic Perlin 3D Noise 
//  by Stefan Gustavson
//
//  Found at https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83
vec4 permute(vec4 x){return mod(((x*34.0)+1.0)*x, 289.0);}
vec4 taylorInvSqrt(vec4 r){return 1.79284291400159 - 0.85373472095314 * r;}
vec3 fade(vec3 t) {return t*t*t*(t*(t*6.0-15.0)+10.0);}

float cnoise(vec3 P){
  vec3 Pi0 = floor(P); // Integer part for indexing
  vec3 Pi1 = Pi0 + vec3(1.0); // Integer part + 1
  Pi0 = mod(Pi0, 289.0);
  Pi1 = mod(Pi1, 289.0);
  vec3 Pf0 = fract(P); // Fractional part for interpolation
  vec3 Pf1 = Pf0 - vec3(1.0); // Fractional part - 1.0
  vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
  vec4 iy = vec4(Pi0.yy, Pi1.yy);
  vec4 iz0 = Pi0.zzzz;
  vec4 iz1 = Pi1.zzzz;

  vec4 ixy = permute(permute(ix) + iy);
  vec4 ixy0 = permute(ixy + iz0);
  vec4 ixy1 = permute(ixy + iz1);

  vec4 gx0 = ixy0 / 7.0;
  vec4 gy0 = fract(floor(gx0) / 7.0) - 0.5;
  gx0 = fract(gx0);
  vec4 gz0 = vec4(0.5) - abs(gx0) - abs(gy0);
  vec4 sz0 = step(gz0, vec4(0.0));
  gx0 -= sz0 * (step(0.0, gx0) - 0.5);
  gy0 -= sz0 * (step(0.0, gy0) - 0.5);

  vec4 gx1 = ixy1 / 7.0;
  vec4 gy1 = fract(floor(gx1) / 7.0) - 0.5;
  gx1 = fract(gx1);
  vec4 gz1 = vec4(0.5) - abs(gx1) - abs(gy1);
  vec4 sz1 = step(gz1, vec4(0.0));
  gx1 -= sz1 * (step(0.0, gx1) - 0.5);
  gy1 -= sz1 * (step(0.0, gy1) - 0.5);

  vec3 g000 = vec3(gx0.x,gy0.x,gz0.x);
  vec3 g100 = vec3(gx0.y,gy0.y,gz0.y);
  vec3 g010 = vec3(gx0.z,gy0.z,gz0.z);
  vec3 g110 = vec3(gx0.w,gy0.w,gz0.w);
  vec3 g001 = vec3(gx1.x,gy1.x,gz1.x);
  vec3 g101 = vec3(gx1.y,gy1.y,gz1.y);
  vec3 g011 = vec3(gx1.z,gy1.z,gz1.z);
  vec3 g111 = vec3(gx1.w,gy1.w,gz1.w);

  vec4 norm0 = taylorInvSqrt(vec4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
  g000 *= norm0.x;
  g010 *= norm0.y;
  g100 *= norm0.z;
  g110 *= norm0.w;
  vec4 norm1 = taylorInvSqrt(vec4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
  g001 *= norm1.x;
  g011 *= norm1.y;
  g101 *= norm1.z;
  g111 *= norm1.w;

  float n000 = dot(g000, Pf0);
  float n100 = dot(g100, vec3(Pf1.x, Pf0.yz));
  float n010 = dot(g010, vec3(Pf0.x, Pf1.y, Pf0.z));
  float n110 = dot(g110, vec3(Pf1.xy, Pf0.z));
  float n001 = dot(g001, vec3(Pf0.xy, Pf1.z));
  float n101 = dot(g101, vec3(Pf1.x, Pf0.y, Pf1.z));
  float n011 = dot(g011, vec3(Pf0.x, Pf1.yz));
  float n111 = dot(g111, Pf1);

  vec3 fade_xyz = fade(Pf0);
  vec4 n_z = mix(vec4(n000, n100, n010, n110), vec4(n001, n101, n011, n111), fade_xyz.z);
  vec2 n_yz = mix(n_z.xy, n_z.zw, fade_xyz.y);
  float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x); 
  return 2.2 * n_xyz;
}


float fbm(vec2 p, float time)
{
    // Fractal Brownian Motion using Perlin noise
     float v = 0.0;
     float amp = 0.3;
     float freq = 20;
     int octaves = 3;
     for (int i = 0; i < octaves; i++)
         v += cnoise(vec3(((p + vec2(1000.0, 1000.0)) * (i * freq)), time / 100)) * (amp / (i + 1));

     return v;
}

vec3 getExplosionColor(float fRadius, float fMaxRadius, float fIntensity, vec3 vPrimaryColor, vec3 vSecondaryColor) {
    const float SIGMA_MAX =                    0.01;
    const float SIGMA_DECAY =                  1.07;
    const float MAX_INTENSITY =                100.0;
    
    
    const float CORE_HEAT_LEVEL =              0.5;
    const float FRINGE_HEAT_LEVEL =            0.35;
    const float FRINGE_HEAT_RANGE =            (CORE_HEAT_LEVEL - FRINGE_HEAT_LEVEL);
    const float FLAME_HEAT_LEVEL =             0.25;
    const float FLAME_HEAT_RANGE =             (FRINGE_HEAT_LEVEL - FLAME_HEAT_LEVEL);
    const float MIN_HEAT_LEVEL =               0.1;
    const float MIN_HEAT_RANGE =               (FLAME_HEAT_LEVEL - MIN_HEAT_LEVEL);
    
    float fHeatCalcIntensity = min(fIntensity, 100);
    float fHeatCountRadius = fRadius / fMaxRadius;
    
    float fSigma2 = SIGMA_MAX * pow(SIGMA_DECAY, MAX_INTENSITY - fHeatCalcIntensity);
    float fSigma = sqrt(fSigma2);
    float fMaxX = sqrt(2) * sqrt(fSigma2 * log(1.0 / (fSigma * MIN_HEAT_LEVEL * sqrt(2 * 3.14159))));
    float fX = fMaxX * (fHeatCountRadius);
    float fHeat = (1.0 / (fSigma * sqrt(2 * 3.14159))) * exp((-fX * fX) / (2.0 * fSigma2));
    
    bool useCoreColor = (fHeat > CORE_HEAT_LEVEL);
    vec3 coreColor = float(useCoreColor) * vec3(1.0);
    
    bool useFringeColor = (fHeat > FRINGE_HEAT_LEVEL) && !useCoreColor;
    vec3 fringeColor = float(useFringeColor) * mix(vPrimaryColor, vec3(1.0), (fHeat - FRINGE_HEAT_LEVEL) / FRINGE_HEAT_RANGE);
    
    bool useFlameColor = (fHeat > FLAME_HEAT_LEVEL) && !useFringeColor && !useCoreColor;
    vec3 flameColor = float(useFlameColor) * mix(vSecondaryColor, vPrimaryColor, pow((fHeat - FLAME_HEAT_LEVEL) * float(useFlameColor) / FLAME_HEAT_RANGE, 0.5));
    
    bool useSecondaryColor = (!useCoreColor) && (!useFringeColor) && (!useFlameColor);
    vec3 secondaryColor = float(useSecondaryColor) * vSecondaryColor;
    //return vec3(fHeat);
    return coreColor + fringeColor + flameColor + secondaryColor;
}

// ORB FUNCTIONS BEGIN HERE!

vec4 calcSmoothColorBase(float fRadius, float fIntensity, vec3 vPrimaryColor, vec3 vSecondaryColor, float fOpacity, vec3 blownColor) {
    float fFringeMaxRadius = fRadius * (fIntensity / 120.0);
    float fFringeWidth = fFringeMaxRadius / 8.0;
    float fBlownRadius = fFringeMaxRadius - fFringeWidth;
    float fFadeWidth = fRadius - fFringeMaxRadius;
    
    // White within blown radius, 100% opacity
    bool useBlownRadius = (pixelsDistanceFromCenter < fBlownRadius);
    vec4 blownRadiusColor = float(useBlownRadius) * vec4(blownColor, fOpacity);
    
    // Mix between primary color and white in fringe radius
    bool useFringeRadius = (pixelsDistanceFromCenter < fFringeMaxRadius && fFringeWidth > 0.0 && pixelsDistanceFromCenter >= fBlownRadius);
    float fringeOpacityStep = (pixelsDistanceFromCenter - fBlownRadius);
    float fringeOpacity = (fringeOpacityStep * fOpacity) / fFringeWidth;
    vec3 fringeRadiusColorRGB = mix(vec3(1.0, 1.0, 1.0), vec3(vPrimaryColor), fringeOpacity);
    vec4 fringeRadiusColor = float(useFringeRadius) * vec4(fringeRadiusColorRGB, fOpacity);
    
    
    // Secondary color in fade radius
    bool useFadeRadius = (pixelsDistanceFromCenter < fRadius && (pixelsDistanceFromCenter >= fFringeMaxRadius || fFringeWidth <= 0.0) && fFadeWidth > 0.0);
    float fadeOpacityStep = (pixelsDistanceFromCenter - fFringeMaxRadius);
    float fadeOpacity = 1.0 - (fadeOpacityStep / fFadeWidth);
    fadeOpacity = (fadeOpacity * fadeOpacity) * fOpacity;
    vec4 fadeRadiusColor = float(useFadeRadius) * vec4(vSecondaryColor, fadeOpacity);
    
    // Black otherwise
    bool useBlackRadius = (pixelsDistanceFromCenter > fRadius);
    vec4 blackRadiusColor = float(useBlackRadius) * vec4(0.0);
    
    return (blownRadiusColor + fringeRadiusColor + fadeRadiusColor + blackRadiusColor);
}

float rand(vec2 co){
  // Canonical PRNG from https://stackoverflow.com/questions/12964279/whats-the-origin-of-this-glsl-rand-one-liner
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float FloatIncrementor(float start, float power, float limit, float end, float inputValue) {
    float range = end - start;
    float currStep = inputValue / limit;
    return (start + pow(currStep, power) * range);
}

float CalcShellOpacity(float fRadius, float fIntensity, float fOpacity) {
/*
    ASSERT(iRadius >= 0);
    ASSERT(iShellMaxRadius >= 0);

    iIntensity = Max(0, Min(iIntensity, 100));
    iShellMaxRadius = Min(iShellMaxRadius, iRadius);
    int iEdgeWidth = iRadius - iShellMaxRadius;

    if (retOpacity.GetCount() < iRadius)
        retOpacity.InsertEmpty(iRadius - retOpacity.GetCount());

    CStepIncrementor MaxIntensity(CStepIncrementor::styleOct, 0.0, 1.0, iShellMaxRadius);
    CStepIncrementor MinIntensity(CStepIncrementor::styleLinear, 0.0, 1.0, iShellMaxRadius);
    Metric rMaxIntensityK = iIntensity / 100.0;
    Metric rMinIntensityK = (100 - iIntensity) / 100.0;

    for (int i = 0; i < iShellMaxRadius; i++)
        retOpacity[i] = (BYTE)(byOpacity * ((MaxIntensity.GetAt(i) * rMaxIntensityK) + (MinIntensity.GetAt(i) * rMinIntensityK)));

    CStepIncrementor Fade(CStepIncrementor::styleSquare, 1.0, 0.0, iEdgeWidth + 1);
    for (int i = 0; i < iEdgeWidth; i++)
        retOpacity[iShellMaxRadius + i] = (BYTE)(byOpacity * Fade.GetAt(1 + i));
    */
    float fEdgeWidthForMaxRadius = fRadius * SHELL_EDGE_WIDTH_RATIO;
    float fShellMaxRadius = fRadius - fEdgeWidthForMaxRadius;
    
    float fCappedIntensity = max(0.0, min(fIntensity, 100.0));
    fShellMaxRadius = min(fShellMaxRadius, fRadius);
    float fEdgeWidth = fRadius - fShellMaxRadius;

    float fMaxIntensity = FloatIncrementor(0.0, 8.0, fShellMaxRadius, 1.0, pixelsDistanceFromCenter);
    float fMinIntensity = FloatIncrementor(0.0, 1.0, fShellMaxRadius, 1.0, pixelsDistanceFromCenter);
    float fMaxIntensityK = fIntensity / 100.0;
    float fMinIntensityK = (100 - fIntensity) / 100.0;
    float fFade = FloatIncrementor(1.0, 2.0, fEdgeWidth + 1.0, 1.0, (1.0 + pixelsDistanceFromCenter));

    bool useInnerOpacity = pixelsDistanceFromCenter < fShellMaxRadius;
    float innerOpacity = fOpacity * ((fMaxIntensity * fMaxIntensityK) + (fMinIntensity * fMinIntensityK)) * float(useInnerOpacity);
    bool useOuterOpacity = (pixelsDistanceFromCenter >= fShellMaxRadius) && (pixelsDistanceFromCenter < fEdgeWidth);
    float outerOpacity = fOpacity * fFade * float(useOuterOpacity);
    return innerOpacity + outerOpacity;
}

vec4 calcShellColor(float fRadius, float fIntensity, vec3 vPrimaryColor, vec3 vSecondaryColor, float fShellOpacity) {
/*
    ASSERT(iRadius >= 0);

    if (retColorTable->GetCount() < iRadius)
        retColorTable->InsertEmpty(iRadius - retColorTable->GetCount());

    int iEdgeWidth = mathRound(iRadius * SHELL_EDGE_WIDTH_RATIO);
    int iShellMaxRadius = iRadius - iEdgeWidth;
    int iHoleRadius = iShellMaxRadius * iIntensity / 120;

    TArray<BYTE> OpacityRamp;
    CalcShellOpacity(iRadius, iShellMaxRadius, iIntensity, byOpacity, OpacityRamp);

    //  Initialize table

    for (int i = 0; i < iRadius; i++)
        {
        if (i < iHoleRadius)
            (*retColorTable)[i] = CG32bitPixel(rgbSecondary, OpacityRamp[i]);

        else if (i < iShellMaxRadius)
            {
            int iStep = (i - iHoleRadius);
            DWORD dwBlend = 255 * iStep / (iShellMaxRadius - iHoleRadius);
            (*retColorTable)[i] = CG32bitPixel(CG32bitPixel::Blend(rgbSecondary, rgbPrimary, (BYTE)dwBlend), OpacityRamp[i]);
            }
        else
            (*retColorTable)[i] = CG32bitPixel(rgbPrimary, OpacityRamp[i]);
        }
    }
    */
    float fEdgeWidth = fRadius * SHELL_EDGE_WIDTH_RATIO;
    float fShellMaxRadius = fRadius - fEdgeWidth;
    float fHoleRadius = fShellMaxRadius * fIntensity / 120;
    
    bool useHoleColor = (pixelsDistanceFromCenter < fHoleRadius);
    vec4 holeColor = float(useHoleColor) * vec4(vSecondaryColor, fShellOpacity);
    
    bool useShellColor = (!useHoleColor) && (pixelsDistanceFromCenter < fShellMaxRadius);
    float blend = (pixelsDistanceFromCenter - fHoleRadius) / (fShellMaxRadius - fHoleRadius);
    vec4 shellColor = float(useShellColor) * vec4(mix(vSecondaryColor, vPrimaryColor, blend), fShellOpacity);
    
    bool useOuterColor = (!useHoleColor) && (!useShellColor);
    vec4 outerColor = float(useOuterColor) * vec4(vPrimaryColor, fShellOpacity);
    
    return holeColor + shellColor + outerColor;
}

vec4 calcSmoothColor(float fRadius, float fIntensity, vec3 vPrimaryColor, vec3 vSecondaryColor, float fOpacity) {
    return calcSmoothColorBase(fRadius, fIntensity, vPrimaryColor, vSecondaryColor, fOpacity, vec3(1.0, 1.0, 1.0));
}

vec4 calcBlackHoleColor(float fRadius, float fIntensity, vec3 vPrimaryColor, vec3 vSecondaryColor, float fOpacity) {
    return calcSmoothColorBase(fRadius, fIntensity, vPrimaryColor, vSecondaryColor, fOpacity, vec3(0.0, 0.0, 0.0));
}

vec4 calcFlareColor(float fRadius, float fIntensity, vec3 vPrimaryColor, vec3 vSecondaryColor, float fOpacity) {
    vec3 centerColor = mix(vPrimaryColor, vec3(1.0, 1.0, 1.0), fIntensity / 100.0);
    vec3 edgeColor = vSecondaryColor;
    float fFade = pixelsDistanceFromCenter / fRadius;
    vec3 rgbColor = mix(centerColor, edgeColor, fFade);
    float alpha = max(0.0, fOpacity - ((pixelsDistanceFromCenter * fOpacity) / fRadius));
    return vec4(rgbColor, alpha);
}

vec4 calcDiffractionColor(float fRadius, float fIntensity, vec3 vPrimaryColor, vec3 vSecondaryColor, float fOpacity, vec4 flareColor) {
    float ring_pixel_distance = 2.0;
    float modVal = mod(pixelsDistanceFromCenter, ring_pixel_distance);
    float useEvenColor = abs(modVal - (ring_pixel_distance / 2.0)) / (ring_pixel_distance / 2.0);
    vec3 evenColor = mix(vec3(0.0, 0.0, 0.0), vec3(flareColor), 0.5);
    vec3 oddColor = mix(vec3(flareColor), vec3(1.0, 1.0, 1.0), 0.5);
    vec4 fColor = vec4(mix(oddColor, evenColor, useEvenColor), flareColor[3]);
    return fColor;
}

vec4 calcCloudRadialColor(float fRadius, float fIntensity, vec3 vPrimaryColor, vec3 vSecondaryColor, float fOpacity) {
    float cloudOpacity = FloatIncrementor(fOpacity, 2.0, fRadius, 0.0, pixelsDistanceFromCenter);
    return vec4(1.0, 1.0, 1.0, cloudOpacity);
}

vec4 calcCloudShellRadialColor(float fRadius, float fIntensity, vec3 vPrimaryColor, vec3 vSecondaryColor, float fOpacity) {
    return vec4(1.0, 1.0, 1.0, fOpacity);
}

vec4 calcSmokeRadialColor(float fRadius, float fIntensity, vec3 vPrimaryColor, vec3 vSecondaryColor, float fOpacity) {
    float fOpacityValue = FloatIncrementor(fOpacity, 4.0, fRadius, 0.0, pixelsDistanceFromCenter);
    float fCenterAdj = FloatIncrementor(max(0.0, (70.0 - fIntensity)) / 100.0, 2.0, fRadius, 1.0, pixelsDistanceFromCenter);
    return vec4(mix(vPrimaryColor, vSecondaryColor, pixelsDistanceFromCenter / fRadius), fCenterAdj * fOpacityValue);
}

vec4 calcCloudPixelColor(float fRadius, float fIntensity, float fLookupValue, vec3 vPrimaryColor, vec3 vSecondaryColor, float fOpacity) {
    float fHighAdj = fOpacity;
    float fLowAdj = 1.0 - fHighAdj;
    
    float newOpacity = fHighAdj + (fLookupValue * fLowAdj);
    return vec4(mix(vSecondaryColor, vPrimaryColor, fLookupValue), newOpacity);
}

vec4 calcCloudColor(vec4 radialColor, vec4 pixelColor) {
    return vec4(pixelColor[0], pixelColor[1], pixelColor[2], pixelColor[3] * radialColor[3]);
}

vec4 calcFirecloudPixelColor(float fRadius, float fIntensity, float fLookupValue, vec3 vPrimaryColor, vec3 vSecondaryColor, float fOpacity) {
    float fHighAdj = fOpacity;
    float fLowAdj = 1.0 - fHighAdj;
    
    float newOpacity = fHighAdj + (fLookupValue * fLowAdj);
    return vec4(getExplosionColor(fLookupValue, 1.0, fIntensity, vPrimaryColor, vSecondaryColor), newOpacity);
}

vec4 calcFireblastRadialColor(float fRadius, float fIntensity, float queryRadius, vec3 vPrimaryColor, vec3 vSecondaryColor, float fOpacity) {
    float BLOWN_INTENSITY = 95.0;
    float BLOWN_SIZE = 100.0 - BLOWN_INTENSITY;
    float FIRE_INTENSITY = 80.0;
    float FIRE_SIZE = BLOWN_INTENSITY - FIRE_INTENSITY;
    float GLOW_INTENSITY = 25.0;
    float GLOW_SIZE = FIRE_INTENSITY - GLOW_INTENSITY;
    float FADE_INTENSITY = 0.0;
    float FADE_SIZE = GLOW_INTENSITY - FADE_INTENSITY;
    //CStepIncrementor Opacity(CStepIncrementor::styleSquare, byOpacity, 0.0, iRadius);
    FloatIncrementor(fOpacity, 2.0, fRadius, 0.0, pixelsDistanceFromCenter);
    
    float fOpacityInc = FloatIncrementor(fOpacity, 8.0, fRadius, 0.0, queryRadius);
    float fRadiusInc = FloatIncrementor(0.0, 4.0, fRadius, 1.0, queryRadius);
    
    bool useBlown = (fIntensity > BLOWN_INTENSITY);
    float fBlownFade = min(FloatIncrementor(100.0, 8.0, BLOWN_SIZE, 0.0, fIntensity - BLOWN_INTENSITY), 100.0);
    vec3 fColCenterBlown = mix(vec3(1.0), vPrimaryColor, fBlownFade / 100.0) * float(useBlown);
    vec3 fColEdgeBlown = mix(vec3(1.0), vPrimaryColor, fBlownFade / 100.0) * float(useBlown);
    
    bool useFire = (fIntensity > FIRE_INTENSITY) && !useBlown;
    float fFireFade = min(FloatIncrementor(100.0, 8.0, FIRE_SIZE, 0.0, fIntensity - FIRE_INTENSITY), 100.0);
    vec3 fColCenterFire = vPrimaryColor * float(useFire);
    vec3 fColEdgeFire = mix(vPrimaryColor, vSecondaryColor, fFireFade / 100.0) * float(useFire);
    
    bool useGlow = (fIntensity > GLOW_INTENSITY) && !useFire && !useBlown;
    float fGlowFade = min(FloatIncrementor(100.0, 8.0, GLOW_SIZE, 0.0, fIntensity - GLOW_INTENSITY), 100.0);
    vec3 fColCenterGlow = mix(vPrimaryColor, vSecondaryColor, fGlowFade / 100.0) * float(useGlow);
    vec3 fColEdgeGlow = vSecondaryColor * float(useGlow);
    
    bool useFade = (fIntensity > FADE_INTENSITY) && !useGlow && !useFire && !useBlown;
    float fFadeFade = min(FloatIncrementor(100.0, 8.0, FADE_SIZE, 0.0, fIntensity - FADE_INTENSITY), 100.0);
    vec3 fColCenterFade = vSecondaryColor * float(useFade);
    vec3 fColEdgeFade = mix(vSecondaryColor, vec3(0.0), fFadeFade / 100.0) * float(useFade);
    
    vec3 fColCenter = fColCenterBlown + fColCenterFire + fColCenterGlow + fColCenterFade;
    vec3 fColEdge = fColEdgeBlown + fColEdgeFire + fColEdgeGlow + fColEdgeFade;
    
    //return vec4(vec3(fColCenterGlow), 1.0);
    return vec4(mix(fColCenter, fColEdge, fRadiusInc), fOpacityInc);
}

vec4 calcFireblastPixelColor(float fRadius, float fIntensity, float queryRadius, vec3 vPrimaryColor, vec3 vSecondaryColor, float fOpacity) {
    float fIntensityInc = min(FloatIncrementor(100.0, 2.0, 100.0, 0.0, fIntensity), 100.0);
    float fOpacityInc = FloatIncrementor(fOpacity, 1.0, fRadius, 0.0, queryRadius);
    float fRadiusInc = FloatIncrementor(0.0, 1.0 / 8.0, fRadius, 1.0, queryRadius);
    
    vec3 fColCenter = mix(vPrimaryColor, vSecondaryColor, fIntensityInc / 100.0);
    vec3 fColEdge = mix(vSecondaryColor, vec3(0.0), fIntensityInc / 100.0);
    
    return vec4(mix(fColCenter, fColEdge, fRadiusInc), fOpacityInc);
}

vec4 calcFireblastColor(float fRadius, float fIntensity, vec3 vPrimaryColor, vec3 vSecondaryColor, float fOpacity) {
    // Adjust radius between 1.0 times radius and (1.0 + disrupt) times radius
    float ANGLE_DISTORTION_MULTIPLIER = 0.4;
    // TODO: Add a seed!
    float angle_phase_adj = animationSeed * 3;
    float rand_noise_seed = animationSeed * 1;
    //float fAngle = (dot(quadPos, vec2(0.0, 1.0)) / length(quadPos));
    float fAngle = abs(mod(atan(quadPos.y, quadPos.x) + angle_phase_adj, (3.14159 * 2)) - 3.14159);//(dot(quadPos, vec2(0.0, 1.0)) / length(quadPos));
    float fRand = ((cnoise(vec3(fAngle * ANGLE_DISTORTION_MULTIPLIER + rand_noise_seed)) / 2.0) + 0.5);
    float fRadAdjFactor = (1 + (fRand * float(distortion) / 100.0));
    vec2 fAdjPos = quadPos * fRadAdjFactor;
    float fAdjRadius = pixelsDistanceFromCenter * fRadAdjFactor;
    //float fAdjRadius = pixelsDistanceFromCenter;
    bool useBlack = (fAdjRadius >= fRadius);
    
    float fNoiseValue = fbm(fAdjPos, current_tick) + 0.5;

    vec4 fireblastPixelColor = calcFireblastPixelColor(fRadius, fIntensity, fAdjRadius, vPrimaryColor, vSecondaryColor, fOpacity);    
    vec4 fireblastRadialColor = calcFireblastRadialColor(fRadius, fIntensity, fAdjRadius, vPrimaryColor, vSecondaryColor, fOpacity);    
    
    //return vec4(fAdjRadius) / 300;
    return mix(fireblastRadialColor, fireblastPixelColor, fNoiseValue) * float(!useBlack);
}

vec4 calcPrimaryColor(int iStyle, float fRadius, float fIntensity, vec3 vPrimaryColor, vec3 vSecondaryColor, float fOpacity, float noise) {
    // Switch for styles.
    // This is analogous to CalcSphericalColorTable in georgecode.
    float shellOpacity = CalcShellOpacity(fRadius, fIntensity, fOpacity); // used for shell, cloudshell
    vec4 cloudRadialColor = calcCloudRadialColor(fRadius, fIntensity, vPrimaryColor, vSecondaryColor, fOpacity); // used for cloud, firecloud
    vec4 cloudShellRadialColor = calcCloudShellRadialColor(fRadius, fIntensity, vPrimaryColor, vSecondaryColor, shellOpacity); // used for cloudshell
    vec4 smokeRadialColor = calcSmokeRadialColor(fRadius, fIntensity, vPrimaryColor, vSecondaryColor, fOpacity); // used for smoke
//    vec4 fireblastRadialColor = calcFireblastRadialColor(fRadius, fIntensity, vPrimaryColor, vSecondaryColor, fOpacity); // used for smoke
    vec4 cloudPixelColor = calcCloudPixelColor(fRadius, fIntensity, noise, vPrimaryColor, vSecondaryColor, fOpacity); // used for cloud, cloudshell, smoke
    vec4 firecloudPixelColor = calcFirecloudPixelColor(fRadius, fIntensity, noise, vPrimaryColor, vSecondaryColor, fOpacity); // used for firecloud
    vec4 fireblastColor = calcFireblastColor(fRadius, fIntensity, vPrimaryColor, vSecondaryColor, fOpacity); // used for firecloud

    vec4 cloudColor = calcCloudColor(cloudRadialColor, cloudPixelColor);
    vec4 firecloudColor = calcCloudColor(cloudRadialColor, firecloudPixelColor);
    vec4 fireCloudshellColor = calcCloudColor(cloudShellRadialColor, firecloudPixelColor);
    vec4 cloudShellColor = calcCloudColor(cloudShellRadialColor, cloudPixelColor);
    vec4 smokeColor = calcCloudColor(smokeRadialColor, cloudPixelColor);
    
    vec4 smoothColor = calcSmoothColor(fRadius, fIntensity, vPrimaryColor, vSecondaryColor, fOpacity);
    vec4 blackHoleColor = calcBlackHoleColor(fRadius, fIntensity, vPrimaryColor, vSecondaryColor, fOpacity);
    vec4 shellColor = calcShellColor(fRadius, fIntensity, vPrimaryColor, vSecondaryColor, shellOpacity);
    vec4 flareColor = calcFlareColor(fRadius, fIntensity, vPrimaryColor, vSecondaryColor, fOpacity);
    vec4 diffractionColor = calcDiffractionColor(fRadius, fIntensity, vPrimaryColor, vSecondaryColor, fOpacity, flareColor);
    
    vec4 finalColor = (
        (cloudColor * float(style == styleCloud)) +
        (firecloudColor * float(style == styleFirecloud)) +
        (fireCloudshellColor * float(style == styleFireCloudshell)) +
        (smokeColor * float(style == styleSmoke)) +
        (smoothColor * float(style == styleSmooth)) +
        (fireblastColor * float(style == styleFireblast)) +
        (blackHoleColor * float(style == styleBlackHole)) +
        (shellColor * float(style == styleShell)) +
        (flareColor * float(style == styleFlare)) +
        (diffractionColor * float(style == styleDiffraction)) +
        (cloudShellColor * float(style == styleCloudshell)) +
        (flareColor * float(style == styleLightning))
    );
    return (finalColor);
}

float calcAnimateExplodeSecondaryOpacity(float fRadius, float fOpacity, float fFrame) {
    float fEndFade = float(lifetime) / 3.0;
    float fEndFadeStart = float(lifetime) - fEndFade;
    
    float useEndFadeStart = float(fFrame > fEndFadeStart);
    float fFadeOpacity = useEndFadeStart * (fEndFade - (fFrame - fEndFadeStart)) / fEndFade;
    
    float useOpacity = float(fFrame <= fEndFadeStart);
    
    return (fFadeOpacity * useEndFadeStart) + (fOpacity * useOpacity);
    
}

vec4 calcAnimationColor(float animatedNoise, float scaledNoise) {
    // Obtain the following attributes for the sphere color calculation:
    // m_iStyle, m_iRadius, m_iIntensity, m_rgbPrimaryColor, m_rgbSecondaryColor, m_byOpacity.
    // Note that style, secondaryColor are not changed by animation style.
        //CStepIncrementor Opacity(CStepIncrementor::styleSquare, byOpacity, 0.0, iRadius);
        //FloatIncrementor(fOpacity, 2.0, fRadius, 0.0, pixelsDistanceFromCenter);
    float fLifetime = float(lifetime);
    float fCurrframe = float(currFrame);
    float animateDissipateRadius = FloatIncrementor(0.2 * orbRadius, 0.5, fLifetime, orbRadius, fCurrframe);
    float animateDissipateIntensity = FloatIncrementor(intensity, 1.0, fLifetime, 0.0, fCurrframe);
    float animateDissipateDetail = FloatIncrementor(float(detail) / 100.0f, 1.0, fLifetime, float(detail) / 1000.0f, fCurrframe);
    float animateDissipateColorFade = FloatIncrementor(0.0, 1.0, fLifetime, 1.0, fCurrframe);

    vec3 animateDissipatePrimaryColor = mix(primaryColor, secondaryColor, animateDissipateColorFade);
    float animateDissipateNoise = scaledNoise;
    float animateDissipateOpacity = opacityAdj;
    float animateDissipateSecondaryOpacity = secondaryOpacity;

    float animateExplodeRadius = animateDissipateRadius;
    float animateExplodeDetail = animateDissipateDetail;
    float animateExplodeHeat = FloatIncrementor(100.0, 2.0, fLifetime, 0.0, fCurrframe);
	float animateExplodeIntensity = animateExplodeHeat;

    vec3 animateExplodePrimaryColor = primaryColor;
    float animateExplodeNoise = scaledNoise;
    float animateExplodeOpacity = calcAnimateExplodeSecondaryOpacity(orbRadius, opacityAdj, fCurrframe);
    float animateExplodeSecondaryOpacity = animateExplodeOpacity;
    
    float animateFadeProgress = (fLifetime - fCurrframe) / fLifetime;
    float animateFadeRadius = orbRadius * animateFadeProgress;
    float animateFadeIntensity = intensity * animateFadeProgress;
    float animateFadeDetail = animateDissipateDetail;

    vec3 animateFadePrimaryColor = primaryColor;
    float animateFadeNoise = scaledNoise;
    float animateFadeOpacity = opacityAdj;
    float animateFadeSecondaryOpacity = secondaryOpacity;
    
    float animateFlickerMultiplier = max(0.5, min(1.0 + (0.25 * cnoise(vec3(0.0, 0.0, current_tick))), 2.0));
    float animateFlickerRadius = orbRadius * animateFlickerMultiplier;
    float animateFlickerIntensity = intensity * animateFlickerMultiplier;
    animateFlickerIntensity = (float(style == styleFireblast) * (min(max(animateFlickerIntensity, 0.0), 100.0))) + (float(style != styleFireblast) * animateFlickerIntensity);
    vec3 animateFlickerPrimaryColor = primaryColor;
    float animateFlickerDetail = detail;
    float animateFlickerNoise = scaledNoise;
    float animateFlickerOpacity = opacityAdj;
    float animateFlickerSecondaryOpacity = secondaryOpacity;
    
    float animateDimRadius = orbRadius;
    float animateDimIntensity = intensity;
    vec3 animateDimPrimaryColor = mix(primaryColor, secondaryColor, FloatIncrementor(0.0, 1.0, fLifetime, 1.0, fCurrframe));
    float animateDimDetail = detail;
    float animateDimNoise = scaledNoise;
    float animateDimOpacity = FloatIncrementor(opacityAdj, 1.0, fLifetime, secondaryOpacity, fCurrframe);
    float animateDimSecondaryOpacity = secondaryOpacity;
    
    float animateNoneRadius = orbRadius;
    float animateNoneIntensity = intensity;
    vec3 animateNonePrimaryColor = primaryColor;
    float animateNoneDetail = detail;
    float animateNoneNoise = scaledNoise;
    float animateNoneOpacity = opacityAdj;
    float animateNoneSecondaryOpacity = secondaryOpacity;
    
    float finalRadius = (
        (animateNoneRadius * float(animation == animateNone)) +
        (animateDissipateRadius * float(animation == animateDissipate)) +
        (animateExplodeRadius * float(animation == animateExplode)) +
        (animateFadeRadius * float(animation == animateFade)) +
        (animateFlickerRadius * float(animation == animateFlicker)) +
        (animateDimRadius * float(animation == animateDim))
    );
    
    float finalIntensity = (
        (animateNoneIntensity * float(animation == animateNone)) +
        (animateDissipateIntensity * float(animation == animateDissipate)) +
        (animateExplodeIntensity * float(animation == animateExplode)) +
        (animateFadeIntensity * float(animation == animateFade)) +
        (animateFlickerIntensity * float(animation == animateFlicker)) +
        (animateDimIntensity * float(animation == animateDim))
    );
    
    float finalDetail = (
        (animateNoneDetail * float(animation == animateNone)) +
        (animateDissipateDetail * float(animation == animateDissipate)) +
        (animateExplodeDetail * float(animation == animateExplode)) +
        (animateFadeDetail * float(animation == animateFade)) +
        (animateFlickerDetail * float(animation == animateFlicker)) +
        (animateDimDetail * float(animation == animateDim))
    );
    
    vec3 finalPrimaryColor = (
        (animateNonePrimaryColor * float(animation == animateNone)) +
        (animateDissipatePrimaryColor * float(animation == animateDissipate)) +
        (animateExplodePrimaryColor * float(animation == animateExplode)) +
        (animateFadePrimaryColor * float(animation == animateFade)) +
        (animateFlickerPrimaryColor * float(animation == animateFlicker)) +
        (animateDimPrimaryColor * float(animation == animateDim))
    );
    
    float finalNoise = (
        (animateNoneNoise * float(animation == animateNone)) +
        (animateDissipateNoise * float(animation == animateDissipate)) +
        (animateExplodeNoise * float(animation == animateExplode)) +
        (animateFadeNoise * float(animation == animateFade)) +
        (animateFlickerNoise * float(animation == animateFlicker)) +
        (animateDimNoise * float(animation == animateDim))
    );
    
    float finalOpacity = (
        (animateNoneOpacity * float(animation == animateNone)) +
        (animateDissipateOpacity * float(animation == animateDissipate)) +
        (animateExplodeOpacity * float(animation == animateExplode)) +
        (animateFadeOpacity * float(animation == animateFade)) +
        (animateFlickerOpacity * float(animation == animateFlicker)) +
        (animateDimOpacity * float(animation == animateDim))
    );
    
    float finalSecondaryOpacity = (
        (animateNoneSecondaryOpacity * float(animation == animateNone)) +
        (animateDissipateSecondaryOpacity * float(animation == animateDissipate)) +
        (animateExplodeSecondaryOpacity * float(animation == animateExplode)) +
        (animateFadeSecondaryOpacity * float(animation == animateFade)) +
        (animateFlickerSecondaryOpacity * float(animation == animateFlicker)) +
        (animateDimSecondaryOpacity * float(animation == animateDim))
    );
    
//    return vec4(0.5);
    // We only use secondary color table for fireblast.
    return calcPrimaryColor(style, finalRadius, finalIntensity, finalPrimaryColor, secondaryColor, finalOpacity, scaledNoise);
}

void main(void)
{
    // Calculate the animation and style intermediates.
    // Really, the style is just what kind of texture we use (fireblast, diffraction or cloud).
    // For fireblast/cloud the fractal pattern can be done with Perlin noise FBM technique.
    float animatedNoise = fbm(quadPos, current_tick) + 0.5;
    float scaledNoise = fbm(quadPos / (orbRadius / 200), current_tick) + 0.5;
    //fragColor = vec4(rand(vec2(pixelsDistanceFromCenter)));
	
	vec4 finalColor = calcAnimationColor(animatedNoise, scaledNoise);
	bool alphaIsZero = finalColor[3] < 0.01;
	gl_FragDepth = depth + float(alphaIsZero);
    fragColor = finalColor;
    //fragColor = vec4(0.2);
    //fragColor = vec4(fbm(quadPos* 1.0, current_tick) + 0.5);
}

