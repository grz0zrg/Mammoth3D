#ifndef TEXTURE_HPP
#define TEXTURE_HPP

	#include <vector>
	#include <GL/glew.h>
#include <iostream>
	#include "image.hpp"
	
	namespace core {
		class Texture {
			public:
				Texture(Image *image, bool linear_filtering = true, bool anisotropy = true, bool mipmaps = true) {
					width = image->width;
					height = image->height;

					glGenTextures(1, &id);
					glBindTexture(GL_TEXTURE_2D, id);
					/*
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
					*/
					if (linear_filtering) {
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					} else {
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					}
					
					if (mipmaps) {
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);
					}
					
					if (anisotropy) {
						GLfloat maxAniso = 0.0f;
						glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
					}
					
					if (image->data.empty()) {
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
					} else {
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image->data[0]);
					}

					if (mipmaps) {
						glGenerateMipmap(GL_TEXTURE_2D);
					}
				}
				
				~Texture() {
					glDeleteTextures(1, &id);
				}
				
			GLuint id;
			int width, height;
		};
	}

#endif