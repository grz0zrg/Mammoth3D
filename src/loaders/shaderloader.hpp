#ifndef MAMMOTH3D_SHADERLOADER_HPP
#define MAMMOTH3D_SHADERLOADER_HPP

	#include <GL/glew.h>
	#include <string>
	#include <vector>
	#include <iostream>
	#include <sstream>
	#include <fstream>
	#include <algorithm>
	#include <cstring>
	
	#include "../programs/program.hpp"
	#include "../misc/builtinshaders.hpp"

	namespace loader {
		class ShaderLoader {
			private:
				ShaderLoader() { 
					_bitmap_fonts_program = 0;
				};
			
				~ShaderLoader() {
					std::for_each(_program_list.begin(), _program_list.end(), 
															glDeleteProgram);
					std::for_each(_shader_list.begin(), _shader_list.end(), 
															glDeleteShader);
					
					for (unsigned int i = 0; i < _programs.size(); i++) {
						delete _programs[i];
					}
				};
			
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
				
				std::vector<GLuint> _shader_list;
				std::vector<GLuint> _program_list;
				std::vector<program::Program *> _programs;
				
				program::Program *_bitmap_fonts_program;
			
			public:
				void compileShader(GLenum eShaderType, 
												const std::string &strShader);
				void compileShaderFile(GLenum eShaderType, 
												const std::string &filename);
				program::Program *buildProgram();
				program::Program *getBitmapFontsProgram();
				
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