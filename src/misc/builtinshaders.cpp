#include "builtinshaders.hpp"

const char builtinshaders::bitmapFontsVertexShader[] = {
			"#version 330\n"
			"layout(location = 0) in vec3 vertexPosition;\n"
			"layout(location = 1) in vec2 vertexUv;\n"
			"uniform mat4 mvp;\n"
			"uniform float alpha;\n"
			"out float fragmentAlpha;\n"
			"out vec2 uv;\n"
			"void main()\n"
			"{\n"
			"	uv = vertexUv;\n"
			"	fragmentAlpha = alpha;\n"
			"	gl_Position = /*mvp * */vec4(vertexPosition, 1.0f);\n"
			"}\n"
		};

const char builtinshaders::bitmapFontsFragmentShader[] = {
			"#version 330\n"
			"layout(location = 0) out vec4 outputColor;\n"
			"in float fragmentAlpha;\n"
			"in vec2 uv;\n"
			"layout (std140) uniform infosBlock {\n"
			"   vec4 color;\n"
			"	//float text_length;\n"
			"	float font_width;\n"
			"	float font_height;\n"
			"	float cell_width;\n"
			"	float cell_height;\n"
			"};\n"

			"uniform sampler2D t0;\n"
			"uniform samplerBuffer t1;\n"
			"void main()\n"
			"{\n"
			" float lim_y = (1.0f-cell_height/font_height);\n"
			//" float off_x = abs(0.5f-text_length)/2.0f;\n"
			//" float cell_height_d2 = (cell_height/2.0f)/font_width;\n"
			//" float off_y = 0.5f - cell_height_d2;\n" // y centering
			" int id = int((uv.x*font_width)/cell_width);\n"
			" vec4 pos = texelFetch(t1, id);\n"
			" float uvx = uv.x;//min(uv.x, text_length);\n"
			" vec4 tx = texture(t0, vec2(pos.x+(uvx-((id*cell_width)/font_width)), max(uv.y, lim_y)-pos.y));\n" //off_y+clamp(uv.y, off_y, 0.498f + cell_height_d2) // y centering
			//" tx.a = smoothstep(0.4375, 0.5625, tx.a);\n" // can smooth the edges if the font have a distance field as alpha channel
			" if(tx.a < color.a){\n"
			"     discard;\n"
			" }\n"
			" tx.a *= fragmentAlpha;\n"
			" tx.rgb *= color.rgb;\n"
			" outputColor = vec4(color.r,color.g,color.b,1.0);//tx;\n"
			"}\n"
		};