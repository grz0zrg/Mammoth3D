#ifndef IMAGE_HPP
#define IMAGE_HPP

	#include <vector>
	
	namespace core {
		class Image {
			public:
				Image(int width, int height, std::vector<unsigned char> data) {
					this->width = width;
					this->height = height;
					this->data = data;
				}
				
				Image(int width, int height) {
					this->width = width;
					this->height = height;
				}
				
				~Image() {
					
				}
				
			int width, height;
			std::vector<unsigned char> data;
		};
	}

#endif