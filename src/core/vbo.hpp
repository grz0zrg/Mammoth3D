#ifndef VBO_HPP
#define VBO_HPP

	#include <GL/glew.h>
	#include <iostream>
	
	namespace core {
		class Vbo {
			public:
				Vbo() {
					usage = GL_STATIC_DRAW;
					target = GL_ARRAY_BUFFER;
					bufferId = attrib_index = 0;
					data_type = GL_FLOAT;
					normalized = GL_FALSE;
					components = 3;
				}
				
				~Vbo() {
					destroy();
				}
				
				bool build(GLsizeiptr size, const GLvoid *data, 
						   GLenum target = GL_ARRAY_BUFFER,
						   GLenum usage = GL_STATIC_DRAW) {
					if (size <= 0) {
						return false;
					}
					
					if (bufferId) {
						destroy();
					}
					
					glGenBuffers(1, &bufferId);
					glBindBuffer(target, bufferId);
					glBufferData(target, size, data, usage);
						
					GLenum err = glGetError();
					if (err != GL_NO_ERROR) {
						destroy();
						log("Creation failed.");
						return false;
					}
					
					return true;
				}
				
				void update(GLsizeiptr size, const GLvoid *data) {
					if (bufferId == 0) {
						return;
					}

					glBindBuffer(target, bufferId);
					
					//glBufferSubData(target, 0, size, data);
					glBufferData(target, size, data, usage);
				}
				
				void attrib(GLuint index) {
					if (bufferId == 0) {
						return;
					}
					
					glBindBuffer(target, bufferId);
					glEnableVertexAttribArray(index);
					
					attrib_index = index;
				}
				
				void setUsage(GLenum usage) {
					this->usage = usage;
				}
				
				void setComponents(GLint c) {
					components = c;
				}
				
				void setDataType(GLenum dtype) {
					data_type = dtype;
				}
				
				void setNormalized(GLboolean n) {
					normalized = n;
				}
				
				void destroy() {
					glDeleteBuffers(1, &bufferId);
				}

				void log(const char *str) {
					std::cout << "[Vbo] " << str << std::endl;
				}

				GLuint bufferId, attrib_index;
				GLenum target, usage, data_type;
				GLboolean normalized;
				GLint components;
		};
	}

#endif