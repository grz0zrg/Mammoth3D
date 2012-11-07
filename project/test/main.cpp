#define GLFW_INCLUDE_GL3

#include <cmath>
#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>

#include "mammoth3d.hpp"

loader::MeshLoader *monkey;
renderer::Renderer *rndr;

GLuint theProgram;

const std::string strVertexShader(
	"#version 330\n"
	"layout(location = 0) in vec4 position;\n"
	"void main()\n"
	"{\n"
	"   vec4 totalOffset = vec4(0.0, 0.0, 0.0, 1.0);\n"
	"   gl_Position = position+totalOffset;\n"
	"}\n"
);

const std::string strFragmentShader(
	"#version 330\n"
	"out vec4 outputColor;\n"
	"void main()\n"
	"{\n"
	"	float lerpValue = gl_FragCoord.y / 600.0f;\n"
	"	outputColor = mix(vec4(1.0f, 1.0f, 1.0f, 1.0f), vec4(0.2f, 0.2f, 0.2f, 1.0f), lerpValue);\n"
	"}\n"
);

const float vertexPositions[] = {
	0.75f, 0.75f, 0.0f, 1.0f,
	0.75f, -0.75f, 0.0f, 1.0f,
	-0.75f, -0.75f, 0.0f, 1.0f,
};

GLuint positionBufferObject;
GLuint testIBO;

void InitializeVertexBuffer()
{
	glGenBuffers(1, &positionBufferObject);
	glGenBuffers(1, &testIBO);

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, monkey->vertices.size() * sizeof(float), &monkey->vertices.front(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, testIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, monkey->indices.size() * sizeof(unsigned int), &monkey->indices[0], GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void cleanup() {
	glDeleteBuffers(1, &positionBufferObject);
	glDeleteBuffers(1, &testIBO);
}

void display()
{
	glUseProgram(theProgram);

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, testIBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//glDrawArrays(GL_TRIANGLES, 0, 3);
	glDrawElements(GL_TRIANGLES, monkey->indices.size(), 
					GL_UNSIGNED_INT, (void*)0);
	
	glDisableVertexAttribArray(0);
	glUseProgram(0);
}

void GLFWCALL windowResize(int width, int height)
{
	rndr->setViewport(width, height);
}

int main(int argc, char **argv) {
	window::Window *screen = window::Window::getInstance();
	screen->setFSAA(4);
	screen->openWindow(800, 600);
	screen->onResize(windowResize);
	screen->setVSync();
	
	rndr = renderer::Renderer::getInstance();
	
	audio::Audio *audioManager = audio::Audio::getInstance();
	audioManager->loadMusic("data/music/lithography.ogg");
	audioManager->playMusic();
	
	loader::ShaderLoader *shaderloader = loader::ShaderLoader::getInstance();
	shaderloader->compileShader(GL_VERTEX_SHADER, strVertexShader);
	shaderloader->compileShader(GL_FRAGMENT_SHADER, strFragmentShader);
	theProgram = shaderloader->buildProgram();

	monkey = new loader::MeshLoader("data/BlenderMonkey.mm");
	
	InitializeVertexBuffer();
	rndr->setViewport(screen->getWindowWidth(), screen->getWindowHeight());
	
	do {
		rndr->clear();
		display();
		screen->swapBuffers();
	} while(screen->running());

	cleanup();

	audioManager->kill();
	rndr->kill();
	screen->kill;
	shaderloader->kill();

	delete monkey;
}