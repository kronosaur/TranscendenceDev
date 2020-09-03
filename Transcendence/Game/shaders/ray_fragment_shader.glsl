#version 410 core

// This shader implements the following effects: orb, ray, lightning.

layout (location = 0) in vec2 quadPos;
layout (location = 1) flat in int rayReshape;
layout (location = 2) flat in int rayWidthAdjType;
layout (location = 3) flat in int rayOpacity;
layout (location = 4) flat in int rayGrainyTexture;
layout (location = 5) in float depth;
layout (location = 6) in float intensity;
layout (location = 7) in vec3 primaryColor;
layout (location = 8) in vec3 secondaryColor;
layout (location = 9) in float rayWaveCyclePos;
layout (location = 10) flat in int colorTypes; // TODO: replace with something more useful
layout (location = 11) in float opacityAdj;
layout (location = 12) flat in int effectType;
layout (location = 13) in float seed;
layout (location = 14) in vec2 quadSize;
layout (location = 15) flat in int orbAnimation; //
layout (location = 16) flat in int orbStyle; //
layout (location = 17) flat in int orbDistortion;
layout (location = 18) flat in int orbDetail;
layout (location = 19) in float orbSecondaryOpacity;
layout (location = 20) flat in int orbLifetime;
layout (location = 21) flat in int orbCurrFrame;
layout (location = 22) flat in int blendMode;

uniform float current_tick;
uniform sampler3D perlin_noise;

out vec4 fragColor;

// This should match enum blendMode in opengl.h.

int blendNormal = 0;      //      Normal drawing
int blendMultiply = 1;      //      Darkens images
int blendOverlay = 2;      //      Combine multiply/screen
int blendScreen = 3;      //      Brightens images
int blendHardLight = 4;
int blendCompositeNormal = 5;

// This should match enum effectType in opengl.h.

int effectTypeRay = 0;
int effectTypeLightning = 1;
int effectTypeOrb = 2;
int effectTypeFlare = 3;
int effectTypeParticle = 4;

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

// Copy of EOpacityTypes enum from SFXRay.cpp (except for opacityFlareGlow)
int opacityGlow = 1;
int opacityGrainy = 2;
int opacityTaperedGlow = 3;
int opacityTaperedExponentialGlow = 4;
int opacityFlareGlow = -1;


//  Classic Perlin 3D Noise 
//  by Stefan Gustavson
//
//  Found at https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83
vec4 permute(vec4 x){return mod(((x*34.0)+1.0)*x, 289.0);}
vec4 taylorInvSqrt(vec4 r){return 1.79284291400159 - 0.85373472095314 * r;}
vec3 fade(vec3 t) {return t*t*t*(t*(t*6.0-15.0)+10.0);}

float rand(vec2 co){
  // Canonical PRNG from https://stackoverflow.com/questions/12964279/whats-the-origin-of-this-glsl-rand-one-liner
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

// BEGIN RAY AND LIGHTNING SUBSHADER

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

float calcOpacityFlareGlow(float distanceFromCenter_w, float coordinate_l)
{
    const float BLOOM_FACTOR = 1.2;
    float rDist = (1.0 - abs(distanceFromCenter_w));
    float rSpread = (1.0 - abs(coordinate_l));
    float rValue = BLOOM_FACTOR * (rDist * rDist) * (rSpread * rSpread);
    float byOpacity = min(1.0, rValue);
    return (byOpacity);
}

float sampleNoiseFBM(vec3 sampler) {
    return texture(perlin_noise, vec3(sampler[0], sampler[1], sampler[2]))[0];
}

float sampleNoiseFBMAnimated(vec3 sampler) {
    return texture(perlin_noise, vec3(sampler[0], sampler[1], sampler[2]))[2];
}

float sampleNoisePerlin(vec3 sampler) {
    //return texture(perlin_noise, vec3(sampler[0], sampler[1], abs(mod(sampler[2], 2.0) - 1.0)))[1];
    return texture(perlin_noise, vec3(sampler[0], sampler[1], sampler[2]))[1];
}

float fbm(vec2 a, float b) {
    return (sampleNoiseFBM(vec3(a.x, a.y, b)) * 2.0) - 1.0;
}

float fbmAnimated(vec2 a, float b) {
    return (sampleNoiseFBMAnimated(vec3(a.x, a.y, b)) * 2.0) - 1.0;
}

vec4 calcRayColor(float taperAdjTop, float taperAdjBottom, float widthAdjTop, float widthAdjBottom, float center_point, vec2 real_texcoord, float intensity, float distanceFromCenter, float grains, float opacityAdj)
{
    float limitTop = taperAdjTop * widthAdjTop;
    float limitBottom = taperAdjBottom * widthAdjBottom;

    // Note we don't use distanceFromCenter in pixelInUpper/LowerBounds since distanceFromCenter is abs, but we can't use abs there
    bool pixelInUpperBounds = ((real_texcoord[1] - center_point) < limitTop) && ((real_texcoord[1] - center_point) > 0);
    bool pixelInLowerBounds = ((real_texcoord[1] - center_point) > (-limitBottom)) && ((real_texcoord[1] - center_point) <= 0);
    float pixelWithinBounds = float(pixelInUpperBounds || pixelInLowerBounds);
    float topOpacityGlow = calcOpacityGlow(limitTop, intensity, distanceFromCenter);
    float bottomOpacityGlow = calcOpacityGlow(limitBottom, intensity, distanceFromCenter);
    float topOpacityTaperedGlow = calcOpacityTaperedGlow(limitTop, intensity, distanceFromCenter, real_texcoord[0]);
    float bottomOpacityTaperedGlow = calcOpacityTaperedGlow(limitBottom, intensity, distanceFromCenter, real_texcoord[0]);
    float topOpacityTaperedExponentialGlow = calcOpacityTaperedExponentialGlow(limitTop, intensity, distanceFromCenter, real_texcoord[0]);
    float bottomOpacityTaperedExponentialGlow = calcOpacityTaperedExponentialGlow(limitBottom, intensity, distanceFromCenter, real_texcoord[0]);
    float topOpacityFlareGlow = calcOpacityFlareGlow(distanceFromCenter, real_texcoord[0]);
    float bottomOpacityFlareGlow = calcOpacityFlareGlow(distanceFromCenter, real_texcoord[0]);

    float topOpacity = (
        (topOpacityGlow * float(rayOpacity == opacityGlow)) +
        (topOpacityTaperedGlow * float(rayOpacity == opacityTaperedGlow)) +
        (topOpacityTaperedExponentialGlow * float(rayOpacity == opacityTaperedExponentialGlow)) +
        (topOpacityFlareGlow * float(rayOpacity == opacityFlareGlow))
    ) * opacityAdj;
    float bottomOpacity = (
        (bottomOpacityGlow * float(rayOpacity == opacityGlow)) +
        (bottomOpacityTaperedGlow * float(rayOpacity == opacityTaperedGlow)) +
        (bottomOpacityTaperedExponentialGlow * float(rayOpacity == opacityTaperedExponentialGlow)) +
        (bottomOpacityFlareGlow * float(rayOpacity == opacityFlareGlow))
    ) * opacityAdj;
    vec4 colorGlowTop = vec4(calcColorGlow(primaryColor, secondaryColor, limitTop, intensity, distanceFromCenter), topOpacity) * float(pixelInUpperBounds);
    vec4 colorGlowBottom = vec4(calcColorGlow(primaryColor, secondaryColor, limitBottom, intensity, distanceFromCenter), bottomOpacity) * float(pixelInLowerBounds);
	vec4 finalColor = (abs(colorGlowTop + colorGlowBottom) * grains);
	return finalColor;
}

vec4 calcLightningColor(float taperAdj, float widthAdj, vec2 real_texcoord)
{
	float wavefront_amplitude = 6.0f;
	float uv_x_lightning_mult = 10;
    float uv_y_mult = 4.0;
	vec2 uv = real_texcoord;
	uv.x *= uv_x_lightning_mult;
    uv.y *= uv_y_mult;
	float fbmscale = 1.0;

    float wavefrontAdj = taperAdj * widthAdj;

    float wavefront_pos = uv.y;
    float wavefront_multiplier = (rand(vec2(current_tick + abs(seed))) - 0.5) * wavefront_amplitude;
    wavefront_pos = uv.y + wavefront_multiplier * wavefrontAdj;

    float endXReduction = clamp((uv.x * -uv.x * 0.1) + uv_x_lightning_mult * uv_x_lightning_mult * 0.1, 0.0f, 1.0f);
    uv.y *= 0;
    uv.x *= 0.8;

    float noise = fbm((uv * 2 * fbmscale), (current_tick + seed) * (1.0 / 30.0) * fbmscale) * 2;
    float s1 = ((wavefront_pos) + (noise * endXReduction));
    float colorGrad = 1.0 / (s1 * (50.0));
	float alpha = abs(colorGrad * colorGrad);
	float quadPosAlphaAdj = (real_texcoord.x + 1) / 2;
	vec3 color = blendVectors(secondaryColor, primaryColor, quadPosAlphaAdj);

	float epsilon = 0.01;
	bool alphaIsZero = alpha < epsilon;
	vec4 finalColor = vec4(color, pow(alpha, 0.1) * quadPosAlphaAdj * float(!alphaIsZero));
	return finalColor;
}

vec4 calcRayLightningColor(vec2 quadSize, vec2 real_texcoord, float waveCyclePos, int grainyTexture, int reshape, int widthAdjType, float ray_center_x, float opacityAdj) {
    float distanceFromCenter = abs(real_texcoord[1] - ray_center_x);

	// Graininess
    float grains_x = quadSize[0] / 20.0f;
    float grains_y = quadSize[1] / 20.0f;
    float perlinNoise = sampleNoisePerlin(vec3(quadPos[0] * grains_x * 0.1, quadPos[1] * grains_y * 0.1, current_tick * 100));
    float grains = (perlinNoise * float(grainyTexture == opacityGrainy) * 2) + float(grainyTexture == 0);

	float widthCalcPos = real_texcoord[0];

    float blobWidthTop = widthRandomWaves(widthCalcPos, BLOB_WAVE_SIZE, WAVY_WAVELENGTH_FACTOR * (quadSize[1] / quadSize[0]), 1.1);
    float blobWidthBottom = blobWidthTop;
    float jaggedWidthTop = widthRandomWaves(widthCalcPos, JAGGED_AMPLITUDE, JAGGED_WAVELENGTH_FACTOR * (quadSize[1] / quadSize[0]), 3.3);
    float jaggedWidthBottom = jaggedWidthTop;
    float whiptailWidthTop = widthWave(widthCalcPos, WHIPTAIL_AMPLITUDE, WHIPTAIL_WAVELENGTH_FACTOR * (quadSize[1] / quadSize[0]), WHIPTAIL_DECAY, waveCyclePos);
    float whiptailWidthBottom = widthWave(widthCalcPos, WHIPTAIL_AMPLITUDE, WHIPTAIL_WAVELENGTH_FACTOR * (quadSize[1] / quadSize[0]), WHIPTAIL_DECAY, waveCyclePos + 0.5);
    float taperWidth = widthTapered(widthCalcPos);
    float coneWidth = widthCone(widthCalcPos);
    float diamondWidth = widthDiamond(widthCalcPos);
    float ovalWidth = widthOval(widthCalcPos);
    float swordWidth = widthSword(widthCalcPos);
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
    
	vec4 rayColor = calcRayColor(taperAdjTop, taperAdjBottom, widthAdjTop, widthAdjBottom, ray_center_x, real_texcoord, intensity, distanceFromCenter, grains, opacityAdj);
	vec4 lightningColor = calcLightningColor(taperAdjTop, widthAdjTop, real_texcoord);


	vec4 finalColor = (
		rayColor * float(effectType == effectTypeRay) +
		lightningColor * float(effectType == effectTypeLightning)
	);
	finalColor[3] = max(finalColor[3], 0.0); // if finalColor[3] is nan, set it to zero
	return finalColor;
}

// BEGIN ORB SUBSHADER
float pixelsDistanceFromCenter = length(quadPos) * (quadSize[0] / 2);

float SHELL_EDGE_WIDTH_RATIO = 0.05;
float orb_fbm_time_divisor = 160.0;
float orb_fbm_space_divisor = 55.0;

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
    bool useFadeRadius = (pixelsDistanceFromCenter < (fFringeMaxRadius + fFadeWidth) && (pixelsDistanceFromCenter >= fFringeMaxRadius || fFringeWidth <= 0.0) && fFadeWidth > 0.0);
    float fadeOpacityStep = (pixelsDistanceFromCenter - fFringeMaxRadius);
    float fadeOpacity = 1.0 - (fadeOpacityStep / fFadeWidth);
    fadeOpacity = (fadeOpacity * fadeOpacity) * fOpacity;
    vec4 fadeRadiusColor = float(useFadeRadius) * vec4(vSecondaryColor, fadeOpacity);
    
    // Black otherwise
    bool useBlackRadius = (pixelsDistanceFromCenter > fRadius) || (pixelsDistanceFromCenter >= (fFringeMaxRadius + fFadeWidth));
    vec4 blackRadiusColor = float(useBlackRadius) * vec4(0.0);
    
    return (blownRadiusColor + fringeRadiusColor + fadeRadiusColor + blackRadiusColor);
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
    float fFade = FloatIncrementor(1.0, 2.0, fEdgeWidth + 1.0, 0.0, (1.0 + pixelsDistanceFromCenter) - fShellMaxRadius);

    bool useInnerOpacity = pixelsDistanceFromCenter < fShellMaxRadius;
    float innerOpacity = fOpacity * ((fMaxIntensity * fMaxIntensityK) + (fMinIntensity * fMinIntensityK)) * float(useInnerOpacity);
    bool useOuterOpacity = (pixelsDistanceFromCenter >= fShellMaxRadius) && (pixelsDistanceFromCenter < fRadius);
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
    float angle_phase_adj = seed * 3;
    float rand_noise_seed = seed * 1;
    //float fAngle = (dot(quadPos, vec2(0.0, 1.0)) / length(quadPos));
    float fAngle = abs(mod(atan(quadPos.y, quadPos.x) + angle_phase_adj, (3.14159 * 2)) - 3.14159);//(dot(quadPos, vec2(0.0, 1.0)) / length(quadPos));
    float fRand = ((sampleNoisePerlin(vec3(fAngle * ANGLE_DISTORTION_MULTIPLIER + rand_noise_seed)) / 2.0) + 0.5);
    float fRadAdjFactor = (1 + (fRand * float(orbDistortion) / 100.0));
    vec2 fAdjPos = quadPos * fRadAdjFactor;
    float fAdjRadius = pixelsDistanceFromCenter * fRadAdjFactor;
    //float fAdjRadius = pixelsDistanceFromCenter;
    bool useBlack = (fAdjRadius >= fRadius);
    
    float fNoiseValue = fbmAnimated((fAdjPos + 1.0) / orb_fbm_space_divisor, current_tick / orb_fbm_time_divisor) + 0.5;

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
        (cloudColor * float(orbStyle == styleCloud)) +
        (firecloudColor * float(orbStyle == styleFirecloud)) +
        (fireCloudshellColor * float(orbStyle == styleFireCloudshell)) +
        (smokeColor * float(orbStyle == styleSmoke)) +
        (smoothColor * float(orbStyle == styleSmooth)) +
        (fireblastColor * float(orbStyle == styleFireblast)) +
        (blackHoleColor * float(orbStyle == styleBlackHole)) +
        (shellColor * float(orbStyle == styleShell)) +
        (flareColor * float(orbStyle == styleFlare)) +
        (diffractionColor * float(orbStyle == styleDiffraction)) +
        (cloudShellColor * float(orbStyle == styleCloudshell)) +
        (flareColor * float(orbStyle == styleLightning))
    );
    return (finalColor);
}

float calcAnimateExplodeSecondaryOpacity(float fRadius, float fOpacity, float fFrame) {
    float fEndFade = float(orbLifetime) / 3.0;
    float fEndFadeStart = float(orbLifetime) - fEndFade;
    
    float useEndFadeStart = float(fFrame > fEndFadeStart);
    float fFadeOpacity = useEndFadeStart * (fEndFade - (fFrame - fEndFadeStart)) / fEndFade;
    
    float useOpacity = float(fFrame <= fEndFadeStart);
    
    return (fFadeOpacity * useEndFadeStart) + (fOpacity * useOpacity);
    
}

vec4 calcAnimationColor(float animatedNoise, float scaledNoise, float orbRadius) {
    // Obtain the following attributes for the sphere color calculation:
    // m_iStyle, m_iRadius, m_iIntensity, m_rgbPrimaryColor, m_rgbSecondaryColor, m_byOpacity.
    // Note that orbStyle, secondaryColor are not changed by animation orbStyle.
        //CStepIncrementor Opacity(CStepIncrementor::styleSquare, byOpacity, 0.0, iRadius);
        //FloatIncrementor(fOpacity, 2.0, fRadius, 0.0, pixelsDistanceFromCenter);
    float fLifetime = float(orbLifetime);
    float fCurrframe = float(orbCurrFrame);
    float animateDissipateRadius = FloatIncrementor(0.2 * orbRadius, 0.5, fLifetime, orbRadius, fCurrframe);
    float animateDissipateIntensity = FloatIncrementor(intensity, 1.0, fLifetime, 0.0, fCurrframe);
    float animateDissipateDetail = FloatIncrementor(float(orbDetail) / 100.0f, 1.0, fLifetime, float(orbDetail) / 1000.0f, fCurrframe);
    float animateDissipateColorFade = FloatIncrementor(0.0, 1.0, fLifetime, 1.0, fCurrframe);

    vec3 animateDissipatePrimaryColor = mix(primaryColor, secondaryColor, animateDissipateColorFade);
    float animateDissipateNoise = scaledNoise;
    float animateDissipateOpacity = opacityAdj;
    float animateDissipateSecondaryOpacity = orbSecondaryOpacity;

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
    float animateFadeSecondaryOpacity = orbSecondaryOpacity;
    
	// Note that for flicker we just use the orb radius and intensity we're given. This is because those variables are
	// supplied on the CPU end, so we can sync the flickering and flare effects.
    float animateFlickerRadius = orbRadius;
    float animateFlickerIntensity = intensity;
    animateFlickerIntensity = (float(orbStyle == styleFireblast) * (min(max(animateFlickerIntensity, 0.0), 100.0))) + (float(orbStyle != styleFireblast) * animateFlickerIntensity);
    vec3 animateFlickerPrimaryColor = primaryColor;
    float animateFlickerDetail = orbDetail;
    float animateFlickerNoise = scaledNoise;
    float animateFlickerOpacity = opacityAdj;
    float animateFlickerSecondaryOpacity = orbSecondaryOpacity;
    
    float animateDimRadius = orbRadius;
    float animateDimIntensity = intensity;
    vec3 animateDimPrimaryColor = mix(primaryColor, secondaryColor, FloatIncrementor(0.0, 1.0, fLifetime, 1.0, fCurrframe));
    float animateDimDetail = orbDetail;
    float animateDimNoise = scaledNoise;
    float animateDimOpacity = FloatIncrementor(opacityAdj, 1.0, fLifetime, orbSecondaryOpacity, fCurrframe);
    float animateDimSecondaryOpacity = orbSecondaryOpacity;
    
    float animateNoneRadius = orbRadius;
    float animateNoneIntensity = intensity;
    vec3 animateNonePrimaryColor = primaryColor;
    float animateNoneDetail = orbDetail;
    float animateNoneNoise = scaledNoise;
    float animateNoneOpacity = opacityAdj;
    float animateNoneSecondaryOpacity = orbSecondaryOpacity;
    
    float finalRadius = (
        (animateNoneRadius * float(orbAnimation == animateNone)) +
        (animateDissipateRadius * float(orbAnimation == animateDissipate)) +
        (animateExplodeRadius * float(orbAnimation == animateExplode)) +
        (animateFadeRadius * float(orbAnimation == animateFade)) +
        (animateFlickerRadius * float(orbAnimation == animateFlicker)) +
        (animateDimRadius * float(orbAnimation == animateDim))
    );
    
    float finalIntensity = (
        (animateNoneIntensity * float(orbAnimation == animateNone)) +
        (animateDissipateIntensity * float(orbAnimation == animateDissipate)) +
        (animateExplodeIntensity * float(orbAnimation == animateExplode)) +
        (animateFadeIntensity * float(orbAnimation == animateFade)) +
        (animateFlickerIntensity * float(orbAnimation == animateFlicker)) +
        (animateDimIntensity * float(orbAnimation == animateDim))
    );
    
    float finalDetail = (
        (animateNoneDetail * float(orbAnimation == animateNone)) +
        (animateDissipateDetail * float(orbAnimation == animateDissipate)) +
        (animateExplodeDetail * float(orbAnimation == animateExplode)) +
        (animateFadeDetail * float(orbAnimation == animateFade)) +
        (animateFlickerDetail * float(orbAnimation == animateFlicker)) +
        (animateDimDetail * float(orbAnimation == animateDim))
    );
    
    vec3 finalPrimaryColor = (
        (animateNonePrimaryColor * float(orbAnimation == animateNone)) +
        (animateDissipatePrimaryColor * float(orbAnimation == animateDissipate)) +
        (animateExplodePrimaryColor * float(orbAnimation == animateExplode)) +
        (animateFadePrimaryColor * float(orbAnimation == animateFade)) +
        (animateFlickerPrimaryColor * float(orbAnimation == animateFlicker)) +
        (animateDimPrimaryColor * float(orbAnimation == animateDim))
    );
    
    float finalNoise = (
        (animateNoneNoise * float(orbAnimation == animateNone)) +
        (animateDissipateNoise * float(orbAnimation == animateDissipate)) +
        (animateExplodeNoise * float(orbAnimation == animateExplode)) +
        (animateFadeNoise * float(orbAnimation == animateFade)) +
        (animateFlickerNoise * float(orbAnimation == animateFlicker)) +
        (animateDimNoise * float(orbAnimation == animateDim))
    );
    
    float finalOpacity = (
        (animateNoneOpacity * float(orbAnimation == animateNone)) +
        (animateDissipateOpacity * float(orbAnimation == animateDissipate)) +
        (animateExplodeOpacity * float(orbAnimation == animateExplode)) +
        (animateFadeOpacity * float(orbAnimation == animateFade)) +
        (animateFlickerOpacity * float(orbAnimation == animateFlicker)) +
        (animateDimOpacity * float(orbAnimation == animateDim))
    );
    
    float finalSecondaryOpacity = (
        (animateNoneSecondaryOpacity * float(orbAnimation == animateNone)) +
        (animateDissipateSecondaryOpacity * float(orbAnimation == animateDissipate)) +
        (animateExplodeSecondaryOpacity * float(orbAnimation == animateExplode)) +
        (animateFadeSecondaryOpacity * float(orbAnimation == animateFade)) +
        (animateFlickerSecondaryOpacity * float(orbAnimation == animateFlicker)) +
        (animateDimSecondaryOpacity * float(orbAnimation == animateDim))
    );
    
//    return vec4(0.5);
    // We only use secondary color table for fireblast.
    return calcPrimaryColor(orbStyle, finalRadius, finalIntensity, finalPrimaryColor, secondaryColor, finalOpacity, scaledNoise);
}

vec4 calcOrbColor (vec2 quadSize) {
    float orbRadius = ((quadSize[0] + quadSize[1]) / 2.0) / 2.0;
    float animatedNoise = fbmAnimated(((quadPos + 1.0) / orb_fbm_space_divisor), current_tick / orb_fbm_time_divisor) + 0.5;
    float scaledNoise = fbmAnimated((quadPos + 1.0) / (orbRadius / 50), current_tick / orb_fbm_time_divisor) + 0.5;
    vec4 finalColor = calcAnimationColor(animatedNoise, scaledNoise, orbRadius);
	finalColor[0] = max(finalColor[0], 0.0); // if finalColor[3] is nan, set it to zero
	finalColor[1] = max(finalColor[1], 0.0); // if finalColor[3] is nan, set it to zero
	finalColor[2] = max(finalColor[2], 0.0); // if finalColor[3] is nan, set it to zero
	finalColor[3] = max(finalColor[3], 0.0); // if finalColor[3] is nan, set it to zero
    return finalColor;
}

void main(void)
{
    float center_point = 0.0; // Remember to remove this in the real shader!!
    vec2 real_texcoord = quadPos;

	vec4 finalColor = (
		(calcRayLightningColor(quadSize, real_texcoord, rayWaveCyclePos, rayGrainyTexture, rayReshape, rayWidthAdjType, center_point, opacityAdj) * float((effectType == effectTypeRay) || (effectType == effectTypeLightning))) +
		(calcOrbColor(quadSize) * float(effectType == effectTypeOrb))
	);

	bool usePreMultipliedAlpha = (
		(blendMode == blendScreen)
	);

	vec3 finalColorRGB = (vec3(finalColor[0], finalColor[1], finalColor[2]) * float(!usePreMultipliedAlpha)) + (vec3(finalColor[0], finalColor[1], finalColor[2]) * float(usePreMultipliedAlpha) * finalColor[3]);

	float epsilon = 0.01;
	bool alphaIsZero = finalColor[3] < epsilon;
	gl_FragDepth = depth + float(alphaIsZero && (finalColor[3] < epsilon));
    fragColor = vec4(finalColorRGB[0], finalColorRGB[1], finalColorRGB[2], finalColor[3]);
}
