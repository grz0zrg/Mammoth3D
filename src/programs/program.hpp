#ifndef MAMMOTH3D_PROGRAM_HPP
#define MAMMOTH3D_PROGRAM_HPP

	#include <map>
	#include <string>
	#include <vector>
	#include <iostream>

	#include <GL/glew.h>
	
	#include "../core/vbo.hpp"
	#include "../core/uniform_block.hpp"

	namespace program {
		class Program {
				public:
				Program(GLuint id) {
					_id = id;
					_vbos_count = 0;
				}
				
				~Program() {	
					for (unsigned int i = 0; i < _vbos.size(); i++) {
						delete _vbos[i];
					}
				}
				
				void bindUniformBlock(core::UniformBlock *ub) {
					GLuint block_index = glGetUniformBlockIndex(_id, ub->_name.c_str());
					if (block_index == GL_INVALID_INDEX) {
						std::string error = "\"" + ub->_name + "\"" + " does not identify an active uniform block";
						log(error.c_str());
					}
					
					glUniformBlockBinding(_id, block_index, ub->getBindingPoint());
				}

				void bindVbos() {
					for (unsigned int i = 0; i < _vbos_count; i++) {
						const auto vbo = _vbos[i];
						glBindBuffer(vbo->_target, vbo->_buffer_id);
						glVertexAttribPointer(vbo->_attrib_index, vbo->_components, 
                                        vbo->_data_type, vbo->_normalized, 0, 0);
					}
				}
				
				void addVbo(core::Vbo *vbo) {
					_vbos.push_back(vbo);
					_vbos_count = _vbos.size();
				}
				
				void registerUniform(const std::string &name) {
					_uniforms[name] = glGetUniformLocation(_id, name.c_str());
				}
				
				void setUniform1f(const std::string &name, float value) {
					glUniform1f(_uniforms[name], value);
				}
				
				void setUniform1i(const std::string &name, GLint value) {
					glUniform1i(_uniforms[name], value);
				}

				GLint getUniformLocation(const std::string &name) {
					return _uniforms[name];
				}
				
				void log(const char *str) {
					std::cout << "[Program     ] " << str << std::endl;
				}
				
				GLuint _id;
				
				std::map<std::string, GLint> _uniforms;
		
				std::vector<core::Vbo *> _vbos;
				unsigned int _vbos_count;
		};
	}

#endif