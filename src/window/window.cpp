#include "window.hpp"

window::Window *window::Window::_singleton = 0;

void window::Window::openWindow(int width, int height, bool fullscreen, 
										const char *title) {
	if (_fail) return;					
	
	glfwWindowHint(GLFW_RED_BITS, 8);
	glfwWindowHint(GLFW_GREEN_BITS, 8);
	glfwWindowHint(GLFW_BLUE_BITS, 8);
	glfwWindowHint(GLFW_ALPHA_BITS, 8);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);
	
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	// opengl 3.3
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		
	//glfwWindowHint(GLFW_STEREO, GL_TRUE);
			
	_primary_monitor = glfwGetPrimaryMonitor();
					
	_glfw_vid_mode = glfwGetVideoMode(_primary_monitor);
	
	if (width == 0 && height == 0) {
		width  = _glfw_vid_mode->width;
		height = _glfw_vid_mode->height;
		setRefreshRate(_glfw_vid_mode->refreshRate);
	}
					
	if (fullscreen) {
		_glfw_window = glfwCreateWindow(width, height, title, _primary_monitor, NULL);
	} else {
		_glfw_window = glfwCreateWindow(width, height, title, NULL, NULL);
	}
					
	if (!_glfw_window) {
		log("openWindow(...) failed");
		_fail = true;
		return;
	}
					
	glfwMakeContextCurrent(_glfw_window);
				
	_window_width = width;
	_window_height = height;
}
