#ifndef RENDERER_HPP
#define RENDERER_HPP

	#include <GL/glew.h>
	
	#include <string>
	#include <iostream>
	
	#include "../loader/meshloader.hpp"

	namespace renderer {
		class Renderer {
			private:
				Renderer() {
					GLenum err = glewInit();
					if (err != GLEW_OK) {
						log("", glewGetErrorString(err));
					} else {
						logPretty("Renderer: ", glGetString(GL_VENDOR));
						logPretty("Vendor: ", glGetString(GL_RENDERER));
						logPretty("Version: ", glGetString(GL_VERSION));

						if (!GLEW_VERSION_3_0) {
							if (!GLEW_VERSION_4_0) {
								log("Incompatible OpenGL version."); 
							}
						}
					}
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
				
				
	
			public:
				void setViewport(GLsizei w, GLsizei h) {
					glViewport(0, 0, w, h);
				}
				
				void clear() {
					glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
					glClear(GL_COLOR_BUFFER_BIT);
				}
				
				void render(const loader::MeshLoader *mesh, 
											int count = 1);
				
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