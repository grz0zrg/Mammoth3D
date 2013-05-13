#define GLFW_INCLUDE_GL3

#include <cmath>
#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>

#include "mammoth3d.hpp"

material::Material *monkeyMat;
object::Mesh *monkey = 0;
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
	screen->displayMouseCursor(false);
	
	rndr = renderer::Renderer::getInstance();
	
	audio::Audio *audioManager = audio::Audio::getInstance();
	audioManager->loadMusic("data/music/lithography.ogg");
	//audioManager->playMusic();
	
	ldr = loader::Loader::getInstance();
	
	GLuint program = ldr->loadProgram("data/glsl/test.vert", "data/glsl/test.frag");
	monkey = ldr->loadMesh("data/Cube.mm");
	monkeyMat = new material::Material();
	monkeyMat->setProgram(program);
	//monkeyMat->setPolyMode(GL_LINE);
	if (monkey != 0) {
		monkey->setMaterial(monkeyMat);
	}
	rndr->add(monkey);

	rndr->setViewport(screen->getWidth(), screen->getHeight());
	
	do {
		double deltaTime = screen->getDeltaTime();
		
		if (screen->isActive()) {
			rndr->clear();
			
			monkey->x += 0.1f * deltaTime;
			
			rndr->render();
		} else {
			screen->sleep();
		}
		
		screen->swapBuffers();
	} while(screen->running());

	audioManager->free();
	ldr->free();
	rndr->free();
	screen->free();

	delete monkey;
	delete monkeyMat;
}