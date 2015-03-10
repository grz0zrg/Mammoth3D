#ifndef MAMMOTH3D_JOYSTICK_HPP
#define MAMMOTH3D_JOYSTICK_HPP

	#include <iostream>
	#include <vector>

	#include <GLFW/glfw3.h>
	
	namespace joystick {
		class Joystick {
			public:
				Joystick() {
					_joy = 0;
					_buttons_count = 0;
					_axes_count = 0;
					_buttons = 0;
					_axes = 0;
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
					_joy = id;
				}
				
				bool buttonPressed(int button_index) {
					return (bool)_buttons[button_index];
				}
				
				bool buttonReleased(int button_index) {
					if (_prev_buttons.empty()) {
						return false;
					}
				
					if (_prev_buttons[button_index] > 0 && _buttons[button_index] == 0) {
						return true;
					}

					return false;
				}
				
				float getAxe(int axe_index) {
					if (_axes) {
						return _axes[axe_index];
					}
					
					return 0.0f;
				}
				
				void update() {
					_prev_buttons.resize(_buttons_count, 0);
					for (int i = 0; i < _buttons_count; i++) {
						_prev_buttons[i] = _buttons[i];
					}
					
					_buttons = glfwGetJoystickButtons(_joy, &_buttons_count);
					_axes = glfwGetJoystickAxes(_joy, &_axes_count);
				}
				
				int _joy;
				std::vector<unsigned char> _prev_buttons;
				const unsigned char *_buttons;
				const float *_axes;
				int _axes_count;
				int _buttons_count;
		};
	}

#endif