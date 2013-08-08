#ifndef UNIFORM_BLOCK_HPP
#define UNIFORM_BLOCK_HPP

	#include <GL/glew.h>
	
	#include <map>
	#include <vector>
	#include <iostream>
	
	namespace core {
		class UniformBlock {
			public:
				UniformBlock(const std::string& block_name, GLuint bpoint = 0) {
					name = block_name;
					binding_point = bpoint;
					
					id = 0;
				}
				
				~UniformBlock() {
					glDeleteBuffers(1, &id);
				}
				
				void setUniform(const std::string& uniform_name,
								float value = 1.0f) {
					if (uniform_index.find(uniform_name) == uniform_index.end()) {
						uniform_index[uniform_name] = data.size();
						data.push_back(value);
						data_count = data.size();
					} else {
						data[uniform_index[uniform_name]] = value;
					}
				}
				
				void initialize(bool dynamic = true) {
					glDeleteBuffers(1, &id);

					glGenBuffers(1, &id);
					glBindBuffer(GL_UNIFORM_BUFFER, id);
	
					if (data.empty()) {
						std::cout << "[UniformBlock] initialize() failed, no data." << std::endl;
						return;
					}

					if (dynamic) {
						glBufferData(GL_UNIFORM_BUFFER, data.size() * sizeof(float), &data[0], GL_DYNAMIC_DRAW);
					} else {
						glBufferData(GL_UNIFORM_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
					}
					
					GLenum err = glGetError();
					if (err != GL_NO_ERROR) {
						std::cout << "[UniformBlock] \"" << name << "\" creation failed, error code: " << err << std::endl;
					}
				}
				
				void update() {
					glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, id);
					glBufferSubData(GL_UNIFORM_BUFFER, 0, data_count * sizeof(float), &data[0]);
				}
				
				std::vector<float> data;
				unsigned int data_count;
				
				std::map<std::string, unsigned int> uniform_index;
				
				std::string name;
				
				GLuint id, binding_point;
		};
	}

#endif