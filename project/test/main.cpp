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
	shaderloader->compileShaderFile(GL_VERTEX_SHADER, "data/glsl/test.vert");
	shaderloader->compileShaderFile(GL_FRAGMENT_SHADER, "data/glsl/test.frag");
	theProgram = shaderloader->buildProgram();

	monkey = new loader::MeshLoader("data/BlenderMonkey.mm");
	monkey->setProgram(theProgram);
	monkey->setPolyMode(GL_LINE);

	rndr->setViewport(screen->getWindowWidth(), screen->getWindowHeight());
	
	do {
		rndr->clear();
		
		rndr->render(monkey);

		screen->swapBuffers();
	} while(screen->running());

	audioManager->free();
	rndr->free();
	screen->free;
	shaderloader->free();

	delete monkey;
}