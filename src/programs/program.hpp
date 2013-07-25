#ifndef PROGRAM_HPP
#define PROGRAM_HPP

	#include <map>
	#include <string>
	#include <vector>

	#include <GL/glew.h>

	namespace program {
		class Program {
				public:
					class Uniforms {
						public:
							Uniforms(GLint loc, float *value = 0) { 
								location = loc; 
								fv = value; 
							}
							
							GLint location;
							float *fv;
					};
				
				Program(GLuint prog) {
					this->prog = prog;
				}
				
				~Program() {
				}
				
				void registerUniform(const std::string &name) {
					uniforms[name] = glGetUniformLocation(prog, name.c_str());
				}
				
				void registerDynamicUniform1f(const std::string &name, float *value) {
					dynamicUniforms[name] = new Uniforms(glGetUniformLocation(prog, name.c_str()), value);
				}
				
				void updateDynamicUniforms() {
					for(std::map<std::string, Uniforms *>::iterator it=dynamicUniforms.begin(); it!=dynamicUniforms.end(); ++it) {
						glUniform1f(it->second->location, *it->second->fv);
					}
				}
				
				GLint getUniformLocation(const std::string &name) {
					return uniforms[name];
				}
				
				GLuint prog;
				
				std::map<std::string, GLint> uniforms;
				std::map<std::string, Uniforms *> dynamicUniforms;
		};
	}

#endif