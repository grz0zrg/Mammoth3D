#include "bitmapfont.hpp"

font::BitmapFont::BitmapFont(core::Texture *font_texture) {
    if (!font_texture) {
        return;
    }

    _bitmap = font_texture;
    
    font_texture->setNearestFiltering();
    //font_texture->setMaxAnisotropy();
    font_texture->generateMipmap();
					
    _cellWidth  = _bitmap->_width  / 16;
    _cellHeight = _bitmap->_height / 16;
					
    _chars.resize(512);

    int currentChar = 0;
    for(int cols = 0; cols < 16; cols++) {
        for(int rows = 0; rows < 16; rows++) {
            _chars[currentChar]   = ((float)(_cellWidth  * rows)+0.5f) / _bitmap->_width;
            _chars[currentChar+1] = ((float)(_cellHeight * cols)+0.5f) / _bitmap->_height;
							
            currentChar += 2;
        }
    }
}

font::BitmapFont::~BitmapFont() {
    delete _bitmap;
    _bitmap = 0;
}