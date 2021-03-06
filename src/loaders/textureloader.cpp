#include "textureloader.hpp"

loader::TextureLoader *loader::TextureLoader::_singleton = 0;

core::Texture *loader::TextureLoader::loadTexture(core::Image *image) {
	if (!image) {
		log("invalid image");
		return 0;
	}

	if (!core::math::isPowerOfTwo(image->_width) && 
		!core::math::isPowerOfTwo(image->_height)) { // build power of two textures
		int u2 = 1; while(u2 < image->_width)  u2 *= 2;
		int v2 = 1; while(v2 < image->_height) v2 *= 2;

		double u3 = (double)image->_width / u2;
		double v3 = (double)image->_height / v2;

		std::vector<unsigned char> new_image_data(u2 * v2 * 4);

		for(int y = 0; y < image->_height; y++) {
			for(int x = 0; x < image->_width; x++) {
				for(int c = 0; c < 4; c++) {
					new_image_data[4 * u2 * y + 4 * x + c] = image->_data[4 * image->_width * y + 4 * x + c];
				}
			}
		}
		
		image->_width = u2;
		image->_width = v2;
		image->_data = new_image_data;
	}
	
	core::Texture *texture = new core::Texture(image);
	texture->generateMipmap(0, 4);
	texture->setLinearFiltering();
	texture->setMaxAnisotropy();
	_textures.push_back(texture);
	
	return texture;
}

core::Texture *loader::TextureLoader::createEmptyTexture(int width, int height, GLenum type) {
	core::Texture *texture = new core::Texture();
	texture->create(width, height, type);
	_textures.push_back(texture);
	
	return texture;
}