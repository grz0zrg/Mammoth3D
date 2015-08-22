#ifndef MAMMOTH3D_WINDOW_HPP
#define MAMMOTH3D_WINDOW_HPP

	#include <GLFW/glfw3.h>
	
	#include <string>
	#include <iostream>

	namespace window {
		class Window {
			private:
				Window() {
					_fail = false;
					
					glfwSetErrorCallback(errorCallback);

					if (glfwInit() == GL_FALSE) {
						log("glfwInit() failed");
						_fail = true;
					}
					
					_primary_monitor = 0;
					_glfw_window = 0;
					_glfw_vid_mode = 0;
					
					_frames = 0;
					_fps_timer = getTime();
					_last_time = getTime();
					_delta_time = 0.0;
					
					_aa = 0;
				}
				
				~Window() {
					glfwTerminate();
				}
				
				template <typename T>
				static void logPretty(const std::string &str, T param) {
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
				
				static void errorCallback(int error_code, const char *description) {
					logPretty(description, error_code);
				}
				
				bool _fail;
				
				const static std::string _engine_version;
				
				int _window_width, _window_height, _aa;
				
				int _frames, _fps;
				double _fps_timer, _delta_time, _last_time;
				
				GLFWmonitor *_primary_monitor;
				const GLFWvidmode *_glfw_vid_mode;
				GLFWwindow *_glfw_window;
			
				Window(const Window&);
				void operator=(const Window&);
				static Window *_singleton;
	
			public:
				void open(const char *title, int width, int height, bool fullscreen = false, int msaa_level = 0);
				
				void setVSync() {
					glfwSwapInterval(1);
				}
				
				void unsetVSync() {
					glfwSwapInterval(0);
				}
			
				void swapBuffers() {
					glfwSwapBuffers(_glfw_window);
					
					_frames++;
					if (getTime()-_fps_timer >= 1.0) {
						_fps = _frames;
						_frames = 0;
						_fps_timer = getTime();
					}
					
					_delta_time = getTime() - _last_time;
					_last_time = getTime();
				}
				
				double getDeltaTime() const {
					return _delta_time;
				}
				
				void hideMouseCursor() {
					glfwSetInputMode(_glfw_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
				}
				
				void showMouseCursor() {
					glfwSetInputMode(_glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				}

				double getTime() const {
					return glfwGetTime();
				}
				
				int getFps() const {
					return _fps;
				}
				
				void setMSAA(int level) {
					if (_glfw_window) {
						log("MSAA cannot be set, window already opened (call setMSAA before calling window open)");
                        
						return;
					}
                    
					glfwWindowHint(GLFW_SAMPLES, level);
					
					_aa = level;
				}
				
				bool isIconified() const {
					if (glfwGetWindowAttrib(_glfw_window, GLFW_ICONIFIED) == GL_TRUE) {
						return true;
					}
					return false;
				}
				
				int getRefreshRate() const {
					if (_glfw_vid_mode) {
						return _glfw_vid_mode->refreshRate;
					}
					
					return 0;
				}
				
				void setRefreshRate(int hz) {
					if (_glfw_window) {
						log("Refresh rate cannot be set, window already opened (call setRefreshRate before calling window open)");
                        
						return;
					}
					glfwWindowHint(GLFW_REFRESH_RATE, hz);
				}

				void onResize(GLFWwindowsizefun cbfun) {
					cbfun(_glfw_window, _window_width, _window_height);
					
					glfwSetWindowSizeCallback(_glfw_window, cbfun);
				}
				
				int getAASamples() const {
					return _aa;
				}
				
				bool running() const {
					if (_fail) { 
						return false; 
					}
					
					glfwPollEvents();
					
					return (glfwGetKey(_glfw_window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
							!glfwWindowShouldClose(_glfw_window));
				}
				
				int getWidth() const {
					return _window_width;
				}
				
				int getHeight() const {
					return _window_height;
				}
				
				static Window *getInstance()
				{
					if (!_singleton) {
						_singleton =  new Window;
						
						std::atexit(free);
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