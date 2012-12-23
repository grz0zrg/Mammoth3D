#define GLFW_INCLUDE_GL3

#include <cmath>
#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>

#include "mammoth3d.hpp"

material::Material *monkeyMat;
object::Mesh *monkey;
renderer::Renderer *rndr;
loader::Loader *ldr;

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
	
	ldr = loader::Loader::getInstance();
	
	GLuint program = ldr->loadProgram("data/glsl/test.vert", "data/glsl/test.frag");
	monkey = ldr->loadMesh("data/BlenderMonkey.mm");
	monkeyMat = new material::Material();
	monkeyMat->setProgram(program);
	monkeyMat->polyMode = GL_LINE;
	monkey->setMaterial(monkeyMat);

	rndr->setViewport(screen->getWindowWidth(), screen->getWindowHeight());
	
	do {
		rndr->clear();
		
		rndr->render(monkey);

		screen->swapBuffers();
	} while(screen->running());

	audioManager->free();
	ldr->free();
	rndr->free();
	screen->free;

	delete monkey;
	delete monkeyMat;
}