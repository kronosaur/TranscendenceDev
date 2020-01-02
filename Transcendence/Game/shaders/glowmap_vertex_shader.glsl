#version 330 core
uniform mat4 rotationMatrix;

in vec3 aPos;
in vec3 aColor;
in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

void main()
{
    gl_Position = rotationMatrix * vec4(aPos, 1.0);
    ourColor = aColor;
    TexCoord = aTexCoord;
}