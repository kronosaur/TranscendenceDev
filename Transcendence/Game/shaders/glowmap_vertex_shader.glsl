#version 410 core
uniform mat4 rotationMatrix;
uniform vec2 aTexStartPoint;
uniform vec2 aTexQuadSizes;

layout (location = 0) in vec3 aPos;

layout (location = 0) out vec2 TexCoord;
layout (location = 1) out vec2 gridSquareSize;

void main()
{
	// Read coords span from 0 to 1, POSITIVE!!!!
    vec2 fixedTexSize = vec2(aTexQuadSizes[0], aTexQuadSizes[1]);
    vec2 texPositionOffset = vec2(fixedTexSize[0], fixedTexSize[1]) / 1.0;
    vec2 fixedTexPos = vec2(aTexStartPoint[0], aTexStartPoint[1]);
	vec2 quarterPos = (vec2(aPos[0], aPos[1]) / 2) + vec2(0.5, 0.5);
	vec2 texPos2dRead = vec2(quarterPos[0] * fixedTexSize[0], -quarterPos[1] * fixedTexSize[1]) + fixedTexPos;

    // Write coords span from -1 to 1
	vec2 positionOffset = vec2(texPositionOffset[0], -texPositionOffset[1]) * 1.0f;
	vec2 fixedCanvPos = vec2((fixedTexPos[0] * 2.0) - 1.0, -((fixedTexPos[1] * 2.0) - 1.0));
	fixedCanvPos = fixedCanvPos + positionOffset;
	vec2 texPos2dWrite = vec2(aPos[0] * fixedTexSize[0] * 1.0, -aPos[1] * fixedTexSize[1] * 1.0) + fixedCanvPos;

    gl_Position = vec4(texPos2dWrite[0], texPos2dWrite[1], 0.1, 1.0);
	TexCoord = texPos2dRead;
	gridSquareSize = aTexQuadSizes;
    //TexCoord = aTexCoord;
}