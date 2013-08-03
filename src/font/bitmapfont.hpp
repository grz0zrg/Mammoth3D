#ifndef BITMAPFONT_HPP
#define BITMAPFONT_HPP

	#include "../core/tbo.hpp"

	#include "../loaders/loader.hpp"
	#include "../loaders/textureloader.hpp"
	#include "../loaders/imageloader.hpp"
	
	namespace font {
		class BitmapFont {
			public:
				BitmapFont(const std::string &file) {
					loader::Loader *ldr = loader::Loader::getInstance();
					bitmap = ldr->createTexture(file);
					
					cellWidth  = bitmap->width  / 16;
					cellHeight = bitmap->height / 16;
					
					chars.resize(512);

					int currentChar = 0;
					for(int cols = 0; cols < 16; cols++) {
						for(int rows = 0; rows < 16; rows++) {
							chars[currentChar]   = (float)(cellWidth  * rows) / bitmap->width;
							chars[currentChar+1] = (float)(cellHeight * cols) / bitmap->height;
							
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