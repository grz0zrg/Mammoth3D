#ifndef MAMMOTH3D_VBO_HPP
#define MAMMOTH3D_VBO_HPP

	#include <GL/glew.h>
	#include <iostream>
	
	namespace core {
		class Vbo {
			public:
				Vbo() {
					_usage = GL_STATIC_DRAW;
					_target = GL_ARRAY_BUFFER;
					_buffer_id = _attrib_index = 0;
					_data_type = GL_FLOAT;
					_normalized = GL_FALSE;
					_components = 3;
				}
				
				~Vbo() {
					destroy();
				}
				
				bool build(GLsizeiptr size, const GLvoid *data, 
						   GLenum target = GL_ARRAY_BUFFER,
						   GLenum usage = GL_STATIC_DRAW);
				
				void update(GLsizeiptr size, const GLvoid *data);
				
				void attrib(GLuint index);
				
				void setUsage(GLenum usage) {
					_usage = usage;
				}
				
				void setComponents(GLint c) {
					_components = c;
				}
				
				void setDataType(GLenum dtype) {
					_data_type = dtype;
				}
				
				void setNormalized(GLboolean n) {
					_normalized = n;
				}
				
				void destroy() {
					glDeleteBuffers(1, &_buffer_id);
				}

				GLuint _buffer_id, _attrib_index;
				GLenum _target, _usage, _data_type;
				GLboolean _normalized;
				GLint _components;
                
            private:
				void log(const char *str) {
					std::cout << "[Vbo] " << str << std::endl;
				}
		};
	}

#endif