#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

float roundme(float input_float)
{
    float roundLevels = 5.0f;
    return float(round(input_float * roundLevels)) / roundLevels;
}

void main()
{
	vec2 onePixel = vec2(1.0, 1.0) / textureSize(ourTexture, 0);
    vec4 RealColor = texture(ourTexture, vec2(-TexCoord[0], TexCoord[1]));
	FragColor = RealColor;
	//FragColor = vec4(roundme(RealColor[0]), roundme(RealColor[1]), roundme(RealColor[2]), roundme(RealColor[3]));
}