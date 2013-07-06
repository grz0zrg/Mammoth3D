#include "ImageLoader.hpp"

loader::ImageLoader *loader::ImageLoader::_singleton = 0;

core::Image *loader::ImageLoader::loadImage(const std::string &fileName) {
	logPretty("loading: ", fileName);
	
	if (images.find(fileName) != images.end()) {
		if (images[fileName] != 0) {
			return images[fileName];
		}
	}
	
	std::vector<unsigned char> data;
	unsigned width, height;

	unsigned error = lodepng::decode(data, width, height, fileName.c_str());

	if (error != 0) {
		log(lodepng_error_text(error));
		return 0;
	}
	
	core::Image *image = new core::Image(width, height, data);
	images[fileName] = image;
	
	return image;
}
