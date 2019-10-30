#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

float roundme(float input_float)
{
    float roundLevels = 10.0f;
    return float(round(input_float * roundLevels)) / roundLevels;
}

void main()
{
    vec4 RealColor = texture(ourTexture, vec2(-TexCoord[0], TexCoord[1]));
	FragColor = RealColor;
	//FragColor = vec4(roundme(RealColor[0]), roundme(RealColor[1]), roundme(RealColor[2]), roundme(RealColor[3]));
}