#ifndef TEXTURE_HPP
#define TEXTURE_HPP

	#include <vector>
	#include <iostream>
	#include <GL/glew.h>

	#include "image.hpp"
	
	namespace core {
		class Texture {
			public:
				Texture(Image *image = 0, GLenum type = GL_TEXTURE_2D) {
					create(image, type);
				}

				void create(int width = 0, int height = 0, GLenum type = GL_TEXTURE_2D) {
					this->width  = width;
					this->height = height;

					glDeleteTextures(1, &id);
					
					glGenTextures(1, &id);

					target = type;
					
					glBindTexture(target, id);
		
					if (target == GL_TEXTURE_2D) {
						//std::vector<unsigned char> fill;
						//fill.resize(width*height*4, 0);
						glTexImage2D(target, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
						glGenerateMipmap(target);
					}
				}
				
				void create(Image *image, GLenum type = GL_TEXTURE_2D) {
					if (!image) {
						return;
					}
					
					this->image = image;
					
					width = image->width;
					height = image->height;
					
					glDeleteTextures(1, &id);
					
					glGenTextures(1, &id);

					target = type;

					glBindTexture(target, id);
					
					if (image->data.empty()) {
						glTexImage2D(target, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
					} else {
						glTexImage2D(target, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image->data[0]);
					}

					glGenerateMipmap(target);
				}
				
				void setTextureSize(int width, int height) {
					glBindTexture(target, id);
					
					if (target == GL_TEXTURE_2D) {
						if (image) { // the image still need to be resized before
							glTexImage2D(target, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image->data[0]);
						} else {
							glTexImage2D(target, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
						}
						glGenerateMipmap(target);
					}
				}

				void bindBuffer(GLenum format, GLuint buffer) {
					glBindTexture(target, id);
					glTexBuffer(GL_TEXTURE_BUFFER, format, buffer);
				}
				
				void setParameter(GLenum pname, GLint param) {
					glBindTexture(target, id);
					glTexParameteri(target, pname, param);
				}
				
				void setNearestFiltering() {
					setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				}
				
				void setLinearFiltering() {
					setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				}
				
				void setMaxAnisotropy() {
					glBindTexture(target, id);
					
					GLfloat maxAniso = 0.0f;
					glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
					glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
				}
				
				void generateMipmap(int base_level = 0, int max_level = 4) {
					glBindTexture(target, id);
					
					glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, base_level);
					glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, max_level);
					
					glGenerateMipmap(target);
				}
				
				~Texture() {
					glDeleteTextures(1, &id);
				}
				
			GLuint id;
			int width, height;
			
			Image *image;
			
			GLenum target;
		};
	}

#endif