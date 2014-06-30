#include "imageloader.hpp"

loader::ImageLoader *loader::ImageLoader::_singleton = 0;

core::Image *loader::ImageLoader::loadImage(const std::string &fileName, int mirrored_vertically) {
	log("loading: ", fileName);
	
	if (_images.find(fileName) != _images.end()) {
		if (_images[fileName] != 0) {
			return _images[fileName];
		}
	}
	
	std::vector<unsigned char> data;
	unsigned width, height, width4, height_m1, height_d2;

	unsigned error = lodepng::decode(data, width, height, fileName.c_str());

	if (error != 0) {
		log(lodepng_error_text(error));
		return 0;
	}
	
	width4 = width * 4; // 4 = always rgba
	height_m1 = height - 1;
	height_d2 = height / 2;
	
	// post process phase (flipping, mirroring etc.)
	if (mirrored_vertically == 1) {
		for(unsigned int i = 0; i < height_d2; i++) {
			for(unsigned int j = 0; j < width4; j++) {
				data[(height_m1-i)*width4 + j] = data[i * width4 + j];
			}
		}
	} else if (mirrored_vertically == 2) {
		for(unsigned int i = 0; i < height_d2; i++) {
			for(unsigned int j = 0; j < width4; j++) {
				data[i * width4 + j] = data[(height_m1-i)*width4 + j];
			}
		}
	} 
	
	if (!mirrored_vertically) {
		for(unsigned int i = 0; i < height_d2; i++) {
			for(unsigned int j = 0; j < width4; j++) {
				unsigned int index = i*width4+j;
				unsigned int index_flipped = (height_m1-i)*width4 + j;
				unsigned char tmp = data[index_flipped];
				data[index_flipped] = data[index];
				data[index] = tmp;
			}
		}
	}
	
	core::Image *image = new core::Image(width, height, data);
	_images[fileName] = image;
	
	return image;
}
