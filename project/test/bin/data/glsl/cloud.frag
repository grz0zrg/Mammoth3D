#version 330

layout(location = 0) out vec4 outputColor;

in vec2 UV;

uniform float alpha;
uniform sampler2D myTextureSampler;

void main()
{
	float depth = gl_FragCoord.z / gl_FragCoord.w;
	float fogFactor = smoothstep(0.0f, 256.0f, depth);
	
	vec4 tex = texture(myTextureSampler, UV);
	tex.a *= pow( gl_FragCoord.z, 20.0f ) * alpha;
	
	outputColor = mix(tex, vec4(vec3(0.192f, 0.478f, 0.670f), tex.a), fogFactor);
}