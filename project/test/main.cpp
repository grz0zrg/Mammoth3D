#include "mammoth3d.hpp"

void windowResize(GLFWwindow* window, int width, int height)
{
    auto rndr = renderer::Renderer::getInstance();
    
    rndr->setViewport(width, height);
}

int main(int argc, char **argv) {
    int screenWidth = 800;
    int screenHeight = 600;
    
    auto app = window::Window::getInstance();
    app->open("example", screenWidth, screenHeight, false, 2);

    auto rndr = renderer::Renderer::getInstance();
    rndr->setGammaCorrection(true);
    app->onResize(windowResize);
    
    auto audioManager = audio::Audio::getInstance();
    audioManager->loadMusic("data/music/generic_house_demo.ogg");

    auto ldr = loader::Loader::getInstance();
    
    auto simple_p = ldr->getProgram("data/glsl/simple.vert", 
                                                "data/glsl/simple.frag");

    auto star_t = ldr->getTexture("data/star.png");
    
    auto text = ldr->getNewBitmapText("data/font.png", "", 255, 255, 255, 0.5f);
    //text->z = -5.0f;
    
    auto stars_n = ldr->getNewMeshNode();

    auto ublock1 = ldr->getNewUniformBlock("Params", {{"aperture", 1.0f}}, simple_p);

    //unsigned int star_count = 256;
    
    /*auto geom = ldr->getNewGeometry();
    for (unsigned int i = 0; i < star_count; i++) {
        auto star_m1 = new object::Quad();
        star_m1->_x = glm::linearRand(-40.0f, 40.0f);
        star_m1->_y = glm::linearRand(-40.0f, 40.0f);
        star_m1->_z = -(float)i/32.0f; //-(float)i/32.0f;
        star_m1->_rz = (glm::linearRand(-1.0f, 1.0f) * M_PI) * 90;
        star_m1->_sx = star_m1->_sy = glm::linearRand(0.0f, 1.0f) * glm::linearRand(0.0f, 1.0f) * 1.5f + 0.5f;
        
        core::geometryutils::merge(geom, star_m1);
        delete star_m1;
    }
    geom->update();
    */
	auto texture_buffer = ldr->getNewTextureBuffer(std::vector<float> {1.0f, 0.0f, 0.0f, 0.0f});

    auto star_f = ldr->getNewQuad({{star_t, 0}, {texture_buffer, 1}});
    star_f->_vertex_colors = true;
    auto star_meshs_instance = star_f->enableInstancing(256);
    
    auto star_f_colors = star_f->getGeometry();
    star_f_colors->setInstanced(core::GEOMETRY_COLOR);

    for (unsigned int i = 0; i < star_meshs_instance.size(); i++) {
        auto m = star_meshs_instance[i];
        
        m->_x = glm::linearRand(-40.0f, 40.0f);
        m->_y = glm::linearRand(-40.0f, 40.0f);
        m->_z = -(float)i/32.0f; //-(float)i/32.0f;
        //m->_rz = (glm::linearRand(-1.0f, 1.0f) * M_PI) * 90;
        m->_sx = m->_sy = glm::linearRand(0.0f, 1.0f) * glm::linearRand(0.0f, 1.0f) * 1.5f + 0.5f;
        
        for (unsigned int j = 0; j < 4; j++) {
            star_f_colors->_colors.push_back(glm::linearRand(0.0f, 1.0f));
        }
    }
    
    star_f_colors->update();
    
    //star_f->setGeometry(geom);
    //star_f->setMaterial(ldr->getNewMaterial());
    //star_f->setTexture(star_t);
    star_f->_mat->setCullMode(GL_NONE);
    //star_f->_mat->setPolyMode(GL_LINE);

    star_f->_mat->setBlending(true);
    star_f->_mat->setDepthTest(false);
    star_f->_mat->setDepthWrite(false);
    star_f->_opacity = 0.45f;//glm::linearRand(0.0f, 0.85f);

    stars_n->addMesh(star_f);
    
    stars_n->setProgram(simple_p);

    auto cam = ldr->getNewCamera(camera::PERSPECTIVE, 75, screenWidth / screenHeight);
    //camera::Camera *cam_ortho = new camera::Camera(camera::ORTHOGRAPHIC, -1.0f, 4.0f, 1.0f, -4.0f);
    rndr->setCamera(cam);

    //rndr->setRenderTarget();

    rndr->add(stars_n);
    rndr->add(text);

    audioManager->playMusic(true, 0.5f);

    float aperture = 30.75;
    float uv_multiplier = 1.0f;
    float time = 0.0f;
    
    auto stracker = sync::SyncTracker::getInstance();
    stracker->setBpm(115);
    stracker->setLpb(4);
    stracker->load("sync.mms");
    stracker->setTracks({{"time", &time}, {"aperture", &aperture}, {"uv_multiplier", &uv_multiplier}});
    //stracker->setTrack("time", &time);
    //stracker->setTrack("aperture", &aperture);
    //stracker->setTrack("uv_multiplier", &uv_multiplier);
    
    auto xrnstracker = sync::SyncTrackerXRNS::getInstance();
    //xrnstracker->parseSong("Song.xml");
    
    auto ctracker = sync::SyncTrackerController::getInstance();
    
    float abc = 0.0f;
    
    do {
        double deltaTime = app->getDeltaTime();
        
        double songTime = audioManager->getMusicTime();
        double songDTime = songTime * 0.075;
        uv_multiplier += 0.01f;
        
        text->setText("Fps:" + core::utils::toString(rndr->getFps()));
        
        //text->setAlphaTreshold(aperture);

        //ublocks1->set({{"aperture", aperture}, {"uv_multiplier", uv_multiplier}, {"time", deltaTime}});
//        ublock1->set("aperture", aperture);
        //ublock1->set("uv_multiplier", uv_multiplier);
        //ublock1->set("time", deltaTime);
//        ublock1->update();
        /*
        for (unsigned int i = 0; i < cloud_count; i++) {
            clouds_m[i]->_z += 0.5f * deltaTime;
            clouds_m[i]->_opacity += 0.09f * deltaTime;
            
            if (clouds_m[i]->_opacity > 0.85f) {
                clouds_m[i]->_opacity = 0.85f;
            }
            
            if (clouds_m[i]->_z > 0) {
                clouds_m[i]->_x = glm::linearRand(-10.0f, 10.0f);
                clouds_m[i]->_y = glm::linearRand(-0.75f, -2.0f);
                clouds_m[i]->_z = glm::linearRand(-10.0f, -30.0f);
                clouds_m[i]->_rz = (glm::linearRand(-1.0f, 1.0f) * M_PI) * 90;
                clouds_m[i]->_sx = clouds_m[i]->_sy = glm::linearRand(0.0f, 1.0f) * glm::linearRand(0.0f, 1.0f) * 2.5f + 0.5f;
        
                clouds_m[i]->_opacity = 0.0f;
            }
        }*/
    
        abc=songDTime;

        cam->lookAt(0, 0, -40, 0, 0, 0, 0, 1, 0);
        //cam->lookAt(cos(abc), -2.5f+sin(abc)*5.0f, -3.0+cos(abc)*3, 0.0f, -0.25f+sin(abc), -10.0f, -1.15f+cos(abc)*0.75f, 1.0f, sin(abc)*0.25f);
            
        stracker->update(songTime);
        
        #ifdef DEBUG
        ctracker->update();
        #endif
        
        rndr->render();
        
        app->swapBuffers();
    } while(app->running() && !audioManager->isMusicFinished());

    return 0;
}