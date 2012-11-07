#ifndef SHADERLOADER_HPP
#define SHADERLOADER_HPP

	#include <GL/glew.h>
	#include <string>
	#include <vector>
	#include <iostream>
	#include <algorithm>
	
	namespace loader {
		class ShaderLoader {
			private:
				ShaderLoader() { };
			
				~ShaderLoader() { };
			
				template <typename T>
				void logPretty(const std::string &str, T param) {
					std::cout << "[ShaderLoader] " << str << "\"" << 
								param << "\"" << std::endl;
				}
				
				template <typename T>
				void log(const std::string &str, T param) {
					std::cout << "[ShaderLoader] " << str << param << std::endl;
				}
					
				void log(const char *str) {
					std::cout << "[ShaderLoader] " << str << std::endl;
				}

				ShaderLoader(const ShaderLoader&);
				void operator=(const ShaderLoader&);
				static ShaderLoader *_singleton;
				
				std::vector<GLuint> shaderList;
			
			public:
				void compileShader(GLenum eShaderType, const std::string &strShaderFile);
				GLuint buildProgram();
				
				static ShaderLoader *getInstance()
				{
					if (!_singleton) {
						_singleton =  new ShaderLoader;
					}

					return _singleton;
				}

				static void kill()
				{
					if (_singleton)
					{
						delete _singleton;
						_singleton = 0;
					}
				}
		};
	}

#endif