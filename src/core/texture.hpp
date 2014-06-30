#ifndef MAMMOTH3D_TEXTURE_HPP
#define MAMMOTH3D_TEXTURE_HPP

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

				void create(int width = 0, int height = 0, 
                                                GLenum type = GL_TEXTURE_2D);
				void create(Image *image, GLenum type = GL_TEXTURE_2D);
				
				void setTextureSize(int width, int height);

				void bindBuffer(GLenum format, GLuint buffer) {
					glBindTexture(_target, _id);
					glTexBuffer(GL_TEXTURE_BUFFER, format, buffer);
				}
				
				void setParameter(GLenum pname, GLint param) {
					glBindTexture(_target, _id);
					glTexParameteri(_target, pname, param);
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
					glBindTexture(_target, _id);
					
					GLfloat maxAniso = 0.0f;
					glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
					glTexParameteri(_target, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
				}
				
				void generateMipmap(int base_level = 0, int max_level = 4) {
					glBindTexture(_target, _id);
					
					glTexParameteri(_target, GL_TEXTURE_BASE_LEVEL, base_level);
					glTexParameteri(_target, GL_TEXTURE_MAX_LEVEL, max_level);
					
					glGenerateMipmap(_target);
				}
				
				void setMultisampling(int samples) {
					if (_target != GL_TEXTURE_2D || _image == 0) {
						return;
					}
					
					_samples = samples;
                    _target = GL_TEXTURE_2D_MULTISAMPLE;
                    
					glDeleteTextures(1, &_id);
					
					glGenTextures(1, &_id);
					
					glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, _id);
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, 
                                            GL_RGBA, _width, _height, GL_TRUE);
				}
				
				~Texture() {
					glDeleteTextures(1, &_id);
				}
				
			GLuint _id;
			int _width, _height;
			int _samples;
			
			Image *_image;
			
			GLenum _target;
		};
	}

#endif