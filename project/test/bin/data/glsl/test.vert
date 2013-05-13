#version 330
layout(location = 0) in vec4 position;
uniform vec4 offset;
uniform vec4 scale;

void main()
{
	gl_Position = position*scale+offset;
}