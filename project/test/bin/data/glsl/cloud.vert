#version 330
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUv;

uniform mat4 mvp;

out vec2 UV;

void main()
{
	UV = vertexUv;
	
	gl_Position = mvp * vec4(vertexPosition, 1.0f);
}