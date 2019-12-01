#version 150 core

in vec3 aPos;
in vec3 aColor;
in vec2 aTexPositions;
in vec2 aQuadSizes;
in vec2 aCanvasPositions;

in vec3 in_Position;
in vec3 in_Color;
out vec3 pass_Color;
void main(void)
{
	vec2 pos2d = vec2(aPos[0] * aQuadSizes[0], aPos[1] * aQuadSizes[1]) + aCanvasPositions;
	
    gl_Position = vec4(pos2d, aPos[2], 1.0);
    pass_Color = aColor;
}