#ifndef MAMMOTH3D_RENDERER_HPP
#define MAMMOTH3D_RENDERER_HPP

	#include <GL/glew.h>
	
	#include <string>
	#include <iostream>
	
	#include "../core/fbo.hpp"
	
	#include "../cameras/camera.hpp"
	#include "../objects/mesh.hpp"
	#include "../objects/bitmaptext.hpp"
	#include "../scenegraph/node.hpp"
	#include "../scenegraph/meshnode.hpp"
	#include "../scenegraph/cameranode.hpp"

	namespace renderer {
		class Renderer {
			private:
				class Renderable {
					public:
						Renderable() {
							renderTarget = 0;
							
							root = new scenegraph::Node("root");
							
							meshNode = new scenegraph::MeshNode();
							cameraNode = new scenegraph::CameraNode();
							addNode(cameraNode);
							addNode(meshNode);
						}
						
						~Renderable() {
							delete root;
							delete meshNode;
							delete cameraNode;
						}

						void addNode(scenegraph::Node *node) { 
							root->add(node); 
						}
						
						void addMesh(object::Mesh *mesh) { 
							meshNode->addMesh(mesh);
						}
						
						void setCamera(camera::Camera *cam) { 
							cameraNode->setCamera(cam);
						}
						
						scenegraph::Node *getRoot() {
							return root;
						}

						core::Fbo *renderTarget;

						scenegraph::CameraNode *cameraNode;
						scenegraph::MeshNode *meshNode;
						scenegraph::Node *root;
				};

				Renderer() {
					glewExperimental = GL_TRUE; 
					GLenum err = glewInit();
					if (err != GLEW_OK) {
						log((const char *)glewGetErrorString(err));
					} else {
						log("Vendor: ", glGetString(GL_VENDOR));
						log("Renderer: ", glGetString(GL_RENDERER));
						log("Version: ", glGetString(GL_VERSION));

						if (!GLEW_VERSION_3_0) {
							if (!GLEW_VERSION_4_0) {
								log("Incompatible OpenGL version."); 
							}
						}
					}
					
					glEnable(GL_POLYGON_SMOOTH);
					glEnable(GL_LINE_SMOOTH);
					
					_curr_camera = 0;
					_viewport_width = _viewport_height = 0;
					
					_aspect_ratio = 0;
					
					setRenderTarget();
				}
				
				~Renderer() {
					for (unsigned int i = 0; i < _render_queue.size(); i++) {
						delete _render_queue[i];
					}
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
				
				std::vector<Renderable *> _render_queue;
				unsigned int _current_render_target_id;
				
				material::Material *_previous_mat;
				camera::Camera *_curr_camera;
				
				int _viewport_width, _viewport_height;
				
				float _aspect_ratio;

			public:
				void setViewport(GLsizei w, GLsizei h) {
					_aspect_ratio = w / (float) h;
					
					_viewport_width  = w;
					_viewport_height = h;
					
					glViewport(0, 0, _viewport_width, _viewport_height);
				}
				
				void setCamera(camera::Camera *camera) {
					_render_queue[_current_render_target_id]->setCamera(camera);
					
					_curr_camera = camera;
				}
				
				//void setRenderTarget(void *target) {
				//	setRenderTarget(static_cast<core::Fbo *>(target));
				//}
				
				void setRenderTarget(core::Fbo *render_target = 0) {
					bool found = false;
					for (unsigned int i = 0; i < _render_queue.size(); i++) {
						if (_render_queue[i]->renderTarget == render_target) {
							_current_render_target_id = i;
							return;
						}
					}
					
					Renderable *renderable = new Renderable();
					renderable->renderTarget = render_target;
					
					renderable->setCamera(_curr_camera);
					
					_render_queue.push_back(renderable);
					
					_current_render_target_id = _render_queue.size()-1;
				}
				
				void add(scenegraph::Node *node) {
					_render_queue[_current_render_target_id]->addNode(node);
				}
				
				void add(object::Mesh *mesh) {
					_render_queue[_current_render_target_id]->addMesh(mesh);
				}
				
				void remove(object::Mesh *mesh) {
					Renderable *currentRenderTarget = _render_queue[_current_render_target_id];
					scenegraph::MeshNode *meshNode = currentRenderTarget->meshNode;

					meshNode->removeMesh(mesh);
				}
				
				void remove(scenegraph::Node *node) {
					Renderable *currentRenderTarget = _render_queue[_current_render_target_id];
					scenegraph::Node *root = currentRenderTarget->getRoot();
					
					root->findAndRemove(node);
				}
				
				void removeAll(scenegraph::Node *node) {
					for (unsigned int i = 0; i < _render_queue.size(); i++) {
						Renderable *renderTarget = _render_queue[i];
						scenegraph::Node *root = renderTarget->getRoot();
						
						root->findAndRemove(node);
					}
				}
				
				void removeAll() {
					for (unsigned int i = 0; i < _render_queue.size(); i++) {
						Renderable *renderTarget = _render_queue[i];
						renderTarget->getRoot()->clear();
					}
				}
				
				void render();
				void render(scenegraph::Node *node);
				void render(scenegraph::MeshNode *node);
				
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
				
				GLuint _previous_program;
				GLenum _previous_poly_mode, _previous_cull_mode;
				bool _previous_depth_write, _previous_depth_test;
		};
	}
	
#endif