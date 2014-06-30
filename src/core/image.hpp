#ifndef MAMMOTH3D_IMAGE_HPP
#define MAMMOTH3D_IMAGE_HPP

	#include <vector>
	
	namespace core {
		class Image {
			public:
				Image(int width, int height, std::vector<unsigned char> data) {
					_width  = width;
					_height = height;
					_data   = data;
				}
				
				Image(int width, int height) {
					_width  = width;
					_height = height;
				}
				
				~Image() {
					
				}
				
			int _width, _height;
			std::vector<unsigned char> _data;
		};
	}

#endif