#include "textureloader.hpp"

loader::TextureLoader *loader::TextureLoader::_singleton = 0;

core::Texture *loader::TextureLoader::loadTexture(core::Image *image) {
	if (!image) {
		log("invalid image");
		return 0;
	}

	if (!core::math::isPowerOfTwo(image->width) && 
		!core::math::isPowerOfTwo(image->height)) { // build power of two textures
		int u2 = 1; while(u2 < image->width)  u2 *= 2;
		int v2 = 1; while(v2 < image->height) v2 *= 2;

		double u3 = (double)image->width / u2;
		double v3 = (double)image->height / v2;

		std::vector<unsigned char> new_image_data(u2 * v2 * 4);

		for(int y = 0; y < image->height; y++) {
			for(int x = 0; x < image->width; x++) {
				for(int c = 0; c < 4; c++) {
					new_image_data[4 * u2 * y + 4 * x + c] = image->data[4 * image->width * y + 4 * x + c];
				}
			}
		}
		
		image->width = u2;
		image->width = v2;
		image->data = new_image_data;
	}
	
	core::Texture *texture = new core::Texture(image);
	textures.push_back(texture);
	
	return texture;
}
