#include "bitmaptext.hpp"

object::BitmapText::BitmapText(font::BitmapFont *font) {
	_font = font;
			
	_geom = new core::Geometry();

	for (unsigned int i = 0; i < 6; i++) {
		_geom->_indices.push_back(builtingeometry::quadIndice[i]);
	}
	
	for (unsigned int i = 0; i < 12; i++) {
		_geom->_vertices.push_back(builtingeometry::quadVertice[i]);
	}
									
	for (unsigned int i = 0; i < 8; i++) {
		_geom->_uvs.push_back(builtingeometry::quadUV[i]);
	}
	
	_geom->update();
	
	auto sldr = loader::ShaderLoader::getInstance();
					
	_prog = new program::Program(sldr->getBitmapFontsProgram()->_id);
				
	_mat = new material::Material();
	_mat->setProgram(_prog);
	_mat->setDepthWrite(true);
	_mat->setCullMode(GL_NONE);

	std::vector<int> empty;
	_tbo = new core::Tbo(empty, GL_RG32F, GL_DYNAMIC_DRAW);
	_texture_buffer = new core::Texture();
	_texture_buffer->create(0, 0, GL_TEXTURE_BUFFER);
	_texture_buffer->bindBuffer(_tbo->_format, _tbo->_id);
				
	_mat->setTexture(font->_bitmap);
	_mat->setTexture(_texture_buffer, 1);
				
	_r = 0.0f; _g = 1.0f; _b = 0.0f;
	_alpha_treshold = 0.5f;
			
	_uniforms = new core::UniformBlock("infosBlock");
	_uniforms->set("r", _r);
	_uniforms->set("g", _g);
	_uniforms->set("b", _b);
	_uniforms->set("a", _alpha_treshold);
	//uniforms->set("text_length", 0);
	_uniforms->set("font_width", font->_bitmap->_width);
	_uniforms->set("font_height", font->_bitmap->_height);
	_uniforms->set("cell_width", font->_cellWidth);
	_uniforms->set("cell_height", font->_cellHeight);
	_uniforms->initialize();

	_prog->bindUniformBlock(_uniforms);
	
	//this->mat->setBlending(true);

	//this->type = BITMAP_TEXT;
	_type = QUAD_ALIGNED;
}

object::BitmapText::~BitmapText() {
	delete _prog;
	delete _mat;
	delete _geom;
	delete _texture_buffer;
	delete _tbo;
	delete _uniforms;
}

void object::BitmapText::setText(const std::string& text) {
	if (_text != text) {
		std::vector<float> chars_map;
		for (unsigned int i = 0; i < text.length(); i++) {
			int cIndex = text[i] * 2;
			chars_map.push_back(_font->_chars[cIndex]);
			chars_map.push_back(_font->_chars[cIndex+1]);
		}
		_tbo->updateData(chars_map);
		
		//uniforms->set("text_length", (float)(text.length()*font->cellWidth)/font->bitmap->width);
		//uniforms->update();
	}
					
	_text = text;
}