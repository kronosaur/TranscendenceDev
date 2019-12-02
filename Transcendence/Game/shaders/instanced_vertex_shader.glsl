#version 330 core

in vec3 aPos;
in vec3 aColor;
in vec2 aTexPositions;
in vec2 aQuadSizes;
in vec2 aCanvasPositions;

out vec3 pass_Color;
void main(void)
{
	// Fix positions and sizes
	vec2 fixedSize = aQuadSizes * 2.0;
	vec2 positionOffset = vec2(fixedSize[0], -fixedSize[1]) / 2.0;
	vec2 fixedCanvPos = vec2((aCanvasPositions[0] * 2.0) - 1.0, (aCanvasPositions[1] * -2.0) + 1.0);
	fixedCanvPos = fixedCanvPos + positionOffset;
	vec2 pos2d = vec2(aPos[0] * fixedSize[0], aPos[1] * fixedSize[1]) + fixedCanvPos;
	
    gl_Position = vec4(pos2d, aPos[2], 1.0);
    pass_Color = vec3(aTexPositions, 1.0) + aColor;
}