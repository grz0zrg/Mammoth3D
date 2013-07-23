#ifndef RENDERER_HPP
#define RENDERER_HPP

	#include <GL/glew.h>
	
	#include <string>
	#include <iostream>
	
	#include "../core/fbo.hpp"
	
	#include "../cameras/camera.hpp"
	#include "../objects/mesh.hpp"

	namespace renderer {
		typedef enum {
			DEFAULT = 0
		}RenderTarget;

		class Renderer {
			private:
				class RenderObjects {
					public:
						RenderObjects() {fbo = (core::Fbo *)DEFAULT;}

						void addMesh(object::Mesh *mesh) { meshs.push_back(mesh); }

						void setFbo(core::Fbo *fbo) { this->fbo = fbo; }

						core::Fbo *fbo;

						std::vector<object::Mesh *> meshs;
				};

				Renderer() {
					GLenum err = glewInit();
					if (err != GLEW_OK) {
						log((const char *)glewGetErrorString(err));
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
					viewportWidth = viewportHeight = 0;
					
					setTarget((core::Fbo *)DEFAULT);
					lastFbo = (core::Fbo *)DEFAULT;
				}
				
				~Renderer() {
					for (unsigned int i = 0; i < render_queue.size(); i++) {
						for (unsigned int e = 0; e < render_queue[i].size(); e++) {
							delete render_queue[i][e];
						}
					}
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
				
				std::vector<std::vector<RenderObjects *> > render_queue;
				std::vector<material::Material *> mats; // all mats
				std::vector<core::Fbo *> fbos; // all render targets
				
				material::Material *previousMat;
				camera::Camera *currCamera;
				unsigned int target;
				
				core::Fbo *lastFbo;
				
				int viewportWidth, viewportHeight;

			public:
				void setViewport(GLsizei w, GLsizei h) {
					if (currCamera) {
						currCamera->aspect = w / (float) h;
						currCamera->buildProjection();
					}
					
					viewportWidth  = w;
					viewportHeight = h;
					
					glViewport(0, 0, viewportWidth, viewportHeight);
				}
				
				void clear() {
					glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
					glClearDepth(1.0f);
					glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
				}
				
				void setCamera(camera::Camera *camera) {
					currCamera = camera;
				}
				
				void setTarget(int fbo) {
					setTarget((core::Fbo *) fbo);
				}
				
				void setTarget(core::Fbo *fbo) {
					bool found = false;
					for (unsigned int i = 0; i < fbos.size(); i++) {
						if (fbos[i] == fbo) {
							target = i;
							return;
						}
					}
					
					fbos.push_back(fbo);
					target = fbos.size()-1;
				}
				
				// add to render queue
				// TODO: remove() and ignore multiples adds
				void add(object::Mesh *mesh) {
					bool found = false;
					for (unsigned int i = 0; i < mats.size(); i++) {
						if (mats[i] == mesh->mat) {
							RenderObjects *ros = render_queue[i][render_queue[i].size()-1];
							ros->setFbo(fbos[target]);
							ros->addMesh(mesh);
							found = true;
							break;
						}
					}
					
					if (!found || mats.empty()) {
						mats.push_back(mesh->mat);
						std::vector<RenderObjects *> roslist;
						render_queue.push_back(roslist);
						
						render_queue[mats.size()-1].push_back(new RenderObjects());
						RenderObjects *ros = render_queue[mats.size()-1][render_queue[mats.size()-1].size()-1];
						ros->setFbo(fbos[target]);
						ros->addMesh(mesh);
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