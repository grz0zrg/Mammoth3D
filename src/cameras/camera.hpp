#ifndef CAMERA_HPP
#define CAMERA_HPP

	#include "../include/glm/glm.hpp"
	#include "../include/glm/gtc/matrix_transform.hpp"
	#include "../include/glm/gtc/type_ptr.hpp"

	#include "../core/math.hpp"
	
	namespace camera {
		typedef enum {
			PERSPECTIVE,
			ORTHOGRAPHIC
		}Type;
		
		class Camera {
			public:
				// type, left, right, top, bottom, near, far : if ORTHOGRAPHIC
				Camera(Type type, float fov = 60.0f, float aspect = 1.0f, 
						float near = 0.1f, float far = 10000.0f) {
					this->type   = type;
					this->left   = this->fov = fov;
					this->right  = this->aspect = aspect;
					this->top    = this->near = near;
					this->bottom = this->far = far;
					
					buildProjection();
					
					lookAt(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f);
				}
				
				~Camera() {
					
				}
				
				void buildProjection() {
					if (type == PERSPECTIVE) {
						projMatrix = glm::perspective(fov, aspect, near, far);
					} else if(type == ORTHOGRAPHIC) {
						projMatrix = glm::ortho(left, right, bottom, top);
					} else {
						log("Unknow camera type.");
					}
				}
				
				void lookAt(float ex, float ey, float ez,
							float tx, float ty, float tz,
							float ux, float uy, float uz) {
					viewMatrix = glm::lookAt(glm::vec3(ex, ey, ez),
											 glm::vec3(tx, ty, tz),
											 glm::vec3(ux, uy, uz));
				}
				
				void log(const char *str) {
					std::cout << "[Camera] " << str << std::endl;
				}
				
				Type type;
				glm::mat4 projMatrix;
				glm::mat4 viewMatrix;
				
				float left, right, top, bottom;
				float fov, aspect, near, far;
		};
	}

#endif