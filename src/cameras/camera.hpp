#ifndef MAMMOTH3D_CAMERA_HPP
#define MAMMOTH3D_CAMERA_HPP

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
					_type   = type;
					_left   = _fov = fov;
					_right  = _aspect = aspect;
					_top    = _near = near;
					_bottom = _far = far;
					
					buildProjection();
					
					lookAt(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f);
				}
				
				~Camera() {
					
				}
				
				void buildProjection() {
					if (_type == PERSPECTIVE) {
						_proj_matrix = glm::perspective(_fov, _aspect, _near, _far);
					} else if(_type == ORTHOGRAPHIC) {
						_proj_matrix = glm::ortho(_left, _right, _bottom, _top);
					} else {
						log("Unknow camera type.");
					}
				}
				
				void lookAt(float ex, float ey, float ez,
							float tx, float ty, float tz,
							float ux, float uy, float uz) {
					_view_matrix = glm::lookAt(glm::vec3(ex, ey, ez),
											 glm::vec3(tx, ty, tz),
											 glm::vec3(ux, uy, uz));
				}
				
				void log(const char *str) {
					std::cout << "[Camera] " << str << std::endl;
				}
				
				Type _type;
				glm::mat4 _proj_matrix;
				glm::mat4 _view_matrix;
				
				float _left, _right, _top, _bottom;
				float _fov, _aspect, _near, _far;
		};
	}

#endif