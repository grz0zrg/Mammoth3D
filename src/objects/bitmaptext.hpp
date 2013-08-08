#ifndef BITMAPTEXT_OBJECT_HPP
#define BITMAPTEXT_OBJECT_HPP
	
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
					this->font = font;
				}
				
				void setColor(int r, int g, int b) {
					this->r = (float)r/255.0f;
					this->g = (float)g/255.0f;
					this->b = (float)b/255.0f;
					
					uniforms->setUniform("r", r);
					uniforms->setUniform("g", g);
					uniforms->setUniform("b", b);
					
					uniforms->update();
				}
				
				void setAlphaTreshold(float value) {
					this->alphaTreshold = value;
					uniforms->setUniform("a", alphaTreshold);
					
					uniforms->update();
				}
				
				float r, g, b, alphaTreshold;
				
				font::BitmapFont *font;
				
				std::string text;
				
				core::Tbo *tbo;
				
				program::Program *prog;
				
				core::UniformBlock *uniforms;
				
				TextAlignment alignment;
		};
	}
	
#endif