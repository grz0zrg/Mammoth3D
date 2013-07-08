#version 330

out vec4 outputColor;
in vec2 UV;

void main()
{
	outputColor = vec4(sin(UV.x), sin(UV.x), sin(UV.y), 1.0f);
}