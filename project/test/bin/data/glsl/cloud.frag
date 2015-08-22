#version 330

layout(location = 0) out vec4 outputColor;

in vec2 UV;

uniform float alpha;
uniform sampler2D t0;

void main()
{
	float depth = gl_FragCoord.z / gl_FragCoord.w;
	float fogFactor = smoothstep(0.0, 1024.0, depth);
	
	vec4 tex = texture(t0, UV);
	tex.a *= pow( gl_FragCoord.z, 20.0 ) * alpha;
	
	outputColor = mix(tex, vec4(vec3(0.192, 0.478, 0.670), tex.a), fogFactor);
}