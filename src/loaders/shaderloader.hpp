#ifndef SHADERLOADER_HPP
#define SHADERLOADER_HPP

	#include <GL/glew.h>
	#include <string>
	#include <vector>
	#include <iostream>
	#include <sstream>
	#include <fstream>
	#include <algorithm>
	
	#include "../programs/program.hpp"

	namespace loader {
		class ShaderLoader {
			private:
				ShaderLoader() { };
			
				~ShaderLoader() {
					std::for_each(programList.begin(), programList.end(), glDeleteProgram);
					std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
				};
			
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
				std::vector<GLuint> programList;
			
			public:
				void compileShader(GLenum eShaderType, const std::string &strShader);
				void compileShaderFile(GLenum eShaderType, const std::string &filename);
				program::Program *buildProgram();
				
				static ShaderLoader *getInstance()
				{
					if (!_singleton) {
						_singleton =  new ShaderLoader;
					}

					return _singleton;
				}

				static void free()
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