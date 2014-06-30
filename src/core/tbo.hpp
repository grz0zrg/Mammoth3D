#ifndef MAMMOTH3D_TBO_HPP
#define MAMMOTH3D_TBO_HPP

	#include <iostream>
	
	#include "texture.hpp"
	
	namespace core {
		class Tbo {
			public:
				template <class T>
				Tbo(const std::vector<T>& data, GLenum format = GL_R32F, 
                                            GLenum usage = GL_STATIC_DRAW) {
					_usage = usage;
					_format = format;
					
					glGetError();
					
					glGenBuffers(1, &_id);
			
					glBindBuffer(GL_TEXTURE_BUFFER, _id); 
					if (data.empty()) {
						//glBufferData(GL_TEXTURE_BUFFER, 0, 0, usage); 
					} else {
						glBufferData(GL_TEXTURE_BUFFER, data.size() * sizeof(T), 
                                                            &data[0], usage); 
					}
					
					GLenum err = glGetError();
					if (err != GL_NO_ERROR) {
						log("creation failed, error code: ", err);
					}
				}
				
				~Tbo() {
					glDeleteBuffers(1, &_id);
				}
				
				template <class T>
				void updateData(const std::vector<T>& data) {
					glBindBuffer(GL_TEXTURE_BUFFER, _id); 
					//glBufferSubData(GL_TEXTURE_BUFFER, 0, data.size() * sizeof(T), &data[0]);
					glBufferData(GL_TEXTURE_BUFFER, data.size() * sizeof(T), 
                                                    &data[0], GL_DYNAMIC_DRAW); 
				}
                
				GLuint _id;
				GLenum _usage, _format;
                
            private:
				template <typename T>
				void log(const std::string &str, T param) {
					std::cout << "[Tbo] " << str << param << std::endl;
				}
                
				void log(const char *str) {
					std::cout << "[Tbo] " << str << std::endl;
				}
		};
	}
	
#endif