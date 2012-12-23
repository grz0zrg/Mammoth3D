#include "shaderloader.hpp"

loader::ShaderLoader *loader::ShaderLoader::_singleton = 0;

void loader::ShaderLoader::compileShader(GLenum eShaderType, 
										const std::string &strShader) {
	GLuint shader = glCreateShader(eShaderType);
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
	} else {
		logPretty("Cannot open: ", filename);
	}
}

GLuint loader::ShaderLoader::buildProgram() {
	GLuint program = glCreateProgram();

	for(size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
		glAttachShader(program, shaderList[iLoop]);

	glLinkProgram(program);

	GLint status;
	glGetProgramiv (program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		log("Linker failure: ", strInfoLog);
		delete[] strInfoLog;
	}

	for(size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
		glDetachShader(program, shaderList[iLoop]);

	std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
	
	return program;
}
