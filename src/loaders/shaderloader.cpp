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
	glShaderSource(shader, 1, &shaderData, NULL);

	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

		log("Compile failure: ", strInfoLog);
		delete[] strInfoLog;
		glDeleteShader(shader);
		
		return;
	}

	shaderList.push_back(shader);
}

void loader::ShaderLoader::compileShaderFile(GLenum eShaderType, 
										const std::string &filename) {
	std::fstream file(filename.c_str(), std::ios::in);
	if (file.is_open())
	{
		std::stringstream buffer;
		buffer << file.rdbuf();
		
		log("loading: ", filename);
		
		compileShader(eShaderType, buffer.str().c_str());
		file.close();
	} else {
		logPretty("Cannot open: ", filename);
	}
}

program::Program *loader::ShaderLoader::buildProgram() {
	GLuint prog = glCreateProgram();

	if (prog == 0) {
		log("glCreateProgram failed");
		return 0;
	}
	
	for(size_t i = 0; i < shaderList.size(); i++) {
		glAttachShader(prog, shaderList[i]);
		if (glGetError() == GL_INVALID_VALUE || 
			glGetError() == GL_INVALID_OPERATION) {
			log("glAttachShader failed");
		}
	}
		
	glLinkProgram(prog);

	GLint status;
	glGetProgramiv(prog, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(prog, infoLogLength, NULL, strInfoLog);
		log("Linker failure: ", strInfoLog);
		delete[] strInfoLog;
		
		glDeleteProgram(prog);
		
		return 0;
	}
	
	programList.push_back(prog);

	for(size_t i = 0; i < shaderList.size(); i++) {
		GLuint shader = shaderList[i];
		glDetachShader(prog, shader);
		//glDeleteShader(shader);
	}

	shaderList.clear();
	
	program::Program *p = new program::Program(prog);
	programs.push_back(p);
	
	return p;
}
