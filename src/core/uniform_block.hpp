#ifndef MAMMOTH3D_UNIFORM_BLOCK_HPP
#define MAMMOTH3D_UNIFORM_BLOCK_HPP

	#include <GL/glew.h>
	
	#include <map>
	#include <vector>
	#include <iostream>
	
	namespace core {
		class UniformBlock {
			public:
				UniformBlock(const std::string& block_name) {
					_name = block_name;
					_binding_point = _binding_point_counter;
					_binding_point_counter++;
					
					_id = 0;
				}
				
				~UniformBlock() {
					glDeleteBuffers(1, &_id);
				}
				
				void setUniform(const std::string& uniform_name,
								float value = 1.0f);
                                
				void initialize(bool dynamic = true);
                
				void update();
				
				GLuint getBindingPoint() {
					return _binding_point;
				}
				
				std::vector<float> _data;
				unsigned int _data_count;
				
				std::map<std::string, unsigned int> _uniform_index;
				
				std::string _name;
				
				GLuint _id;
				GLuint _binding_point;
				
			private:
				static GLuint _binding_point_counter;
        
				template <typename T>
				void log(const std::string &str, const std::string &str2, 
                            const std::string &str3, T param) {
					std::cout << "[UniformBlock] " << str << str2 << 
                                                    str3 << param << std::endl;
				}
        
				template <typename T>
				void log(const std::string &str, T param) {
					std::cout << "[UniformBlock] " << str << param << std::endl;
				}
                
				void log(const char *str) {
					std::cout << "[UniformBlock] " << str << std::endl;
				}
		};
	}

#endif