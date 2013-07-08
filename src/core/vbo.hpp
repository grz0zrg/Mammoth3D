#ifndef VBO_HPP
#define VBO_HPP

	#include <GL/glew.h>
	
	namespace core {
		class Vbo {
			public:
				Vbo() {
					verticeBufferUsage = colorBufferUsage = GL_STATIC_DRAW;
					
					indiceBufferId = verticeBufferId = colorBufferId = uvBufferId = normalBufferId = 0;
				}
				
				~Vbo() {
					glDeleteBuffers(1, &indiceBufferId);
					glDeleteBuffers(1, &verticeBufferId);
					glDeleteBuffers(1, &colorBufferId);
					glDeleteBuffers(1, &uvBufferId);
					glDeleteBuffers(1, &normalBufferId);
				}
				
				bool buildBuffer(GLuint *bufferId, GLsizeiptr size, 
												const GLvoid *data, 
												GLenum target = GL_ARRAY_BUFFER,
												GLenum usage = GL_STATIC_DRAW) {
					if (size <= 0) {
						return false;
					}
					
					glGenBuffers(1, bufferId);
					glBindBuffer(target, *bufferId);
					glBufferData(target, size, data, usage);
						
					GLenum err = glGetError();
					if (err != GL_NO_ERROR) {
						glDeleteBuffers(1, bufferId);
						return false;
					}
					
					return true;
				}
				
				bool buildIndiceBuffer(GLsizeiptr size, const GLvoid *data) {
					if (!buildBuffer(&indiceBufferId, size, data, 
													GL_ELEMENT_ARRAY_BUFFER)) {
						log("Indice buffer creation failed.");
						return false;
					}
					
					return true;
				}

				bool buildVerticeBuffer(GLsizeiptr size, const GLvoid *data) {
					if (!buildBuffer(&verticeBufferId, size, data, 
										GL_ARRAY_BUFFER, verticeBufferUsage)) {
						log("Vertice buffer creation failed.");
						return false;
					}
					
					return true;
				}
				
				bool buildUvBuffer(GLsizeiptr size, const GLvoid *data) {
					if (!buildBuffer(&uvBufferId, size, data, 
										GL_ARRAY_BUFFER)) {
						log("UV buffer creation failed.");
						return false;
					}
					
					return true;
				}
				
				bool buildNormalBuffer(GLsizeiptr size, const GLvoid *data) {
					if (!buildBuffer(&uvBufferId, size, data, 
										GL_ARRAY_BUFFER)) {
						log("Normal buffer creation failed.");
						return false;
					}
					
					return true;
				}

				bool buildColorBuffer(GLsizeiptr size, const GLvoid *data) {
					if (!buildBuffer(&colorBufferId, size, data, GL_ARRAY_BUFFER,
															colorBufferUsage)) {
						log("Color buffer creation failed.");
						return false;
					}
					
					return true;
				}
				
				void updateColorBuffer(GLsizeiptr size, const GLvoid *data) {
					if (colorBufferId) {
						glBindBuffer(GL_ARRAY_BUFFER, colorBufferId);
						glBufferData(GL_ARRAY_BUFFER, size, data, colorBufferUsage);
					} else {
						buildColorBuffer(size, data);
					}
				}
				
				void updateVerticeBuffer(GLsizeiptr size, const GLvoid *data) {
					if (verticeBufferId) {
						glBindBuffer(GL_ARRAY_BUFFER, verticeBufferId);
						glBufferData(GL_ARRAY_BUFFER, size, data, verticeBufferUsage);
					} else {
						buildVerticeBuffer(size, data);
					}
				}
				
				void deleteColorBuffer() {
					glDeleteBuffers(1, &colorBufferId);
				}
				
				void deleteVerticeBuffer() {
					glDeleteBuffers(1, &verticeBufferId);
				}
				
				void log(const char *str) {
					std::cout << "[Vbo] " << str << std::endl;
				}
				
				GLuint indiceBufferId, verticeBufferId, colorBufferId, uvBufferId, normalBufferId;
				GLenum verticeBufferUsage, colorBufferUsage;
		};
	}

#endif