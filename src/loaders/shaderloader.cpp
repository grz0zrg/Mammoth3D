#include "shaderloader.hpp"

loader::ShaderLoader *loader::ShaderLoader::_singleton = 0;

void loader::ShaderLoader::compileShader(GLenum eShaderType, 
										const std::string &strShader) {
	GLuint shader = glCreateShader(eShaderType);
	
	if (shader == 0) {
		log("glCreateShader failed");
		return;
	}
	
	const char *shaderData = strShader.c_str();
	glShaderSource(shader, 1, &shaderData, 0);

	glCompileShader(shader);

	GLint infoLogLength;
	
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, 0, strInfoLog);

		log("Compile failure: ", strInfoLog);
		
		delete[] strInfoLog;
		glDeleteShader(shader);
		
		return;
	} else { // check log for warning
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 1) {
			GLchar *strInfoLog = new GLchar[infoLogLength + 1];
			
			if (strcmp(strInfoLog, "No errors.") != 0) {
				glGetShaderInfoLog(shader, infoLogLength, 0, strInfoLog);
				
				log(strInfoLog);

				delete[] strInfoLog;
			}
		}
	}
	
	_shader_list.push_back(shader);
}

void loader::ShaderLoader::compileShaderFile(GLenum eShaderType, 
										const std::string &filename) {
	std::fstream file(filename.c_str(), std::ios::in);
	if (file.is_open()) {
		std::stringstream buffer;
		buffer << file.rdbuf();
		
		log("loading: ", filename);
		
		compileShader(eShaderType, buffer.str().c_str());
		file.close();
	} else {
		log("Cannot open: ", filename);
	}
}

program::Program *loader::ShaderLoader::buildProgram() {
	GLuint prog = glCreateProgram();

	if (prog == 0) {
		log("glCreateProgram failed");
		return 0;
	}

	for(size_t i = 0; i < _shader_list.size(); i++) {
		glAttachShader(prog, _shader_list[i]);
		if (glGetError() == GL_INVALID_VALUE || 
			glGetError() == GL_INVALID_OPERATION) {
			log("glAttachShader failed");
		}
	}

	glLinkProgram(prog);

	GLint status;
	glGetProgramiv(prog, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint infoLogLength;
		glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(prog, infoLogLength, NULL, strInfoLog);
		log("Linker failure: ", strInfoLog);
		delete[] strInfoLog;
		
		glDeleteProgram(prog);
		
		return 0;
	}

	_program_list.push_back(prog);

	for(size_t i = 0; i < _shader_list.size(); i++) {
		GLuint shader = _shader_list[i];
		glDetachShader(prog, shader);
		glDeleteShader(shader);
	}

	_shader_list.clear();
	
	program::Program *p = new program::Program(prog);
	_programs.push_back(p);
	
	return p;
}

program::Program *loader::ShaderLoader::getBitmapFontsProgram() {
	if (_bitmap_fonts_program == 0) {
		compileShader(GL_VERTEX_SHADER, builtinshaders::bitmapFontsVertexShader);
		compileShader(GL_FRAGMENT_SHADER, builtinshaders::bitmapFontsFragmentShader);
		
		_bitmap_fonts_program = buildProgram();
		
		_bitmap_fonts_program->registerUniform("chars_pos");
	}
	
	return _bitmap_fonts_program;
}