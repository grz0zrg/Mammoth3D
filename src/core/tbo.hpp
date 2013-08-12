#ifndef TBO_HPP
#define TBO_HPP

	#include <iostream>
	
	#include "texture.hpp"
	
	namespace core {
		class Tbo {
			public:
				template <class T>
				Tbo(const std::vector<T>& data, GLenum format = GL_R32F, GLenum usage = GL_STATIC_DRAW) {
					this->usage = usage;
					this->format = format;
					
					glGetError();
					
					glGenBuffers(1, &id);
			
					glBindBuffer(GL_TEXTURE_BUFFER, id); 
					if (data.empty()) {
						//glBufferData(GL_TEXTURE_BUFFER, 0, 0, usage); 
					} else {
						glBufferData(GL_TEXTURE_BUFFER, data.size() * sizeof(T), &data[0], usage); 
					}
					
					GLenum err = glGetError();
					if (err != GL_NO_ERROR) {
						std::cout << "Tbo creation failed, error code: " << err << std::endl;
					}
				}
				
				~Tbo() {
					glDeleteBuffers(1, &id);
				}
				
				template <class T>
				void updateData(const std::vector<T>& data) {
					glBindBuffer(GL_TEXTURE_BUFFER, id); 
					//glBufferSubData(GL_TEXTURE_BUFFER, 0, data.size() * sizeof(T), &data[0]);
					glBufferData(GL_TEXTURE_BUFFER, data.size() * sizeof(T), &data[0], GL_DYNAMIC_DRAW); 
				}
				
				GLuint id;
				GLenum usage, format;
		};
	}
	
#endif