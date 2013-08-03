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
				BitmapText(font::BitmapFont *font) {
					this->font = font;
					
					this->geom = new core::Geometry();
					
					for (unsigned int i = 0; i < 12; i++) {
						this->geom->vertices.push_back(builtingeometry::quad[i]);
					}
									
					for (unsigned int i = 0; i < 8; i++) {
						this->geom->uvs.push_back(builtingeometry::quadUV[i]);
					}
					
					this->geom->generateVbo();
					
					std::vector<int> empty;
					tbo = new core::Tbo(empty, GL_RG32F, GL_DYNAMIC_DRAW);
					
					this->mat = new material::Material();
					this->mat->setTexture(font->bitmap);
					
					loader::ShaderLoader *sldr = loader::ShaderLoader::getInstance();
					
					prog = new program::Program(sldr->getBitmapFontsProgram()->prog);
					
					this->mat->setProgram(prog);
					this->mat->setDepthWrite(true);
					this->mat->setCullMode(GL_NONE);
					
					uniforms = new core::UniformBlock("infosBlock");
					uniforms->setUniform("text_length", 0.0f);
					uniforms->setUniform("font_width", font->bitmap->width);
					uniforms->setUniform("font_height", font->bitmap->height);
					uniforms->setUniform("cell_width", font->cellWidth);
					uniforms->setUniform("cell_height", font->cellHeight);

					prog->registerUniform("chars_pos");
					
					prog->addUniformBlock(uniforms);

					//this->mat->setBlending(true);

					this->type = BITMAP_TEXT;
					
					alignment = CENTER;
				}
				
				~BitmapText() {
					delete prog;
					delete this->mat;
					delete this->geom;
					delete tbo;
				}
				
				void setText(const std::string& text) {
					if (this->text != text) {
						std::vector<float> chars_map;
						for (unsigned int i = 0; i < text.length(); i++) {
							int cIndex = text[i] * 2;
							chars_map.push_back(font->chars[cIndex]);
							chars_map.push_back(font->chars[cIndex+1]);
						}
						tbo->updateData(chars_map);
						
						uniforms->setUniform("text_length", (float)(text.length()*16)/256.0f);
					}
					
					this->text = text;
				}
				
				font::BitmapFont *font;
				
				std::string text;
				
				core::Tbo *tbo;
				
				program::Program *prog;
				
				core::UniformBlock *uniforms;
				
				TextAlignment alignment;
		};
	}
	
#endif