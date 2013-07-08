#version 330
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUv;
layout(location = 2) in vec4 vertexColor;

uniform mat4 mvp;
uniform float alpha;

smooth out vec4 fragmentColor;
out float fragmentAlpha;
out vec2 UV;

void main()
{
	UV = vertexUv;
	
	fragmentColor = vertexColor;
	fragmentAlpha = alpha;
	gl_Position = mvp * vec4(vertexPosition, 1.0f);
}