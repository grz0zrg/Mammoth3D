#ifndef RENDERER_HPP
#define RENDERER_HPP

	#include <GL/glew.h>
	
	#include <string>
	#include <iostream>
	
	#include "../core/fbo.hpp"
	
	#include "../cameras/camera.hpp"
	#include "../objects/mesh.hpp"
	#include "../scenegraph/node.hpp"
	#include "../scenegraph/meshnode.hpp"

	namespace renderer {
		typedef enum {
			DEFAULT = 0
		}RenderTarget;

		class Renderer {
			private:
				class Renderable {
					public:
						Renderable() {
							renderTarget = (core::Fbo *)DEFAULT;
							
							root = new scenegraph::Node("root");
							
							meshNode = new scenegraph::MeshNode();
							addNode(meshNode);
						}
						
						~Renderable() {
							delete root;
							delete meshNode;
						}

						void addNode(scenegraph::Node *node) { 
							root->add(node); 
						}
						
						void addMesh(object::Mesh *mesh) { 
							meshNode->addMesh(mesh);
						}
						
						scenegraph::Node *getRoot() {
							return root;
						}

						core::Fbo *renderTarget;

						scenegraph::MeshNode *meshNode;
						scenegraph::Node *root;
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
					glEnable(GL_LINE_SMOOTH);
					
					currCamera = 0;
					viewportWidth = viewportHeight = 0;
					
					setRenderTarget((core::Fbo *)DEFAULT);
				}
				
				~Renderer() {
					for (unsigned int i = 0; i < render_queue.size(); i++) {
						delete render_queue[i];
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
				
				std::vector<Renderable *> render_queue;
				unsigned int currentRenderTargetId;
				
				material::Material *previousMat;
				camera::Camera *currCamera;
				
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
				
				void setCamera(camera::Camera *camera) {
					currCamera = camera;
				}
				
				void setRenderTarget(int fbo) {
					setRenderTarget((core::Fbo *) fbo);
				}
				
				void setRenderTarget(core::Fbo *rt) {
					bool found = false;
					for (unsigned int i = 0; i < render_queue.size(); i++) {
						if (render_queue[i]->renderTarget == rt) {
							currentRenderTargetId = i;
							return;
						}
					}
					
					Renderable *renderable = new Renderable();
					renderable->renderTarget = rt;
					
					render_queue.push_back(renderable);
					
					currentRenderTargetId = render_queue.size()-1;
				}
				
				void add(scenegraph::Node *node) {
					render_queue[currentRenderTargetId]->addNode(node);
				}
				
				void add(object::Mesh *mesh) {
					render_queue[currentRenderTargetId]->addMesh(mesh);
				}
				
				void remove(object::Mesh *mesh) {
					Renderable *currentRenderTarget = render_queue[currentRenderTargetId];
					scenegraph::MeshNode *meshNode = currentRenderTarget->meshNode;

					meshNode->removeMesh(mesh);
				}
				
				void remove(scenegraph::Node *node) {
					Renderable *currentRenderTarget = render_queue[currentRenderTargetId];
					scenegraph::Node *root = currentRenderTarget->getRoot();
					
					root->findAndRemove(node);
				}
				
				void removeAll(scenegraph::Node *node) {
					for (unsigned int i = 0; i < render_queue.size(); i++) {
						Renderable *renderTarget = render_queue[i];
						scenegraph::Node *root = renderTarget->getRoot();
						
						root->findAndRemove(node);
					}
				}
				
				void removeAll() {
					for (unsigned int i = 0; i < render_queue.size(); i++) {
						Renderable *renderTarget = render_queue[i];
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
				
				GLuint previousProgram;
				GLenum previousPolyMode, previousCullMode;
				bool previousDepthWrite, previousDepthTest;
		};
	}
	
#endif