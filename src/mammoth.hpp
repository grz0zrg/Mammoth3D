#ifndef MAMMOTH_HPP
#define MAMMOTH_HPP

	#include <GL/glew.h>
	#include <GL/glfw.h>
	
	#include <string>
	#include <iostream>

	namespace framework {
		class Mammoth {
			private:
				Mammoth() {
					failure = false;
					log("Mammoth3D v", engineVersion);
	
					if (glfwInit() != GL_TRUE) {
						log("glfwInit() failed.");
						fail();
					}
				}
				
				~Mammoth() {
					if (!failure) {
						glfwTerminate();
					}
				}
				
				template <typename T>
				void logPretty(const std::string &str, T param) {
					std::cout << "[Mammoth] " << str << "\"" << 
								param << "\"" << std::endl;
				}
				
				template <typename T>
				void log(const std::string &str, T param) {
					std::cout << "[Mammoth] " << str << param << std::endl;
				}
					
				void log(const char *str) {
					std::cout << "[Mammoth] " << str << std::endl;
				}
				
				void fail() {
					failure = true;
				}
				
				bool failure;
				
				const static std::string engineVersion;
			
				Mammoth(const Mammoth&);
				void operator=(const Mammoth&);
				static Mammoth *_singleton;
	
			public:
				void openWindow(int width, int height, bool fullscreen = false, const char *title = "Mammoth3D") {
					if (failure) return;
				
					int ret = GL_FALSE;
					if (fullscreen) {
						ret = glfwOpenWindow(width, height, 5, 6, 5, 0, 0, 0, GLFW_FULLSCREEN);
					} else {
						ret = glfwOpenWindow(width, height, 5, 6, 5, 0, 0, 0, GLFW_WINDOW);
					}
					
					if (ret != GL_TRUE) {
						log("glfwOpenWindow(...) failed.");
						fail();
						return;
					}
					
					glfwSetWindowTitle(title);
					
					GLenum err = glewInit();
					if (err != GLEW_OK) {
						log("", glewGetErrorString(err));
						fail();
						glfwTerminate();
						return;
					}

					logPretty("Renderer: ", glGetString(GL_VENDOR));
					logPretty("Vendor: ", glGetString(GL_RENDERER));
					logPretty("Version: ", glGetString(GL_VERSION));

					if (!GLEW_VERSION_3_0) {
						if (!GLEW_VERSION_4_0) {
							log("Incompatible OpenGL version."); 
							fail();
							glfwTerminate();
						}
					}
				}
				
				bool running() {
					if (failure) { 
						return false; 
					} else {
						return (glfwGetKey(GLFW_KEY_ESC) != GLFW_PRESS &&
							glfwGetWindowParam(GLFW_OPENED));
					}
				}
				
				static Mammoth *getInstance()
				{
					if (!_singleton) {
						_singleton =  new Mammoth;
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