#version 330 core

in vec3 aPos;
in vec2 aTexPositions;
in vec2 aCanvasQuadSizes;
in vec2 aCanvasPositions;
in vec2 aTexQuadSizes;
in float aAlphaStrength;
in float aDepth;

out vec2 texture_uv;
out vec2 texture_pos;
out vec2 texture_size;
out float alpha_strength;
out float depth;
void main(void)
{
	// Fix positions and sizes
	vec2 fixedSize = aCanvasQuadSizes * 2.0;
	vec2 fixedTexSize = aTexQuadSizes * 1.0;
	vec2 positionOffset = vec2(fixedSize[0], -fixedSize[1]) / 2.0;
	vec2 texPositionOffset = vec2(fixedTexSize[0], fixedTexSize[1]) / 2.0;
	vec2 fixedCanvPos = vec2((aCanvasPositions[0] * 2.0) - 1.0, (aCanvasPositions[1] * -2.0) + 1.0);
	vec2 fixedTexPos = vec2((aTexPositions[0] * 1.0), (aTexPositions[1] * 1.0));
	fixedCanvPos = fixedCanvPos + positionOffset;
	vec2 pos2d = vec2(aPos[0] * fixedSize[0], aPos[1] * fixedSize[1]) + fixedCanvPos;
	vec2 texPos2d = vec2(aPos[0] * fixedTexSize[0], -aPos[1] * fixedTexSize[1]) + fixedTexPos + texPositionOffset;
	
    gl_Position = vec4(pos2d, aDepth, 1.0);
    texture_uv = texPos2d;
	texture_pos = aTexPositions;
	texture_size = fixedTexSize;
	alpha_strength = aAlphaStrength;
}