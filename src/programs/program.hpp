#ifndef PROGRAM_HPP
#define PROGRAM_HPP

	#include <map>
	#include <string>
	#include <vector>
	#include <iostream>

	#include <GL/glew.h>
	
	#include "../core/uniform_block.hpp"

	namespace program {
		class Program {
				public:
				class Ubo {
					public:
						core::UniformBlock *block;
						GLuint bindingPoint;
						GLuint blockIndex;
						GLuint id;
				};
				
				Program(GLuint prog) {
					this->prog = prog;
				}
				
				~Program() {
					for (unsigned int i = 0; i < ubos.size(); i++) {
						delete ubos[i];
					}
				}
				
				void addUniformBlock(core::UniformBlock *ub) {
					Ubo *ubo = new Ubo();
					
					ubo->block = ub;
					ubo->bindingPoint = 0;
					ubo->blockIndex = glGetUniformBlockIndex(prog, ub->name.c_str());
					glUniformBlockBinding(prog, ubo->blockIndex, ubo->bindingPoint);
					
					glGenBuffers(1, &ubo->id);
					glBindBuffer(GL_UNIFORM_BUFFER, ubo->id);
					if (!ub->data.empty()) {
						glBufferData(GL_UNIFORM_BUFFER, ub->data.size() * sizeof(float), &ub->data[0], GL_DYNAMIC_DRAW);
					}
					
					glBindBufferBase(GL_UNIFORM_BUFFER, ubo->bindingPoint, ubo->id);
				
					GLenum err = glGetError();
					if (err != GL_NO_ERROR) {
						std::cout << "Ubo creation failed while adding \"" << ub->name << "\" block, error code: " << err << std::endl;
						std::cout << "Check if the uniform is used..." << std::endl;
					}
					
					ubos.push_back(ubo);
				}
				
				void updateUniforms() {
					for (unsigned int i = 0; i < ubos.size(); i++) {
						glBindBufferBase(GL_UNIFORM_BUFFER, ubos[i]->bindingPoint, ubos[i]->id);
						glBufferSubData(GL_UNIFORM_BUFFER, 0, ubos[i]->block->data.size() * sizeof(float), &ubos[i]->block->data[0]);
					}
				}
				
				void registerUniform(const std::string &name) {
					uniforms[name] = glGetUniformLocation(prog, name.c_str());
				}

				GLint getUniformLocation(const std::string &name) {
					return uniforms[name];
				}
				
				GLuint prog;
				
				std::map<std::string, GLint> uniforms;
					
				std::vector<Ubo *> ubos;
		};
	}

#endif