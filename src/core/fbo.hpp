#ifndef MAMMOTH3D_FBO_HPP
#define MAMMOTH3D_FBO_HPP

	#include "texture.hpp"
	
	namespace core {
		class Fbo {
			public:
				Fbo(Texture *texture, bool multisampling = false, 
                                                    bool depth_buffer = true);
                ~Fbo();
				
				void setTexture(Texture *texture);
				
				void log(const char *str) {
					std::cout << "[Fbo] " << str << std::endl;
				}
				
				bool _depth_buffer, _multisampling;
				GLuint _id, _id_depthbuffer;
				Texture *_texture;
		};
	}
	
#endif