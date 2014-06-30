#ifndef MAMMOTH3D_CAMERANODE_HPP
#define MAMMOTH3D_CAMERANODE_HPP

	#include "node.hpp"
	#include "../cameras/camera.hpp"

	namespace scenegraph {
		class CameraNode : public Node {
			public:
				CameraNode(const std::string &name = "") {
					_name = name;
					_type = NODE_CAMERA;
					_camera = 0;
				}
				
				~CameraNode() {
					
				}
				
				void setCamera(camera::Camera *camera) {
					_camera = camera;
				}
				
				camera::Camera *_camera;
		};
	}

#endif