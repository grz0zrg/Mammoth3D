#version 330

out vec4 outputColor;
in vec2 UV;

uniform float alpha;
uniform sampler2D t0;

void main()
{
	vec4 tex = texture(t0, UV);
	outputColor = vec4(tex.rgb, alpha);
}