#version 410 core
uniform mat4 rotationMatrix;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

layout (location = 0) out vec3 ourColor;
layout (location = 1) out vec2 TexCoord;

void main()
{
    gl_Position = rotationMatrix * vec4(aPos, 1.0);
    ourColor = aColor;
    TexCoord = aTexCoord;
}