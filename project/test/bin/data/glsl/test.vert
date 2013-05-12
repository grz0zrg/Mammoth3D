#version 330
layout(location = 0) in vec4 position;

void main()
{
	vec4 totalOffset = vec4(0.0, 0.0, 0.0, 1.0);
	gl_Position = position+totalOffset;
}