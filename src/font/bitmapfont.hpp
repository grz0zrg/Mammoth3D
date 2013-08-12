#ifndef BITMAPFONT_HPP
#define BITMAPFONT_HPP

	#include "../core/tbo.hpp"

	#include "../core/texture.hpp"
	
	namespace font {
		class BitmapFont {
			public:
				BitmapFont(core::Texture *font_texture) {
					if (!font_texture) {
						return;
					}

					bitmap = font_texture;
					
					font_texture->setLinearFiltering();
					
					cellWidth  = bitmap->width  / 16;
					cellHeight = bitmap->height / 16;
					
					chars.resize(512);

					int currentChar = 0;
					for(int cols = 0; cols < 16; cols++) {
						for(int rows = 0; rows < 16; rows++) {
							chars[currentChar]   = ((float)(cellWidth  * rows)+0.5f) / bitmap->width;
							chars[currentChar+1] = ((float)(cellHeight * cols)+0.5f) / bitmap->height;
							
							currentChar += 2;
						}
					}
				}
				
				~BitmapFont() {
					delete bitmap;
					bitmap = 0;
				}

				core::Texture *bitmap;
				
				std::vector<float> chars;
				
				int cellWidth, cellHeight;
		};
	}
	
#endif