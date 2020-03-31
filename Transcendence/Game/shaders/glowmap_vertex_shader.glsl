#version 410 core
uniform mat4 rotationMatrix;
uniform vec2 aTexStartPoint;
uniform vec2 aTexQuadSizes;
uniform ivec2 aTexTotalSize;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

layout (location = 0) out vec3 ourColor; // TODO: Remove ourColor and aTexCoord, we don't use either
layout (location = 1) out vec2 TexCoord;
layout (location = 2) out vec2 quadSize;

void main()
{
    vec2 fixedTexSize = vec2(aTexQuadSizes[0] / float(aTexTotalSize[0]), aTexQuadSizes[1] / float(aTexTotalSize[1]));
    vec2 texPositionOffset = vec2(fixedTexSize[0], -fixedTexSize[1]) / 1.0;
    vec2 fixedTexPos = vec2((aTexStartPoint[0] / float(aTexTotalSize[0])), (aTexStartPoint[1] / float(aTexTotalSize[1])));
	vec2 texPos2dRead = vec2(aPos[0] * fixedTexSize[0], -aPos[1] * fixedTexSize[1]) + fixedTexPos + texPositionOffset;

	vec2 positionOffset = texPositionOffset * 2.0f;
	vec2 fixedCanvPos = vec2((fixedTexPos[0] * 2.0) - 1.0, (fixedTexPos[1] * 2.0) - 1.0);
	fixedCanvPos = fixedCanvPos + positionOffset;
	vec2 texPos2dWrite = vec2(aPos[0] * fixedTexSize[0] * 2.0, -aPos[1] * fixedTexSize[1] * 2.0) + fixedCanvPos;

    gl_Position = vec4(texPos2dWrite[0], texPos2dWrite[1], 0.1, 1.0);
	TexCoord = texPos2dRead;
    ourColor = aColor;
	quadSize = aTexQuadSizes;
    //TexCoord = aTexCoord;
}