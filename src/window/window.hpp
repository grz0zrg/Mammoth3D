#ifndef WINDOW_HPP
#define WINDOW_HPP

	#include <GL/glfw.h>
	
	#include <string>
	#include <iostream>

	namespace window {
		class Window {
			private:
				Window() {
					fail = false;

					if (glfwInit() != GL_TRUE) {
						log("glfwInit() failed.");
						fail = true;
					}
				}
				
				~Window() {
					glfwTerminate();
				}
				
				template <typename T>
				void logPretty(const std::string &str, T param) {
					std::cout << "[Window] " << str << "\"" << 
								param << "\"" << std::endl;
				}
				
				template <typename T>
				void log(const std::string &str, T param) {
					std::cout << "[Window] " << str << param << std::endl;
				}
					
				void log(const char *str) {
					std::cout << "[Window] " << str << std::endl;
				}
				
				bool fail;
				
				const static std::string engineVersion;
				
				int windowWidth, windowHeight;
			
				Window(const Window&);
				void operator=(const Window&);
				static Window *_singleton;
	
			public:
				void openWindow(int width, int height, bool fullscreen = false, const char *title = "") {
					if (fail) return;
					
					int ret = GL_FALSE;
					if (fullscreen) {
						ret = glfwOpenWindow(width, height, 5, 6, 5, 0, 0, 0, GLFW_FULLSCREEN);
					} else {
						ret = glfwOpenWindow(width, height, 5, 6, 5, 0, 0, 0, GLFW_WINDOW);
					}
					
					if (ret != GL_TRUE) {
						log("glfwOpenWindow(...) failed.");
						fail = true;
						return;
					}
					
					windowWidth  = width;
					windowHeight = height;

					glfwSetWindowTitle(title);
					
					log("Created.");
				}
				
				void setVSync() {
					glfwSwapInterval(1);
				}
				
				void unsetVSync() {
					glfwSwapInterval(0);
				}	
			
				void swapBuffers() {
					glfwSwapBuffers();
				}
				
				double getTime() {
					return glfwGetTime();
				}
				
				void setFSAA(int level) {
					if (glfwGetWindowParam(GLFW_OPENED)) {
						log("FSAA cannot be applied, the window is already opened");
						return;
					}
					glfwOpenWindowHint(GLFW_FSAA_SAMPLES, level);
				}
				
				void setRefreshRate(int hz) {
					glfwOpenWindowHint(GLFW_REFRESH_RATE, hz);
				}

				void onResize(GLFWwindowsizefun cbfun) {
					glfwSetWindowSizeCallback(cbfun);
				}
				
				bool running() {
					if (fail) { 
						return false; 
					} else {
						return (glfwGetKey(GLFW_KEY_ESC) != GLFW_PRESS &&
							glfwGetWindowParam(GLFW_OPENED));
					}
				}
				
				int getWindowWidth() {
					return windowWidth;
				}
				
				int getWindowHeight() {
					return windowHeight;
				}
				
				static Window *getInstance()
				{
					if (!_singleton) {
						_singleton =  new Window;
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
		};
	}
	
#endif