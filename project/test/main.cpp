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
	
	// setup per vertex colors
	monkey->vertexColors = true;
	monkey->colors.resize(monkey->vertices.size(), 0.0f);
					
	for (unsigned int i=0; i<monkey->colors.size(); i+=3) {
		float rcolor = (float)rand()/(float)RAND_MAX;
		float gcolor = (float)rand()/(float)RAND_MAX;
		float bcolor = (float)rand()/(float)RAND_MAX;
		rcolor = sin(monkey->vertices[i]);
		gcolor = cos(monkey->vertices[i+1]);
		bcolor = sin(monkey->vertices[i+2]);
		monkey->colors[i] = rcolor;
		monkey->colors[i+1] = gcolor;
		monkey->colors[i+2] = bcolor;
	}
	monkey->update(object::COLOR_BUFFER); // update color buffer
	
	rndr->add(monkey);

	rndr->setViewport(screenWidth, screenHeight);
	
	camera::Camera *cam = new camera::Camera(camera::PERSPECTIVE, 75, screenWidth / screenHeight);
	rndr->setCamera(cam);
	
	monkey->rx = core::math::deg2rad(-80);
	monkey->z = -4.0f;
	
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