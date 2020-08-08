#version 410 core

// This shader implements the following effects: orb, ray, lightning.

layout (location = 0) in vec2 quadPos;
layout (location = 1) flat in int rayReshape;
layout (location = 2) flat in int rayWidthAdjType;
layout (location = 3) flat in int opacity;
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
layout (location = 19) in float orbRadius;
layout (location = 20) in float orbSecondaryOpacity;
layout (location = 21) flat in int orbLifetime;
layout (location = 22) flat in int orbCurrFrame;

uniform float current_tick;
uniform sampler3D perlin_noise;

out vec4 fragColor;

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

float sampleNoiseFBM(vec3 sampler) {
    return texture(perlin_noise, vec3(sampler[0], sampler[1], sampler[2]))[0];
}

float sampleNoisePerlin(vec3 sampler) {
    //return texture(perlin_noise, vec3(sampler[0], sampler[1], abs(mod(sampler[2], 2.0) - 1.0)))[1];
    return texture(perlin_noise, vec3(sampler[0], sampler[1], sampler[2]))[1];
}

float fbm(vec2 a, float b) {
    return (sampleNoiseFBM(vec3(a.x, a.y, b)) * 2.0) - 1.0;
}

vec4 calcRayColor(float taperAdjTop, float taperAdjBottom, float widthAdjTop, float widthAdjBottom, float center_point, vec2 real_texcoord, float intensity, float distanceFromCenter, float grains, float opacityAdj)
{
    float limitTop = taperAdjTop * widthAdjTop;
    float limitBottom = taperAdjBottom * widthAdjBottom;

    bool pixelInUpperBounds = ((real_texcoord[1] - center_point) < limitTop) && ((real_texcoord[1] - center_point) > 0);
    bool pixelInLowerBounds = ((real_texcoord[1] - center_point) > (-limitBottom)) && ((real_texcoord[1] - center_point) <= 0);
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
	return finalColor;
}

// BEGIN ORB SUBSHADER

void main(void)
{
    float center_point = 0.0; // Remember to remove this in the real shader!!
    vec2 real_texcoord = quadPos;

	vec4 finalColor = (
		calcRayLightningColor(quadSize, real_texcoord, rayWaveCyclePos, rayGrainyTexture, rayReshape, rayWidthAdjType, center_point, opacityAdj)
	);
	float epsilon = 0.01;
	bool alphaIsZero = finalColor[3] < epsilon;
	gl_FragDepth = depth + float(alphaIsZero && (finalColor[3] < epsilon));
    fragColor = finalColor;
}
