#ifndef CAMERA_HPP
#define CAMERA_HPP

	#include "../core/vector3.hpp"
	#include "../core/matrix4.hpp"
	#include "../core/math.hpp"
	
	namespace camera {
		typedef enum {
			PERSPECTIVE,
			ORTHOGRAPHIC
		}Type;
		
		class Camera {
			public:
				// type, left, right, top, bottom, near, far : if ORTHOGRAPHIC
				Camera(Type type, float fov = 60, float aspect = 1, 
						float near = 0.1, float far = 10000) {
					this->type   = type;
					this->left   = this->fov = fov;
					this->right  = this->aspect = aspect;
					this->top    = this->near = near;
					this->bottom = this->far = far;
					
					update();
					
					lookAt(0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
				}
				
				~Camera() {
					
				}
				
				void update() {
					core::Matrix4 pm, vm;
					
					if (type == PERSPECTIVE) {
						float ymax = near * tan(core::math::deg2rad(fov * 0.5));
						float ymin = -ymax;
						float xmin = ymin * aspect;
						float xmax = ymax * aspect;
						
						pm.makeFrustum(xmin, xmax, ymin, ymax, near, far);
					} else if(type == ORTHOGRAPHIC) {
						pm.makeOrthographic(left, right, top, bottom, near, far);
					} else {
						log("Unknow camera type.");
					}
					
					projMatrix = pm;
					viewMatrix = vm;
				}
				
				void lookAt(float ex, float ey, float ez,
							float tx, float ty, float tz,
							float ux, float uy, float uz) {
					core::Vector3 x, y, z;
					core::Vector3 eye, target, up;
					
					eye.x = ex; eye.y = ey; eye.z = ez;
					target.x = tx; target.y = ty; target.z = tz;
					up.x = ux; up.y = uy; up.z = uz;
					
					z = eye - target;
					z.normalize();
					
					if (z.length() == 0) {
						z.z = 1;
					}
					
					x = x.cross(up, z);
					x.normalize();
					
					if (x.length() == 0) {
						z.x += 0.0001f;
						x = x.cross(up, z);
						x.normalize();
					}
					
					y = y.cross(z, x);
	
					viewMatrix.m[0] = x.x; viewMatrix.m[4] = y.x; viewMatrix.m[8 ] = z.x;
					viewMatrix.m[1] = x.y; viewMatrix.m[5] = y.y; viewMatrix.m[9 ] = z.y;
					viewMatrix.m[2] = x.z; viewMatrix.m[6] = y.z; viewMatrix.m[10] = z.z;
				}
				
				void log(const char *str) {
					std::cout << "[Camera] " << str << std::endl;
				}
				
				Type type;
				core::Matrix4 projMatrix;
				core::Matrix4 viewMatrix;
				
				float left, right, top, bottom;
				float fov, aspect, near, far;
		};
	}

#endif