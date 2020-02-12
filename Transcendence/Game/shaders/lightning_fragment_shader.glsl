#version 410 core

layout (location = 0) in vec2 quadPos;
layout (location = 1) flat in int reshape;
layout (location = 2) flat in int widthAdjType;
layout (location = 3) in float depth;
layout (location = 4) in vec3 primaryColor;
layout (location = 5) in vec3 secondaryColor;
layout (location = 6) in float seed;

uniform float current_tick;

out vec4 fragColor;

// Copy of EWidthAdjTypes enum from SFXRay.cpp
int widthAdjStraight = 0;
int widthAdjBlob = 1;
int widthAdjDiamond = 2;
int widthAdjJagged = 3;
int widthAdjOval = 4;
int widthAdjTaper = 5;
int widthAdjCone = 6;
int widthAdjWhiptail = 7;

// Copy of EOpacityTypes enum from SFXRay.cpp
int opacityGlow = 1;
int opacityGrainy = 2;
int opacityTaperedGlow = 3;

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
     float v = 0.0;
     float amp = 0.75;
     float freq = 1;
     int octaves = 2;
     for (int i = 0; i < octaves; i++)
         v += cnoise(vec3((p * (i * freq)), time)) * (amp / (i + 1));

     return v;
}

float rand(vec2 co){
  // Canonical PRNG from https://stackoverflow.com/questions/12964279/whats-the-origin-of-this-glsl-rand-one-liner
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec3 blendVectors(vec3 rgbFrom, vec3 rgbTo, float rFade)
{
    float rFadeVal = max(0.0, min(1.0, rFade));
    vec3 diff = rgbTo - rgbFrom;
    return (rgbFrom + (diff * rFadeVal));
}

float widthCone(float rInputValue)
{
    return (-(rInputValue - 1.0) / 2.0);
}

float widthDiamond(float rInputValue)
{
    return (1.0 - abs(rInputValue));
}

float widthTapered(float rInputValue)
{
    // Circular from head to peak
    float rQuadHalfLength = 1.0;
    float rTaperFraction = 8.0;
    float rPeakDistance = ((2.0 * rQuadHalfLength) / (rTaperFraction / 2.0));
    float rPeakPoint = rQuadHalfLength - rPeakDistance;
    float rCircleCoord = ((rPeakPoint - rInputValue) / rPeakDistance);
    bool inCircularPortion = rQuadHalfLength - rInputValue < rPeakDistance;
    float rCircleWidth = sqrt(rQuadHalfLength - (rCircleCoord * rCircleCoord)) * float(inCircularPortion);
    // Tapered from peak onwards
    float rTaperedDY = (2.0 * rQuadHalfLength) - rPeakDistance;
    float rTaperedWidth = ((rInputValue - (-rQuadHalfLength)) / rTaperedDY) * float(!inCircularPortion);
    float wtf = ((rInputValue - (-rQuadHalfLength)));
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

void main(void)
{
    float wavefront_amplitude = 6.0f;
    float uv_y_mult = 4.0;
    float uv_x_mult = 10;
    float ray_glow_thickness = 15.0;
    float blending_ratio = 4.1;
    vec2 uv = quadPos;
    uv.x *= uv_x_mult;
    uv.y *= uv_y_mult;
 
    float taperWidth = widthTapered(uv.x / uv_x_mult);
    float coneWidth = widthCone(uv.x / uv_x_mult);
    float diamondWidth = widthDiamond(uv.x / uv_x_mult);
    float ovalWidth = widthOval(uv.x / uv_x_mult);
    float straightWidth = 1.0;
 
    float taperAdj = (
        (straightWidth * float(reshape == widthAdjStraight)) + 
        (diamondWidth * float(reshape == widthAdjDiamond)) + 
        (coneWidth * float(reshape == widthAdjCone)) + 
        (taperWidth * float(reshape == widthAdjTaper)) + 
        (ovalWidth * float(reshape == widthAdjOval))
    );
    
    float widthAdj = (
        (straightWidth * float(widthAdjType == widthAdjStraight)) + 
        (diamondWidth * float(widthAdjType == widthAdjDiamond)) + 
        (coneWidth * float(widthAdjType == widthAdjCone)) + 
        (taperWidth * float(widthAdjType == widthAdjTaper)) + 
        (ovalWidth * float(widthAdjType == widthAdjOval))
    );
    
    float wavefrontAdj = taperAdj * widthAdj;
 
    float wavefront_pos = uv.y;
    float wavefront_multiplier = (rand(vec2(current_tick + abs(seed))) - 0.5) * wavefront_amplitude;
    wavefront_pos = uv.y + wavefront_multiplier * wavefrontAdj;

    float endXReduction = clamp((uv.x * -uv.x * 0.1) + uv_x_mult * uv_x_mult * 0.1, 0.0f, 1.0f);
    uv.y *= 0;
    uv.x *= 0.8;

    float noise = fbm((uv * 2) + vec2(100, 100), (current_tick + seed) * 5) * 2;
    float s1 = ((wavefront_pos) + (noise * endXReduction));
    float colorGrad = 1.0 / (s1 * (50.0));
	float alpha = abs(colorGrad);
	vec3 color = blendVectors(secondaryColor, primaryColor, (quadPos.x + 1) / 2);

	float epsilon = 0.01;
	bool alphaIsZero = alpha < epsilon;
	vec4 finalColor = vec4(color, alpha * float(!alphaIsZero));
	gl_FragDepth = depth + float(alphaIsZero && (finalColor[3] < epsilon));
    fragColor = finalColor;
}