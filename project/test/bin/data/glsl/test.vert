#version 330
layout(location = 0) in vec4 vertexPosition;
layout(location = 1) in vec3 vertexColor;

uniform mat4 mvp;
uniform float alpha;

out vec3 fragmentColor;
out float fragmentAlpha;

void main()
{
	fragmentColor = vertexColor;
	fragmentAlpha = alpha;
	gl_Position = mvp * vertexPosition;
}