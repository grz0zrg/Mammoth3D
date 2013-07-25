#version 330

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUv;

uniform float uv_add;

uniform float alpha;
out float fragmentAlpha;

out vec2 UV;

void main(){
	fragmentAlpha = alpha;

	UV = vertexUv+vec2(uv_add/vertexPosition.y, uv_add*vertexPosition.x);
	gl_Position = vec4(vertexPosition, 1.0f);
}