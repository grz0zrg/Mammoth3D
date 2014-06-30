#ifndef MAMMOTH3D_IMAGELOADER_HPP
#define MAMMOTH3D_IMAGELOADER_HPP

	#include <fstream>
	#include <iostream>
	#include <vector>
	#include <map>
	
	#include "lodepng/lodepng.h"
	
	#include "../core/image.hpp"
	
	namespace loader {
		class ImageLoader {
			public:
				ImageLoader() { 
				}
			
				~ImageLoader() { 
					std::map<std::string, core::Image *>::iterator imagesIt 
																= _images.begin();
					for (imagesIt; imagesIt != _images.end(); ++imagesIt) {
						if (imagesIt->second) {
							delete imagesIt->second;
						}
					}
				}
				
				static ImageLoader *getInstance()
				{
					if (!_singleton) {
						_singleton =  new ImageLoader;
					}

					return _singleton;
				}

				static void free()
				{
					if (_singleton)
					{
						delete _singleton;
						_singleton = 0;
					}
				}

				std::map<std::string, core::Image *> _images;
				
				core::Image *loadImage(const std::string &fileName, int mirrored_vertically = 0);
			
			private:
				template <typename T>
				void log(const std::string &str, T param) {
					std::cout << "[ImageLoader ] " << str << param << std::endl;
				}
					
				void log(const char *str) {
					std::cout << "[ImageLoader ] " << str << std::endl;
				}
				
			ImageLoader(const ImageLoader&);
			void operator=(const ImageLoader&);
			static ImageLoader *_singleton;
		};
	}

#endif