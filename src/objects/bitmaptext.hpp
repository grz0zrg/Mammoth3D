#ifndef MAMMOTH3D_BITMAPTEXT_OBJECT_HPP
#define MAMMOTH3D_BITMAPTEXT_OBJECT_HPP
	
	#include "../font/bitmapfont.hpp"
	#include "../loaders/shaderloader.hpp"
	#include "mesh.hpp"

	namespace object {
		enum TextAlignment {
			LEFT,
			CENTER,
			RIGHT
		};
		
		class BitmapText : public Mesh {
			public:
				BitmapText(font::BitmapFont *font);
				
				~BitmapText();
				
				void setText(const std::string& text);
				
				void setFont(font::BitmapFont *font) {
					_font = font;
				}
				
				void setColor(int r, int g, int b) {
					_r = (float)r/255.0f;
					_g = (float)g/255.0f;
					_b = (float)b/255.0f;
					
					_uniforms->setUniform("r", _r);
					_uniforms->setUniform("g", _g);
					_uniforms->setUniform("b", _b);
					
					_uniforms->update();
				}
				
				void setAlphaTreshold(float value) {
					_alpha_treshold = value;
					_uniforms->setUniform("a", _alpha_treshold);
					
					_uniforms->update();
				}
				
				float _r, _g, _b, _alpha_treshold;
				
				font::BitmapFont *_font;
				
				std::string _text;
				
				core::Tbo *_tbo;
				
				program::Program *_prog;
				
				core::UniformBlock *_uniforms;
				
				core::Texture *_texture_buffer;
				
				TextAlignment _alignment;
		};
	}
	
#endif