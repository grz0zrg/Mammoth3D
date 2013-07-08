#ifndef TEXTURE_HPP
#define TEXTURE_HPP

	#include <vector>
	#include <GL/glew.h>
	
	#include "image.hpp"
	
	namespace core {
		class Texture {
			public:
				Texture(Image *image) {
					glGenTextures(1, &id);
					glBindTexture(GL_TEXTURE_2D, id);
					
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);
					
					GLfloat maxAniso = 0.0f;
					glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
					
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image->data[0]);
					
					glGenerateMipmap(GL_TEXTURE_2D);
				}
				
				~Texture() {
					glDeleteTextures(1, &id);
				}
				
			GLuint id;
		};
	}

#endif