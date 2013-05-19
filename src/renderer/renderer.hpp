#ifndef RENDERER_HPP
#define RENDERER_HPP

	#include <GL/glew.h>
	
	#include <string>
	#include <iostream>
	
	#include "../cameras/camera.hpp"
	#include "../objects/mesh.hpp"

	namespace renderer {
		class Renderer {
			private:
				Renderer() {
					GLenum err = glewInit();
					if (err != GLEW_OK) {
						log("", glewGetErrorString(err));
					} else {
						logPretty("Vendor: ", glGetString(GL_VENDOR));
						logPretty("Renderer: ", glGetString(GL_RENDERER));
						logPretty("Version: ", glGetString(GL_VERSION));

						if (!GLEW_VERSION_3_0) {
							if (!GLEW_VERSION_4_0) {
								log("Incompatible OpenGL version."); 
							}
						}
					}
					
					glEnable(GL_POLYGON_SMOOTH);
					
					currCamera = 0;
				}
				
				~Renderer() {
				}
				
				template <typename T>
				void logPretty(const std::string &str, T param) {
					std::cout << "[Renderer] " << str << "\"" << 
								param << "\"" << std::endl;
				}
				
				template <typename T>
				void log(const std::string &str, T param) {
					std::cout << "[Renderer] " << str << param << std::endl;
				}
					
				void log(const char *str) {
					std::cout << "[Renderer] " << str << std::endl;
				}

				Renderer(const Renderer&);
				void operator=(const Renderer&);
				static Renderer *_singleton;
				
				std::vector<std::vector<object::Mesh *> > render_queue;
				std::vector<material::Material *> mats; // all mats
				
				material::Material *previousMat;
				camera::Camera *currCamera;

			public:
				void setViewport(GLsizei w, GLsizei h) {
					if (currCamera) {
						currCamera->aspect = w / h;
						currCamera->update();
					}
					
					glViewport(0, 0, w, h);
				}
				
				void clear() {
					glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
					glClearDepth(1.0f);
					glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
				}
				
				void setCamera(camera::Camera *camera) {
					currCamera = camera;
				}
				
				// add to render queue
				// TODO: remove() and ignore multiples adds
				void add(object::Mesh *mesh) {
					bool found = false;
					for (unsigned int i = 0; i < mats.size(); i++) {
						if (mats[i] == mesh->mat) {
							render_queue[i].push_back(mesh);
							found = true;
							break;
						}
					}
					
					if (!found || mats.empty()) {
						mats.push_back(mesh->mat);
						std::vector<object::Mesh *> mlist;
						render_queue.push_back(mlist);
						render_queue[mats.size()-1].push_back(mesh);
					}
				}
				
				void render();
				
				static Renderer *getInstance()
				{
					if (!_singleton) {
						_singleton =  new Renderer;
					}

					return _singleton;
				}

				static void free()
				{
					if (_singleton)
					{
						delete _singleton;
						_singleton = 0;
					}
				}
				
				GLuint previousProgram;
				GLenum previousPolyMode, previousCullMode;
				bool previousDepthWrite, previousDepthTest;
		};
	}
	
#endif