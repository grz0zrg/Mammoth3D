#ifndef UNIFORM_BLOCK_HPP
#define UNIFORM_BLOCK_HPP

	#include <GL/glew.h>
	
	#include <map>
	#include <vector>
	
	namespace core {
		class UniformBlock {
			public:
				UniformBlock(const std::string& block_name) {
					name = block_name;
				}
				
				~UniformBlock() {
				}
				
				void setUniform(const std::string& uniform_name,
								float value) {
					if (uniform_index.find(uniform_name) == uniform_index.end()) {
						uniform_index[uniform_name] = data.size();
						data.push_back(value);
					} else {
						data[uniform_index[uniform_name]] = value;
					}
				}
				
				std::vector<float> data;
				
				std::map<std::string, unsigned int> uniform_index;
				
				std::string name;
		};
	}

#endif