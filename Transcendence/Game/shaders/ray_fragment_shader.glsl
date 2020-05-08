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

layout (location = 0) in vec2 quadPos;
layout (location = 1) flat in int reshape;
layout (location = 2) flat in int widthAdjType;
layout (location = 3) flat in int opacity;
layout (location = 4) flat in int grainyTexture;
layout (location = 5) in float depth;
layout (location = 6) in float intensity;
layout (location = 7) in vec3 primaryColor;
layout (location = 8) in vec3 secondaryColor;
layout (location = 9) in float waveCyclePos;
layout (location = 10) flat in int colorTypes;
layout (location = 11) in float opacityAdj;
layout (location = 12) in vec2 quadSize;

uniform float current_tick;

out vec4 fragColor;

float PI = 3.14159;
float BLOB_WAVE_SIZE = 0.3;
float WAVY_WAVELENGTH_FACTOR = 1.0;
float JAGGED_AMPLITUDE = 0.45;
float JAGGED_WAVELENGTH_FACTOR = 0.33;
float WHIPTAIL_AMPLITUDE = 0.45;
float WHIPTAIL_WAVELENGTH_FACTOR = 1.0;
float WHIPTAIL_DECAY = 0.13;

// Copy of EWidthAdjTypes enum from SFXRay.cpp
int widthAdjStraight = 0;
int widthAdjBlob = 1;
int widthAdjDiamond = 2;
int widthAdjJagged = 3;
int widthAdjOval = 4;
int widthAdjTaper = 5;
int widthAdjCone = 6;
int widthAdjWhiptail = 7;
int widthAdjSword = 8;

// Copy of EOpacityTypes enum from SFXRay.cpp
int opacityGlow = 1;
int opacityGrainy = 2;
int opacityTaperedGlow = 3;
int opacityTaperedExponentialGlow = 4;


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

float rand(vec2 co){
  // Canonical PRNG from https://stackoverflow.com/questions/12964279/whats-the-origin-of-this-glsl-rand-one-liner
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float widthCone(float rInputValue)
{
    return (-(rInputValue - 1.0) / 2.0);
}

float widthDiamond(float rInputValue)
{
    return (1.0 - abs(rInputValue));
}

float widthSword(float rInputValue)
    {
    // Just like taper, but with triangular rather than circular head from head to peak
    float rQuadHalfLength = 1.0;
    float rSwordFraction = 5.0;
    float rPeakDistance = ((2.0 * rQuadHalfLength) / (rSwordFraction / 1.0));
    float rPeakPoint = rQuadHalfLength - rPeakDistance;
    float rTipCoord = ((rPeakPoint - rInputValue) / rPeakDistance);
    bool inTipPortion = rQuadHalfLength - rInputValue < rPeakDistance;
    float rTipWidth = (1.0 - abs(rTipCoord)) * float(inTipPortion);
    // Tapered from peak onwards
    float rTaperedDY = (2.0 * rQuadHalfLength) - rPeakDistance;
    float rTaperedWidth = ((rInputValue - (-rQuadHalfLength)) / rTaperedDY) * float(!inTipPortion);
    return rTaperedWidth + max(rTipWidth, 0);
    }

float widthTapered(float rInputValue)
{
    // Circular from head to peak
    float rQuadHalfLength = 1.0;
    float rTaperFraction = 8.0;
    float rPeakDistance = ((2.0 * rQuadHalfLength) / (rTaperFraction / 1.0));
    float rPeakPoint = rQuadHalfLength - rPeakDistance;
    float rCircleCoord = ((rPeakPoint - rInputValue) / rPeakDistance);
    bool inCircularPortion = rQuadHalfLength - rInputValue < rPeakDistance;
    float rCircleWidth = sqrt(rQuadHalfLength - (rCircleCoord * rCircleCoord)) * float(inCircularPortion);
    // Tapered from peak onwards
    float rTaperedDY = (2.0 * rQuadHalfLength) - rPeakDistance;
    float rTaperedWidth = ((rInputValue - (-rQuadHalfLength)) / rTaperedDY) * float(!inCircularPortion);
    return rTaperedWidth + max(rCircleWidth, 0);
}

float widthOval(float rInputValue)
    {
    float rMidPoint = 0.0;
    float rQuadHalfLength = 1.0;
    //  If before the midpoint, then we have a distance of sqrt(0.5 - (pos^2))
    float rBeforeMidpointValue = sqrt((rQuadHalfLength - (rInputValue * rInputValue)) * (1 / rQuadHalfLength)) * float(rInputValue < rMidPoint);
    //  If after the midpoint, then we have a distance of sqrt(0.5 + (pos^2))
    float rAfterMidpointValue = sqrt((rQuadHalfLength - (rInputValue * rInputValue)) * (1 / rQuadHalfLength)) * float(rInputValue > rMidPoint);
    return rBeforeMidpointValue + rAfterMidpointValue;
    }
    
float widthWave(float rInputValue, float rAmplitude, float rWavelength, float rDecay, float rPhase)
    {
    //  Wavelength decays (increases with each cycle)
    float rStartWavelength = max(0.001, rWavelength);
    
    //  Cycle around a mid point which is 1.0 - amplitude
    float rPos = 1.0 - rAmplitude;
    
    //  Offset wave to produce an animation
    float rAngleStart = 2.0 * PI * (1.0 - rPhase);
    
    //  Determine the wavelength and the angle
    float rPointWavelength = (rStartWavelength + (rInputValue * rDecay));
    float rAngle = rAngleStart + (2.0 * PI * (rInputValue / rPointWavelength));
    return (sin(rAngle) * rAmplitude) + rPos;
    
    
    }

float widthRandomWaves(float rInputValue, float rAmplitude, float rWavelength, float seed)
    //  Note, rAmplitude and rWavelength both assume that the quad's size is 2.0.
    //  
    {
    int i, v;
    int iNumHarmonics = 2;
    
    //  Peak size is related to intensity.
    float iStdPeakSize = max(0.001, rWavelength);
    //  Cycle around the mid-point
    float rPos = 1.0 - rAmplitude;
    
    //  Set a value to store the last sine function result
    float rRandValue = rand(vec2(current_tick + seed, current_tick + seed));
    float rPrevSinValue = int(rRandValue * 100.0);
    float rTotalSumValue = 0;
    
    //  Loop 5 times
    for (i = 0; i < iNumHarmonics; i++)
        {
        //  Amplitude is between 0.2 and 0.8 times rAmplitude, divided by number of harmonics
        float rSinusAmplitude = (float(rAmplitude) * ((rRandValue * 0.8) + 0.2)) / float(iNumHarmonics);
        rRandValue = rand(vec2(rRandValue + 1, rRandValue + 1));
        //  Wavelength is between 0.5 and 2.5 times standard peak size
        float rSinusWavelength = ((0.5 + (rRandValue * 2.0)) * float(iStdPeakSize));
        float rSinusFrequency = (2.0 * PI) / rSinusWavelength;
        float rSinusPhase = rRandValue;
        rPrevSinValue = sin(((rInputValue - 1.0) * rSinusFrequency) + rSinusPhase) * rSinusAmplitude;
        rTotalSumValue += rPrevSinValue;
        rRandValue = rand(vec2(rRandValue + 2, rRandValue + 2));
        }
    return (rTotalSumValue + rPos);
    }

vec3 blendVectors(vec3 rgbFrom, vec3 rgbTo, float rFade)
{
    float rFadeVal = max(0.0, min(1.0, rFade));
    vec3 diff = rgbTo - rgbFrom;
    return (rgbFrom + (diff * rFadeVal));
}
    
vec3 calcColorGlow(vec3 primaryColor, vec3 secondaryColor, float rWidthCount, float iIntensity, float distanceFromCenter)
{
    // Note, Intensity should be a percentage of the ray's width.
    float BRIGHT_FACTOR = 0.0025;

    float centerBlendIntensity = min(50.0, iIntensity) / 50.0;
    float rBrightPoint = BRIGHT_FACTOR * rWidthCount * iIntensity;
    vec3 innerColor = blendVectors(primaryColor, vec3(1.0, 1.0, 1.0), centerBlendIntensity);
    
    float rFadeInc = max(0.0f, 1.0f / (rWidthCount - rBrightPoint)) * (distanceFromCenter - rBrightPoint);
    vec3 outerColor = blendVectors(secondaryColor, primaryColor, 1.0f - rFadeInc);
    
    float useInnerColor = float(distanceFromCenter < rBrightPoint);
    float useOuterColor = float(distanceFromCenter >= rBrightPoint);
    return (innerColor * useInnerColor) + (outerColor * useOuterColor);
}

float calcOpacityGlow(float rWidthCount, float rIntensity, float distanceFromCenter)
{
    float SOLID_FACTOR = 0.004;
    float MIN_GLOW_LEVEL = 0.6;
    float GLOW_FACTOR = 0.004;
    
    // Solid opacity from center to peak
    float rPeakPoint = SOLID_FACTOR * rIntensity * rWidthCount;
    float useSolid = float(distanceFromCenter < rPeakPoint);
    
    // Decay exponentially to edge
    float useNonSolid = float(distanceFromCenter >= rPeakPoint);
    float rGlowLevel = MIN_GLOW_LEVEL + (rIntensity * GLOW_FACTOR);
    float rGlowInc = max(0.0f, 1.0f / (rWidthCount - rPeakPoint)) * (distanceFromCenter - rPeakPoint);
    float rGlow = 1.0;
    float outerGlow = rGlowLevel * (rGlow - rGlowInc) * (rGlow - rGlowInc);
    
    return (useSolid) + (useNonSolid * outerGlow);
}

float calcOpacityTaperedGlow(float rWidthCount, float rIntensity, float distanceFromCenter_w, float coordinate_l)
{
    float SOLID_FACTOR = 0.004;
    float MIN_GLOW_LEVEL = 0.6;
    float GLOW_FACTOR = 0.004;
    float rLengthCount = 1.0;
    
    float rPeakPoint = SOLID_FACTOR * rIntensity * rWidthCount;
    // Fade out linearly after the 1/3 point
    float rMinFadePoint = 0.0; // TODO: fix this to 300 pixels
    float rFadePoint = max(rMinFadePoint, 0.3);
    float rTaperInc = max(0.0, 1.0 / (2.0 - rFadePoint));
    
    // Solid from center to peak plus taper
    // If before the fade point lengthwise, and before the peak point widthwise, then solid
    float useSolid = float(distanceFromCenter_w < rPeakPoint);// && (coordinate_l < rFadePoint));
    
    // Otherwise, if after the fade point lengthwise, decrease by taperInc
    float taperComponent = (1.0 - (max(0.0, rFadePoint - coordinate_l)) * rTaperInc);
    
    
    // Decay exponentially to edge
    float useNontaperedOpacity = float(coordinate_l < rFadePoint);
    float useNonSolid = float(distanceFromCenter_w >= rPeakPoint);
    float rGlowLevel = MIN_GLOW_LEVEL + (rIntensity * GLOW_FACTOR);
    float rGlowInc = max(0.0f, 1.0f / (rWidthCount - rPeakPoint)) * (distanceFromCenter_w - rPeakPoint);
    float rGlow = 1.0;
    float outerGlow = rGlowLevel * (rGlow - rGlowInc) * (rGlow - rGlowInc) * taperComponent;
    
    return (useSolid * taperComponent) + (useNonSolid * outerGlow);
}

float calcOpacityTaperedExponentialGlow(float rWidthCount, float rIntensity, float distanceFromCenter_w, float coordinate_l)
{
    float SOLID_FACTOR = 0.004;
    float MIN_GLOW_LEVEL = 0.6;
    float GLOW_FACTOR = 0.004;
    float rLengthCount = 1.0;
    
    float rPeakPoint = SOLID_FACTOR * rIntensity * rWidthCount;
    // Fade out linearly after the 1/3 point
    float rMinFadePoint = 0.0; // TODO: fix this to 300 pixels
    float rFadePoint = max(rMinFadePoint, 0.3);
    float rTaperInc = max(0.0, 1.0 / (2.0 - rFadePoint));
    
    // Solid from center to peak plus taper
    // If before the fade point lengthwise, and before the peak point widthwise, then solid
    float useSolid = float(distanceFromCenter_w < rPeakPoint);// && (coordinate_l < rFadePoint));
    
    // Otherwise, if after the fade point lengthwise, decrease by taperInc
    float taperComponent = (1.0 - (max(0.0, rFadePoint - coordinate_l)) * rTaperInc);
    
    
    // Decay exponentially to edge
    float useNontaperedOpacity = float(coordinate_l < rFadePoint);
    float useNonSolid = float(distanceFromCenter_w >= rPeakPoint);
    float rGlowLevel = MIN_GLOW_LEVEL + (rIntensity * GLOW_FACTOR);
    float rGlowInc = max(0.0f, 1.0f / (rWidthCount - rPeakPoint)) * (distanceFromCenter_w - rPeakPoint);
    float rGlow = 1.0;
    float outerGlow = rGlowLevel * (rGlow - rGlowInc) * (rGlow - rGlowInc) * taperComponent * taperComponent;
    
    return (useSolid * taperComponent * taperComponent) + (useNonSolid * outerGlow);
}

void main(void)
{
    float center_point = 0.0; // Remember to remove this in the real shader!!
    vec2 uv = -1. + 2. * quadPos;
    vec2 real_texcoord = quadPos;
    
    float distanceFromCenter = abs(real_texcoord[1] - center_point);
    
    float grains_x = quadSize[0] / 20.0f;
    float grains_y = quadSize[1] / 20.0f;
    float perlinNoise = (0.5 + (cnoise(vec3(quadPos[0] * grains_x, quadPos[1] * grains_y, current_tick * 100)) / 2.0));
    float grains = (perlinNoise * float(grainyTexture == opacityGrainy) * 2) + float(grainyTexture == 0);

    float blobWidthTop = widthRandomWaves(real_texcoord[0], BLOB_WAVE_SIZE, WAVY_WAVELENGTH_FACTOR * (quadSize[1] / quadSize[0]), 1.1);
    float blobWidthBottom = blobWidthTop;
    //float blobWidthBottom = widthRandomWaves(real_texcoord[0], BLOB_WAVE_SIZE, WAVY_WAVELENGTH_FACTOR * (quadSize[1] / quadSize[0]), 2.2);
    float jaggedWidthTop = widthRandomWaves(real_texcoord[0], JAGGED_AMPLITUDE, JAGGED_WAVELENGTH_FACTOR * (quadSize[1] / quadSize[0]), 3.3);
    float jaggedWidthBottom = jaggedWidthTop;
    //float jaggedWidthBottom = widthRandomWaves(real_texcoord[0], JAGGED_AMPLITUDE, JAGGED_WAVELENGTH_FACTOR * (quadSize[1] / quadSize[0]), 4.4);
    float whiptailWidthTop = widthWave(real_texcoord[0], WHIPTAIL_AMPLITUDE, WHIPTAIL_WAVELENGTH_FACTOR * (quadSize[1] / quadSize[0]), WHIPTAIL_DECAY, waveCyclePos);
    float whiptailWidthBottom = widthWave(real_texcoord[0], WHIPTAIL_AMPLITUDE, WHIPTAIL_WAVELENGTH_FACTOR * (quadSize[1] / quadSize[0]), WHIPTAIL_DECAY, waveCyclePos + 0.5);
    float taperWidth = widthTapered(real_texcoord[0]);
    float coneWidth = widthCone(real_texcoord[0]);
    float diamondWidth = widthDiamond(real_texcoord[0]);
    float ovalWidth = widthOval(real_texcoord[0]);
    float swordWidth = widthSword(real_texcoord[0]);
    float straightWidth = 1.0;
    
    float taperAdjTop = (
        (straightWidth * float(reshape == widthAdjStraight)) + 
        (blobWidthTop * float(reshape == widthAdjBlob)) + 
        (diamondWidth * float(reshape == widthAdjDiamond)) + 
        (coneWidth * float(reshape == widthAdjCone)) + 
        (taperWidth * float(reshape == widthAdjTaper)) + 
        (ovalWidth * float(reshape == widthAdjOval)) + 
        (jaggedWidthTop * float(reshape == widthAdjJagged)) + 
        (whiptailWidthTop * float(reshape == widthAdjWhiptail)) +
		(swordWidth * float(reshape == widthAdjSword))
    );
    float taperAdjBottom = (
        (straightWidth * float(reshape == widthAdjStraight)) + 
        (blobWidthBottom * float(reshape == widthAdjBlob)) + 
        (diamondWidth * float(reshape == widthAdjDiamond)) + 
        (coneWidth * float(reshape == widthAdjCone)) + 
        (taperWidth * float(reshape == widthAdjTaper)) + 
        (ovalWidth * float(reshape == widthAdjOval)) + 
        (jaggedWidthBottom * float(reshape == widthAdjJagged)) + 
        (whiptailWidthBottom * float(reshape == widthAdjWhiptail)) +
		(swordWidth * float(reshape == widthAdjSword))
    );
    
    float widthAdjTop = (
        (straightWidth * float(widthAdjType == widthAdjStraight)) + 
        (blobWidthTop * float(widthAdjType == widthAdjBlob)) + 
        (diamondWidth * float(widthAdjType == widthAdjDiamond)) + 
        (coneWidth * float(widthAdjType == widthAdjCone)) + 
        (taperWidth * float(widthAdjType == widthAdjTaper)) + 
        (ovalWidth * float(widthAdjType == widthAdjOval)) + 
        (jaggedWidthTop * float(widthAdjType == widthAdjJagged)) + 
        (whiptailWidthTop * float(widthAdjType == widthAdjWhiptail)) +
		(swordWidth * float(widthAdjType == widthAdjSword))
    );
    float widthAdjBottom = (
        (straightWidth * float(widthAdjType == widthAdjStraight)) + 
        (blobWidthBottom * float(widthAdjType == widthAdjBlob)) + 
        (diamondWidth * float(widthAdjType == widthAdjDiamond)) + 
        (coneWidth * float(widthAdjType == widthAdjCone)) + 
        (taperWidth * float(widthAdjType == widthAdjTaper)) + 
        (ovalWidth * float(widthAdjType == widthAdjOval)) + 
        (jaggedWidthBottom * float(widthAdjType == widthAdjJagged)) + 
        (whiptailWidthBottom * float(widthAdjType == widthAdjWhiptail)) +
		(swordWidth * float(widthAdjType == widthAdjSword))
    );
    
    float limitTop = taperAdjTop * widthAdjTop;
    float limitBottom = taperAdjBottom * widthAdjBottom;

    bool pixelInUpperBounds = ((real_texcoord[1] - center_point) < limitTop) && ((real_texcoord[1] - center_point) > 0);
    bool pixelInLowerBounds = ((real_texcoord[1] - center_point) > (-limitBottom)) && ((real_texcoord[1] - center_point) < 0);
    float pixelWithinBounds = float(pixelInUpperBounds || pixelInLowerBounds);
    float topOpacityGlow = calcOpacityGlow(limitTop, intensity, distanceFromCenter);
    float bottomOpacityGlow = calcOpacityGlow(limitBottom, intensity, distanceFromCenter);
    float topOpacityTaperedGlow = calcOpacityTaperedGlow(limitTop, intensity, distanceFromCenter, real_texcoord[0]);
    float bottomOpacityTaperedGlow = calcOpacityTaperedGlow(limitBottom, intensity, distanceFromCenter, real_texcoord[0]);
    float topOpacityTaperedExponentialGlow = calcOpacityTaperedExponentialGlow(limitTop, intensity, distanceFromCenter, real_texcoord[0]);
    float bottomOpacityTaperedExponentialGlow = calcOpacityTaperedExponentialGlow(limitBottom, intensity, distanceFromCenter, real_texcoord[0]);
	
    float topOpacity = (
        (topOpacityGlow * float(opacity == opacityGlow)) + 
        (topOpacityTaperedGlow * float(opacity == opacityTaperedGlow)) +
        (topOpacityTaperedExponentialGlow * float(opacity == opacityTaperedExponentialGlow))
    ) * opacityAdj;
    float bottomOpacity = (
        (bottomOpacityGlow * float(opacity == opacityGlow)) + 
        (bottomOpacityTaperedGlow * float(opacity == opacityTaperedGlow)) +
        (bottomOpacityTaperedExponentialGlow * float(opacity == opacityTaperedExponentialGlow))
    ) * opacityAdj;
    //vec4 colorGlowTop = vec4(topOpacityGlow, topOpacityGlow, topOpacityGlow, topOpacityGlow) * float(pixelInUpperBounds);
    //vec4 colorGlowBottom = vec4(bottomOpacityGlow, bottomOpacityGlow, bottomOpacityGlow, bottomOpacityGlow) * float(pixelInLowerBounds);
    vec4 colorGlowTop = vec4(calcColorGlow(primaryColor, secondaryColor, limitTop, intensity, distanceFromCenter), topOpacity) * float(pixelInUpperBounds);
    vec4 colorGlowBottom = vec4(calcColorGlow(primaryColor, secondaryColor, limitBottom, intensity, distanceFromCenter), bottomOpacity) * float(pixelInLowerBounds);
    //fragColor = vec4(grains, grains, grains, grains);
    //fragColor = (abs(colorGlowTop + colorGlowBottom) + vec4((primaryColor + secondaryColor) * min(1.0, grains) / 2.0, grains) * pixelWithinBounds);

	float epsilon = 0.01;
	vec4 finalColor = (abs(colorGlowTop + colorGlowBottom) * grains);
	bool alphaIsZero = finalColor[3] < epsilon;
	gl_FragDepth = depth + float(alphaIsZero && (finalColor[3] < epsilon));
    fragColor = finalColor + (0.00001 * float(colorTypes));
	//fragColor = vec4(1.0, 1.0, 1.0, 1.0);
	//fragColor = (real_texcoord[0], real_texcoord[1], 0.0, real_texcoord[1] + 0.5) + (abs(colorGlowTop + colorGlowBottom) * 0.8 * depth * float(colorTypes));
}
