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
	
	program::Program *prog = ldr->getProgram("data/glsl/test.vert", 
												"data/glsl/test.frag");

	core::Texture *texture = ldr->createTexture("data/Bench_2K_Diffuse.png");
	
	monkey = ldr->getMesh("data/bench.mm");
	monkeyMat = monkey->mat;//ldr->getNewMaterial();
	monkeyMat->setProgram(prog);
	//monkeyMat->setCullMode(GL_NONE);
	monkeyMat->setDepthTest(true);
	monkeyMat->setDepthWrite(true);
	monkeyMat->setBlending(true);
	//monkeyMat->setPolyMode(GL_LINE);
	monkeyMat->setTexture(texture);
	//monkey->opacity = 0.5f;
	/*if (monkey != 0) {
		monkey->setMaterial(monkeyMat);
	}*/
	
	// setup per vertex colors
	monkey->vertexColors = false;
	
	// should be deleted when app end
	core::Geometry *monkeyGeom = monkey->cloneGeometry(); 
	monkey->setGeometry(monkeyGeom);
	//monkeyGeom->setDynamic(core::GEOMETRY_VERTICE);
	//monkeyGeom->setDynamic(core::GEOMETRY_COLOR);
	
	rndr->add(monkey);

	rndr->setViewport(screenWidth, screenHeight);
	
	camera::Camera *cam = new camera::Camera(camera::PERSPECTIVE, 75, screenWidth / screenHeight);
	rndr->setCamera(cam);
	
	//monkey->rx = core::math::deg2rad(-80);
	monkey->z = -300.5f;
	monkey->y = -150.5f;
	
	float colorChange = 0.0f;
	float verticeChange = 0.0;
	float colorVelocity = 1.0f;
	
	do {
		double deltaTime = screen->getDeltaTime();
		
		if (screen->isActive()) {
			rndr->clear();
			
			colorChange += colorVelocity * deltaTime;
			verticeChange += 1.0f * deltaTime;
			if (colorChange >= 1.0f || colorChange <= -1.0f)
				colorVelocity = -colorVelocity;
			
			//monkey->rx +=1.4f* deltaTime;	
			//monkey->ry +=2.4f* deltaTime;	
			monkey->ry +=colorChange/200;
			monkey->rz +=colorChange/200;
			//monkey->z -= (-colorChange)/50;
			
			// change colors
			monkeyGeom->colors.clear();
			for (unsigned int i=0; i<monkeyGeom->vertices.size(); i+=3) {
				float v1 = monkeyGeom->vertices[i];
				float v2 = monkeyGeom->vertices[i+1];
				float v3 = monkeyGeom->vertices[i+2];
				float rcolor = sinf(v1+colorChange/2);
				float gcolor = cosf(v2+colorChange/2);
				float bcolor = sinf(v3+colorChange/2);
				
				rcolor = core::math::clamp(rcolor, 0.0, 1.0);
				gcolor = core::math::clamp(gcolor, 0.0, 1.0);
				bcolor = core::math::clamp(bcolor, 0.0, 1.0);
				//monkeyGeom->vertices[i] += sinf(v1+verticeChange)/200;
				//monkeyGeom->vertices[i+1] += cosf(monkeyGeom->vertices[i+1]+verticeChange)/200;
				//monkeyGeom->vertices[i+2] += sinf(monkeyGeom->vertices[i+2]+verticeChange)/200;
				
				monkeyGeom->colors.push_back(rcolor);
				monkeyGeom->colors.push_back(gcolor);
				monkeyGeom->colors.push_back(bcolor);
			}
			
			rndr->render();
		} else {
			screen->sleep();
		}
		
		screen->swapBuffers();
	} while(screen->running());

	delete monkeyGeom;
	
	audioManager->free();
	ldr->free();
	rndr->free();
	screen->free();
	
	delete cam;
}