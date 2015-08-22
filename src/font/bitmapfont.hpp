#ifndef MAMMOTH3D_BITMAPFONT_HPP
#define MAMMOTH3D_BITMAPFONT_HPP

	#include "../core/tbo.hpp"

	#include "../core/texture.hpp"
	
	namespace font {
		class BitmapFont {
			public:
				BitmapFont(core::Texture *font_texture, int col_count = 16, int row_count = 16);
				
				~BitmapFont();

				core::Texture *_bitmap;
				
				std::vector<float> _chars;
				
				int _cellWidth, _cellHeight;
		};
	}
	
#endif