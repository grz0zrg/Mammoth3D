#version 330

layout(location = 0) out vec3 outputColor;

//out vec4 outputColor;

smooth in vec4 fragmentColor;
in float fragmentAlpha;
in vec2 UV;

uniform sampler2D myTextureSampler;

void main()
{
    /*if(fragmentAlpha < 0.5f){
        discard;
    }*/
	outputColor = /*texture(myTextureSampler, UV) **/vec4(fragmentColor.r, fragmentColor.g, fragmentColor.b, fragmentAlpha);
}