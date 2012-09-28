#define GLFW_INCLUDE_GL3

#include <cmath>
#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>

#include "core/vector2.hpp"
#include "core/vector3.hpp"
#include "audio/audio.hpp"

#include "loader/collada/collada.hpp"

#include <GL/glew.h>
#include <GL/glfw.h>

GLuint CreateShader(GLenum eShaderType, const std::string &strShaderFile)
{
	GLuint shader = glCreateShader(eShaderType);
	const char *strFileData = strShaderFile.c_str();
	glShaderSource(shader, 1, &strFileData, NULL);

	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

		const char *strShaderType = NULL;
		switch(eShaderType)
		{
		case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
		case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
		case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
		}

		fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
		delete[] strInfoLog;
	}

	return shader;
}

GLuint CreateProgram(const std::vector<GLuint> &shaderList)
{
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
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
	}

	for(size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
		glDetachShader(program, shaderList[iLoop]);

	return program;
}

GLuint theProgram;

const std::string strVertexShader(
	"#version 330\n"
	"layout(location = 0) in vec4 position;\n"
	"void main()\n"
	"{\n"
	"   gl_Position = position;\n"
	"}\n"
);

const std::string strFragmentShader(
	"#version 330\n"
	"out vec4 outputColor;\n"
	"void main()\n"
	"{\n"
	"   outputColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
	"}\n"
);

void InitializeProgram()
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(CreateShader(GL_VERTEX_SHADER, strVertexShader));
	shaderList.push_back(CreateShader(GL_FRAGMENT_SHADER, strFragmentShader));

	theProgram = CreateProgram(shaderList);

	std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
}

const float vertexPositions[] = {
	0.75f, 0.75f, 0.0f, 1.0f,
	0.75f, -0.75f, 0.0f, 1.0f,
	-0.75f, -0.75f, 0.0f, 1.0f,
};

GLuint positionBufferObject;
GLuint vao;


void InitializeVertexBuffer()
{
	glGenBuffers(1, &positionBufferObject);

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void init()
{
	InitializeProgram();
	InitializeVertexBuffer();

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
}

void display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(theProgram);

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(0);
	glUseProgram(0);

	glfwSwapBuffers();
}

void reshape (int w, int h)
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	  case 0:
		  return;
	}
}

int main(int argc, char **argv) {
	if (glfwInit() != GL_TRUE)
		return 0;

	if (glfwOpenWindow(800, 600, 5, 6, 5, 0, 0, 0, GLFW_WINDOW) != GL_TRUE) {
		glfwTerminate();
		return 0;
	}
		
	glfwSetWindowTitle("Mammoth3D");
	
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cout << "Error: " << glewGetErrorString(err) << std::endl;
	}
	std::cout << "Status: Using GLEW: " << glewGetString(GLEW_VERSION) << std::endl;

	if (GLEW_VERSION_2_0) {
		std::cout << "OpenGL 2.0 supported! " << std::endl;
	}
	if (GLEW_VERSION_2_1) {
		std::cout << "OpenGL 2.1 supported! " << std::endl;
	}
	if (GLEW_VERSION_3_0) {
		std::cout << "OpenGL 3.0 supported! " << std::endl;
	}
	if (GLEW_VERSION_3_1) {
		std::cout << "OpenGL 3.1 supported! " << std::endl;
	}
	if (GLEW_VERSION_3_2) {
		std::cout << "OpenGL 3.2 supported! " << std::endl;
	}
	if (GLEW_VERSION_3_3) {
		std::cout << "OpenGL 3.3 supported! " << std::endl;
	}
	
	audio::Audio *audioManager = audio::Audio::getInstance();
	audioManager->loadMusic("../../../project/test/data/music/lithography.ogg");
	audioManager->playMusic();
	
	loader::Collada *cola = new loader::Collada("../../../project/test/data/blender_monkey.dae");

	init();
	reshape(800, 600);
	while(glfwGetKey( GLFW_KEY_ESC ) == GLFW_RELEASE) {
		display();
	}
	std::cout << "OpenGL 3.3 supported! " << std::endl;
	audioManager->kill();

	delete cola;
	glfwTerminate();
}