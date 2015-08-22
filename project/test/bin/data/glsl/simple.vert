#version 330
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUv;
layout(location = 3) in vec4 icolor;
layout(location = 4) in mat4 imvp;

uniform mat4 mvp;

out vec2 UV;
out vec4 COLOR;

void main()
{
	UV = vertexUv;
	COLOR = icolor;

	gl_Position = imvp * vec4(vertexPosition, 1.0);
}
