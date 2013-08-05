#version 330

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUv;
/*
layout (std140) uniform fxStuff {
    float uv_a;
};
*/

out vec2 UV;

void main(){
	UV = vertexUv;//vertexPosition.xy;
	gl_Position = vec4(vertexPosition, 1.0f);
}