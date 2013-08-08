#include "window.hpp"

window::Window *window::Window::_singleton = 0;

void window::Window::openWindow(int width, int height, bool fullscreen, 
										const char *title) {
	if (fail) return;					
	
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
			
	primaryMonitor = glfwGetPrimaryMonitor();
					
	glfwVidMode = glfwGetVideoMode(primaryMonitor);
					
	if (fullscreen) {
		glfw_window = glfwCreateWindow(width, height, title, primaryMonitor, NULL);
	} else {
		glfw_window = glfwCreateWindow(width, height, title, NULL, NULL);
	}
					
	if (!glfw_window) {
		log("glfwOpenWindow(...) failed");
		fail = true;
		return;
	}
					
	glfwMakeContextCurrent(glfw_window);
				
	windowWidth = width;
	windowHeight = height;
}