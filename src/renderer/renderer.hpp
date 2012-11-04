#ifndef RENDERER_HPP
#define RENDERER_HPP

	#include <GL/glew.h>
	
	#include <string>
	#include <iostream>

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
				static Renderer *getInstance()
				{
					if (!_singleton) {
						_singleton =  new Renderer;
					}

					return _singleton;
				}

				static void kill()
				{
					if (_singleton)
					{
						delete _singleton;
						_singleton = 0;
					}
				}
		};
	}
	
#endif