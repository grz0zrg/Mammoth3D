#ifndef PROGRAM_HPP
#define PROGRAM_HPP

	#include <map>
	#include <string>
	#include <vector>

	#include <GL/glew.h>

	namespace program {
		class Program {
			public:
				Program(GLuint prog) {
					this->prog = prog;
				}
				
				~Program() {
				}
				
				void registerUniform(const std::string &name) {
					uniforms[name] = glGetUniformLocation(prog, name.c_str());
				}
				
				GLint getUniformLocation(const std::string &name) {
					return uniforms[name];
				}
				
				GLuint prog;
				
				std::map<std::string, GLint> uniforms;
		};
	}

#endif