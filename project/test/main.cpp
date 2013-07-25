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
												
	program::Program *prog_textured = ldr->getProgram("data/glsl/textured.vert", 
												"data/glsl/textured.frag");
												
	program::Program *post_process = ldr->getProgram("data/glsl/postprocess.vert", 
												"data/glsl/postprocess.frag");
												
	program::Program *post_process2 = ldr->getProgram("data/glsl/postprocess2.vert", 
												"data/glsl/postprocess2.frag");

	core::Texture *texture = ldr->createTexture("data/texture2.png");
	core::Texture *texture2 = ldr->createTexture("data/texture.png");
	core::Texture *mammothtexture = ldr->createTexture("data/mammoth3d.png");

	monkey = ldr->getMesh("data/monkey_colored.mm");
	monkeyMat = monkey->mat;//ldr->getNewMaterial();
	monkeyMat->setProgram(prog);
	monkeyMat->setCullMode(GL_NONE);
	monkeyMat->setDepthTest(true);
	monkeyMat->setDepthWrite(true);
	monkeyMat->setBlending(true);
	//monkeyMat->setPolyMode(GL_LINE);
	//monkeyMat->setTexture(texture);
	monkey->opacity = 0.05f;
	/*if (monkey != 0) {
		monkey->setMaterial(monkeyMat);
	}*/
	
	// setup per vertex colors
	monkey->vertexColors = true;
	
	// should be deleted when app end
	core::Geometry *monkeyGeom = monkey->cloneGeometry(); 
	//monkey->setGeometry(monkeyGeom);
	//monkeyGeom->setDynamic(core::GEOMETRY_VERTICE);
	//monkeyGeom->setDynamic(core::GEOMETRY_COLOR);

	rndr->setViewport(screenWidth, screenHeight);
	
	camera::Camera *cam = new camera::Camera(camera::PERSPECTIVE, 75, screenWidth / screenHeight);
	rndr->setCamera(cam);
	
	//monkey->rx = core::math::deg2rad(-80);
	monkey->z = -5.5f;
	
	object::Quad *screen_aligned_quad = new object::Quad(true);
	core::Fbo *fbo = new core::Fbo(screen_aligned_quad->screen_aligned_texture);
	screen_aligned_quad->mat->setProgram(post_process);
	
	object::Quad *screen_aligned_quad4 = new object::Quad(true);
	core::Fbo *fbo2 = new core::Fbo(screen_aligned_quad4->screen_aligned_texture);
	screen_aligned_quad4->mat->setProgram(post_process2);
	screen_aligned_quad4->mat->setDepthWrite(false);
	screen_aligned_quad4->mat->setBlending(true);
	screen_aligned_quad4->mat->setBlendFunc(GL_ONE_MINUS_CONSTANT_COLOR, GL_ONE);
	screen_aligned_quad4->opacity = 1.0f;
	
	float uv = 0.0f;
	post_process2->registerDynamicUniform1f("uv_add", &uv);
	
	object::Quad *screen_aligned_quad2 = new object::Quad(true);
	screen_aligned_quad2->mat->setProgram(post_process);
	screen_aligned_quad2->mat->setTexture(texture2);
	screen_aligned_quad2->mat->setDepthWrite(false);
	screen_aligned_quad2->mat->setBlending(true);
	screen_aligned_quad2->opacity = 0.5f;
	
	object::Quad *screen_aligned_quad3 = new object::Quad(true);
	screen_aligned_quad3->mat->setProgram(post_process2);
	screen_aligned_quad3->mat->setTexture(texture);
	screen_aligned_quad3->mat->setDepthWrite(false);
	screen_aligned_quad3->mat->setBlending(true);
	screen_aligned_quad3->mat->setBlendFunc(GL_ONE_MINUS_CONSTANT_COLOR, GL_ONE_MINUS_SRC_COLOR);
	screen_aligned_quad3->mat->setBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
	screen_aligned_quad3->opacity = 1.0f;
	
	object::Quad *quad1 = new object::Quad();
	quad1->mat->setProgram(prog_textured);
	quad1->mat->setTexture(mammothtexture);
	quad1->mat->setCullMode(GL_NONE);
	quad1->z = -100.0f;
	
	post_process->registerDynamicUniform1f("uv_add", &uv);
	
	rndr->setTarget(fbo);
	rndr->add(monkey);
	
	rndr->setTarget(fbo2);
	rndr->add(screen_aligned_quad);
	rndr->add(screen_aligned_quad2);
	
	rndr->setTarget(renderer::DEFAULT);
	rndr->add(screen_aligned_quad4);
	rndr->add(screen_aligned_quad3);
	rndr->add(quad1);
	
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
			monkey->ry +=colorChange;
			monkey->rz +=colorChange;
			//monkey->z -= (-colorChange)/50;
			
			uv += colorChange/200;
			
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
				monkeyGeom->colors.push_back(bcolor);
			}
			
			rndr->render();
		} else {
			screen->sleep();
		}
		
		screen->swapBuffers();
	} while(screen->running());

	//delete fbo;
	//delete screen_aligned_quad;
	delete monkeyGeom;
	
	audioManager->free();
	ldr->free();
	rndr->free();
	screen->free();
	
	delete cam;
}