#ifndef WINDOW_HPP
#define WINDOW_HPP

	#include <GLFW/glfw3.h>
	
	#include <string>
	#include <iostream>

	namespace window {
		class Window {
			private:
				Window() {
					fail = false;

					if (!glfwInit()) {
						log("glfwInit() failed");
						fail = true;
					}
					
					primaryMonitor = 0;
					glfw_window = 0;
					glfwVidMode = 0;
					
					frames = 0;
					fpsTimer = getTime();
					lastTime = getTime();
					deltaTime = 0.0;
					
					aa = 0;
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
				
				int windowWidth, windowHeight, aa;
				
				int frames, fps;
				double fpsTimer, deltaTime, lastTime;
				
				GLFWmonitor *primaryMonitor;
				const GLFWvidmode *glfwVidMode;
				GLFWwindow *glfw_window;
			
				Window(const Window&);
				void operator=(const Window&);
				static Window *_singleton;
	
			public:
				void openWindow(int width, int height, bool fullscreen = false, 
													const char *title = "");
				
				void setVSync() {
					glfwSwapInterval(1);
				}
				
				void unsetVSync() {
					glfwSwapInterval(0);
				}
			
				void swapBuffers() {
					glfwSwapBuffers(glfw_window);
					
					frames++;
					if (getTime()-fpsTimer >= 1.0) {
						fps = frames;
						frames = 0;
						fpsTimer = getTime();
					}
					
					deltaTime = getTime() - lastTime;
					lastTime = getTime();
				}
				
				double getDeltaTime() {
					return deltaTime;
				}
				
				void hideMouseCursor() {
					glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
				}
				
				void showMouseCursor() {
					glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				}

				double getTime() {
					return glfwGetTime();
				}
				
				int getFps() {
					return fps;
				}
				
				void setFSAA(int level) {
					if (glfw_window) {
						log("FSAA cannot be applied, window already opened");
						return;
					}
					glfwWindowHint(GLFW_SAMPLES, level);
					
					aa = level;
				}
				
				bool isIconified() {
					if (glfwGetWindowAttrib(glfw_window, GLFW_ICONIFIED) == GL_TRUE) {
						return true;
					}
					return false;
				}
				
				int getRefreshRate() {
					if (glfwVidMode) {
						return glfwVidMode->refreshRate;
					}
					
					return 0;
				}
				
				void setRefreshRate(int hz) {
					if (glfw_window) {
						log("Refresh rate cannot be applied, window already opened");
						return;
					}
					glfwWindowHint(GLFW_REFRESH_RATE, hz);
				}

				void onResize(GLFWwindowsizefun cbfun) {
					glfwSetWindowSizeCallback(glfw_window, cbfun);
				}
				
				int getAASamples() {
					return aa;
				}
				
				bool running() {
					if (fail) { 
						return false; 
					}
					
					glfwPollEvents();
					
					return (glfwGetKey(glfw_window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
							!glfwWindowShouldClose(glfw_window));
				}
				
				int getWidth() {
					return windowWidth;
				}
				
				int getHeight() {
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