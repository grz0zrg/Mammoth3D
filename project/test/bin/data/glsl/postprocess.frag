#version 330

out vec4 outputColor;
in vec2 UV;

in float fragmentAlpha;

uniform sampler2D myTextureSampler;

void main()
{
	outputColor = vec4(texture(myTextureSampler, UV/*+ 0.005*vec2( sin(UV.x+1024.0*UV.x),cos(UV.x+768.0*UV.y))*/).rgb, fragmentAlpha);
}