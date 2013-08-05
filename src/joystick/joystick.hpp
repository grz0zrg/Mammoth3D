#ifndef MAMMOTH3D_JOYSTICK_HPP
#define MAMMOTH3D_JOYSTICK_HPP

	#include <iostream>
	#include <vector>

	#include <GLFW/glfw3.h>
	
	namespace joystick {
		class Joystick {
			public:
				Joystick() {
					joy = 0;
					buttons_count = 0;
					axes_count = 0;
					buttons = 0;
					axes = 0;
				}
				
				~Joystick() {
					
				}
				
				void printAvailables() {
					for (int i = 0; i < GLFW_JOYSTICK_LAST; i++) {
						if (glfwJoystickPresent(i) == GL_TRUE) {
							const char *name = glfwGetJoystickName(i);
							std::cout << "[Joystick] " << name << " id: " << i << std::endl;
						}
					}
				}
				
				void setJoystick(int id) {
					joy = id;
				}
				
				bool isButtonPressed(int button_index) {
					return (bool)buttons[button_index];
				}
				
				bool isButtonReleased(int button_index) {
					if (prevButtons.empty()) {
						return false;
					}
				
					if (prevButtons[button_index] > 0 && buttons[button_index] == 0) {
						return true;
					}

					return false;
				}
				
				float getAxe(int axe_index) {
					return axes[axe_index];
				}
				
				void update() {
					prevButtons.resize(buttons_count, 0);
					for (int i = 0; i < buttons_count; i++) {
						prevButtons[i] = buttons[i];
					}
					
					buttons = glfwGetJoystickButtons(joy, &buttons_count);
					axes = glfwGetJoystickAxes(joy, &axes_count);
				}
				
				int joy;
				std::vector<unsigned char> prevButtons;
				const unsigned char *buttons;
				const float *axes;
				int axes_count;
				int buttons_count;
		};
	}

#endif