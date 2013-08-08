#include "mammoth3d.hpp"

renderer::Renderer *rndr = 0;

void windowResize(GLFWwindow* window, int width, int height)
{
	if (rndr) {
		rndr->setViewport(width, height);
	}
}

int main(int argc, char **argv) {
	int screenWidth = 800;
	int screenHeight = 600;
	
	window::Window *screen = window::Window::getInstance();
	screen->setFSAA(2);
	screen->openWindow(screenWidth, screenHeight);
	screen->setVSync();
	screen->hideMouseCursor();
	
	rndr = renderer::Renderer::getInstance();
	screen->onResize(windowResize);
	
	audio::Audio *audioManager = audio::Audio::getInstance();
	audioManager->loadMusic("data/music/Hear_the_voices_from_outer_space.ogg");

	loader::Loader *ldr = loader::Loader::getInstance();
	
	program::Program *cloud_p = ldr->getProgram("data/glsl/cloud.vert", 
												"data/glsl/cloud.frag");
												
	program::Program *background_p = ldr->getProgram("data/glsl/background.vert", 
												"data/glsl/background.frag");
												
	program::Program *kaleido_p = ldr->getProgram("data/glsl/kaleido.vert", 
												"data/glsl/kaleido.frag");
								
	core::Texture *cloud_t = ldr->createTexture("data/cloud10.png");
	core::Texture *gradient_t = ldr->createTexture("data/gradient.png");
	core::Texture *fbo_t = ldr->createTexture("data/texture.png");
	
	font::BitmapFont *font = new font::BitmapFont("data/font.png");
	
	object::BitmapText *text = new object::BitmapText(font);
	text->setText("Hello World!");
	
	scenegraph::MeshNode *cloud_n = new scenegraph::MeshNode();
	
	object::Quad *background_m = new object::Quad(true);
	background_m->mat->setProgram(background_p);
	background_m->mat->setTexture(gradient_t);
	
	core::UniformBlock *ublock1 = new core::UniformBlock("fx");
	ublock1->setUniform("uv_multiplier");
	ublock1->setUniform("time");
	ublock1->initialize();
	
	kaleido_p->bindUniformBlock(ublock1);
	
	object::Quad *kaleido_m = new object::Quad(true);
	kaleido_m->mat->setProgram(kaleido_p);
	/*kaleido_m->mat->setBlending(true);
	kaleido_m->mat->setDepthTest(false);
	kaleido_m->mat->setDepthWrite(false);
	kaleido_m->opacity = 0.75f;
		*/
	core::Fbo *fbo = new core::Fbo(kaleido_m->screen_aligned_texture);
	
	unsigned int cloud_count = 512;
	object::Quad *clouds_m[cloud_count];
	for (unsigned int i = 0; i < cloud_count; i++) {
		object::Quad *cloud_m = new object::Quad();
		cloud_m->mat->setTexture(cloud_t);
		cloud_m->mat->setProgram(cloud_p);
		cloud_m->x = glm::linearRand(-10.0f, 10.0f);
		cloud_m->y = glm::linearRand(-0.95f, -2.75f);
		cloud_m->z = -(float)i/32.0f;
		cloud_m->rz = (glm::linearRand(-1.0f, 1.0f) * M_PI) * 90;
		
		cloud_m->sx = cloud_m->sy = glm::linearRand(0.0f, 1.0f) * glm::linearRand(0.0f, 1.0f) * 1.5f + 0.5f;
		
		cloud_m->mat->setBlending(true);
		cloud_m->mat->setDepthTest(false);
		cloud_m->mat->setDepthWrite(false);
		cloud_m->opacity = glm::linearRand(0.0f, 0.85f);
		
		//cloud_n->addMesh(cloud_m);
		
		clouds_m[i] = cloud_m;
	}
	
	core::Geometry *geom = new core::Geometry();
	for (unsigned int i = 0; i < cloud_count; i++) {
		object::Quad *cloud_m1 = new object::Quad();
		cloud_m1->x = glm::linearRand(-10.0f, 10.0f);
		cloud_m1->y = glm::linearRand(-0.95f, -2.75f);
		cloud_m1->z = -(float)i/32.0f;
		cloud_m1->rz = (glm::linearRand(-1.0f, 1.0f) * M_PI) * 90;
		cloud_m1->sx = cloud_m1->sy = glm::linearRand(0.0f, 1.0f) * glm::linearRand(0.0f, 1.0f) * 1.5f + 0.5f;
		
		core::geometryutils::merge(geom, cloud_m1);
		delete cloud_m1;
	}
	geom->update();

	object::Mesh *cloud_f = new object::Mesh();
	cloud_f->setGeometry(geom);
	cloud_f->setMaterial(ldr->getNewMaterial());
	cloud_f->mat->setTexture(cloud_t);
	cloud_f->mat->setProgram(cloud_p);
	cloud_f->mat->setCullMode(GL_NONE);
	//cloud_f->mat->setPolyMode(GL_LINE);

	cloud_f->mat->setBlending(true);
	cloud_f->mat->setDepthTest(false);
	cloud_f->mat->setDepthWrite(false);
	cloud_f->opacity = 0.45f;//glm::linearRand(0.0f, 0.85f);
		
	cloud_n->addMesh(cloud_f);

	rndr->setViewport(screenWidth, screenHeight);
	
	camera::Camera *cam = new camera::Camera(camera::PERSPECTIVE, 75, screenWidth / screenHeight);
	rndr->setCamera(cam);

	rndr->setRenderTarget(fbo);
	rndr->add(background_m);
	rndr->add(cloud_n);
	
	rndr->setRenderTarget(renderer::DEFAULT);
	rndr->add(kaleido_m);
	rndr->add(text);

	audioManager->playMusic();

	float aperture = 0.75;
	float uv_multiplier = 1.0f;
	float time = 0.0f;
	
	sync::SyncTracker *stracker = sync::SyncTracker::getInstance();
	stracker->setBPM(115);
	stracker->setLPB(4);
	//stracker->load("sync.mms");
	stracker->setTrack("time", &time);
	stracker->setTrack("aperture", &aperture);
	stracker->setTrack("uv_multiplier", &uv_multiplier);
	
	sync::SyncTrackerXRNS *xrnstracker = sync::SyncTrackerXRNS::getInstance();
	xrnstracker->parseSong("Song.xml");
	
	sync::SyncTrackerController *ctracker = sync::SyncTrackerController::getInstance();
	
	float abc = 0.0f;
	
	do {
		double deltaTime = screen->getDeltaTime();
		
		double songTime = audioManager->getSongTime();
		double songDTime = songTime * 0.075;
		uv_multiplier+=0.01f;
		ublock1->setUniform("uv_multiplier", uv_multiplier);
		ublock1->setUniform("time", deltaTime);
		ublock1->update();
		
/*
		for (unsigned int i = 0; i < cloud_count; i++) {
			clouds_m[i]->z += 0.5f * deltaTime;
			clouds_m[i]->opacity += 0.09f * deltaTime;
			
			if (clouds_m[i]->opacity > 0.85f) {
				clouds_m[i]->opacity = 0.85f;
			}
			
			if (clouds_m[i]->z > 0) {
				clouds_m[i]->x = glm::linearRand(-10.0f, 10.0f);
				clouds_m[i]->y = glm::linearRand(-0.75f, -2.0f);
				clouds_m[i]->z = glm::linearRand(-10.0f, -30.0f);
				clouds_m[i]->rz = (glm::linearRand(-1.0f, 1.0f) * M_PI) * 90;
				clouds_m[i]->sx = clouds_m[i]->sy = glm::linearRand(0.0f, 1.0f) * glm::linearRand(0.0f, 1.0f) * 2.5f + 0.5f;
		
				clouds_m[i]->opacity = 0.0f;
			}
		}
	*/	
		abc=songDTime;//+=0.09f*songDeltaTime;
		//std::cout << abc << std::endl;
		cam->lookAt(cos(abc), -2.5f+sin(abc)*5.0f, -3.0+cos(abc)*3, 0.0f, -0.25f+sin(abc), -10.0f, -1.15f+cos(abc)*0.75f, 1.0f, sin(abc)*0.25f);
			
		//stracker->update(songTime);
		
		#ifdef DEBUG
		ctracker->update();
		#endif
		
		rndr->render();
		
		screen->swapBuffers();
	} while(screen->running() && !audioManager->isMusicFinished());

	for (unsigned int i = 0; i < cloud_count; i++) {
		delete clouds_m[i];
	}
	
	delete ublock1;
	delete fbo;
	delete kaleido_m;
	delete background_m;
	delete cloud_n;
	delete font;
	delete text;

	stracker->free();
	ctracker->free();
	audioManager->free();
	ldr->free();
	rndr->free();
	screen->free();
	
	delete cam;
	
	return 0;
}