#version 330 core
in vec2 texture_uv;
in vec2 texture_pos;
out vec4 out_Color;

uniform sampler2D obj_texture;

void main(void)
{
	vec2 onePixel = vec2(1.0, 1.0) / textureSize(obj_texture, 0);
	vec2(-texture_uv[0], texture_uv[1]);
	vec4 RealColor = texture(obj_texture, vec2(-texture_uv[0], texture_uv[1]));
    out_Color = RealColor;
}