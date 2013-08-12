#ifndef PROGRAM_HPP
#define PROGRAM_HPP

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
				Program(GLuint prog) {
					this->prog = prog;
					vbos_count = 0;
				}
				
				~Program() {	
					for (unsigned int i = 0; i < vbos.size(); i++) {
						delete vbos[i];
					}
				}
				
				void bindUniformBlock(core::UniformBlock *ub) {
					GLuint blockIndex = glGetUniformBlockIndex(prog, ub->name.c_str());
					glUniformBlockBinding(prog, blockIndex, ub->getBindingPoint());
				}

				void bindVbos() {
					for (unsigned int i = 0; i < vbos_count; i++) {
						const core::Vbo *vbo = vbos[i];
						glBindBuffer(vbo->target, vbo->bufferId);
						glVertexAttribPointer(vbo->attrib_index, vbo->components, vbo->data_type, vbo->normalized, 0, 0);
					}
				}
				
				void addVbo(core::Vbo *vbo) {
					vbos.push_back(vbo);
					vbos_count = vbos.size();
				}
				
				void registerUniform(const std::string &name) {
					uniforms[name] = glGetUniformLocation(prog, name.c_str());
				}
				
				void setUniform1f(const std::string &name, float value) {
					glUniform1f(uniforms[name], value);
				}
				
				void setUniform1i(const std::string &name, GLint value) {
					glUniform1i(uniforms[name], value);
				}

				GLint getUniformLocation(const std::string &name) {
					return uniforms[name];
				}
				
				GLuint prog;
				
				std::map<std::string, GLint> uniforms;
		
				std::vector<core::Vbo *> vbos;
				unsigned int vbos_count;
		};
	}

#endif