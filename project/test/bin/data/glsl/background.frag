#version 330

out vec4 outputColor;
in vec2 UV;

uniform float alpha;
uniform sampler2D myTextureSampler;

void main()
{
	vec4 tex = texture(myTextureSampler, UV);
	outputColor = vec4(tex.rgb, alpha);
}