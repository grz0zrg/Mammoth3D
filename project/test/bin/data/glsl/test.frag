#version 330

out vec4 outputColor;

in vec3 fragmentColor;
in float fragmentAlpha;

void main()
{
    /*if(fragmentAlpha < 0.5f){
        discard;
    }*/
	
	outputColor = vec4(fragmentColor.r, fragmentColor.g, fragmentColor.b, fragmentAlpha);
}