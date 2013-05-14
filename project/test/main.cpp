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
renderer::Renderer *rndr = 0;
loader::Loader *ldr;

void GLFWCALL windowResize(int width, int height)
{
	if (rndr) {
		rndr->setViewport(width, height);
	}
}

int main(int argc, char **argv) {
	int screenWidth = 800;
	int screenHeight = 600;
	
	window::Window *screen = window::Window::getInstance();
	screen->setFSAA(4);
	screen->openWindow(screenWidth, screenHeight);
	screen->setVSync();
	screen->displayMouseCursor(false);
	
	rndr = renderer::Renderer::getInstance();
	screen->onResize(windowResize);
	
	audio::Audio *audioManager = audio::Audio::getInstance();
	audioManager->loadMusic("data/music/lithography.ogg");
	//audioManager->playMusic();
	
	ldr = loader::Loader::getInstance();
	
	program::Program *prog = ldr->loadProgram("data/glsl/test.vert", "data/glsl/test.frag");
	monkey = ldr->loadMesh("data/Cube.mm");
	monkeyMat = new material::Material();
	monkeyMat->setProgram(prog);
	//monkeyMat->setCullMode(GL_NONE);
	monkeyMat->setDepthTest(true);
	monkeyMat->setDepthWrite(true);
	//monkeyMat->setBlending(true);
	//monkeyMat->setPolyMode(GL_LINE);
	if (monkey != 0) {
		monkey->setMaterial(monkeyMat);
	}
	rndr->add(monkey);

	rndr->setViewport(screenWidth, screenHeight);
	
	camera::Camera *cam = new camera::Camera(camera::PERSPECTIVE, 75, screenWidth / screenHeight);
	rndr->setCamera(cam);
	
	do {
		double deltaTime = screen->getDeltaTime();
		
		if (screen->isActive()) {
			rndr->clear();
			
			monkey->ry += 0.1f * deltaTime;
			
			rndr->render();
		} else {
			screen->sleep();
		}
		
		screen->swapBuffers();
	} while(screen->running());

	delete prog;
	
	audioManager->free();
	ldr->free();
	rndr->free();
	screen->free();

	delete cam;
	delete monkey;
	delete monkeyMat;
}