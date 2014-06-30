#version 330

out vec4 outputColor;
in vec2 UV;

uniform float alpha;
//uniform sampler2D t0;
uniform sampler2DMS t0;

layout (std140) uniform Params {
	float aperture;
    //float uv_multiplier;
	//float time;
};

vec2 barrelDistortion(vec2 coord, float amt) {
	vec2 cc = coord - 0.5f;
	float dist = dot(cc, cc);
	return coord + cc * dist * amt;
}

float sat( float t )
{
	return clamp( t, 0.0, 1.0 );
}

float linterp( float t ) {
	return sat( 1.0 - abs( 2.0*t - 1.0 ) );
}

float remap( float t, float a, float b ) {
	return sat( (t - a) / (b - a) );
}

vec3 spectrum_offset( float t ) {
	vec3 ret;
	float lo = step(t,0.5);
	float hi = 1.0-lo;
	float w = linterp( remap( t, 1.0/6.0, 5.0/6.0 ) );
	ret = vec3(lo,1.0,hi) * vec3(1.0-w, w, 1.0-w);

	return pow( ret, vec3(1.0/2.2) );
}

const float max_distort = 1.4;
const int num_iter = 16;
const float reci_num_iter_f = 1.0 / float(num_iter);

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{

	//ivec2 iUV = ivec2(textureSize(t0) * UV);
	//vec4 tex = texture(t0, UV);
	
	//vec4 tex =vec4(0.0,0.0,0.0,0.0);
	//tex += texture2D(t0, vec2(UV.y-1-sqrt(abs(0.5*UV.x-1-0.5))*abs(UV.x-1), 1.25-UV.x-1));
	//tex += texelFetch(t0, ivec2(textureSize(t0) * vec2(UV.y-1-sqrt(abs(0.5*UV.x-1-0.5))*abs(UV.x-1), 1.25-UV.x-1))*uv_multiplier*time,0);
	//vec4 tex = texelFetch(t0, ivec2(textureSize(t0) * UV),0);
	//tex += texelFetch(t0, ivec2(textureSize(t0) * UV),1);
	//tex /= 2;

	
	vec4 tex = vec4(0);
	vec3 sumcol = vec3(0.0);
	vec3 sumw = vec3(0.0);	
	for ( int i=0; i<num_iter;++i )
	{
		float t = float(i) * reci_num_iter_f;
		vec3 w = spectrum_offset( t );
		sumw += w;
		//sumcol += w * texture2D( t0, barrelDistortion((UV*0.5)+0.25, max_distort*t ) ).rgb;

		ivec2 UVi = ivec2(textureSize(t0) * barrelDistortion((UV*0.5)+0.25, max_distort*t ));
		
		vec3 accum = vec3(0.0);
		for(int sample = 0; sample < 4; ++sample) {
			accum += texelFetch(t0, UVi, sample).rgb;
		}
		accum /= 4;

		sumcol += w * accum;
	}

	tex.rgb = sumcol/sumw;
	
	float dist = distance(UV, vec2(0.5, 0.5)) * aperture;
	tex.rgb *= smoothstep(0.755, 0.755-0.5, dist);
	
	float grain = rand(UV);
	tex.rgb *= max(0.1095+grain, 1.0);
 
	outputColor = vec4(tex.rgb, alpha);
}