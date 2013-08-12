#ifndef QUAD_OBJECT_HPP
#define QUAD_OBJECT_HPP
	
	#include "mesh.hpp"

	#include "../window/window.hpp"

	namespace object {
		class Quad : public Mesh {
			public:
				Quad(bool screen_aligned = false) {
					this->geom = new core::Geometry();
					
					for (unsigned int i = 0; i < 6; i++) {
						this->geom->indices.push_back(builtingeometry::quadIndice[i]);
					}
				 
					for (unsigned int i = 0; i < 12; i++) {
						this->geom->vertices.push_back(builtingeometry::quadVertice[i]);
					}
									
					for (unsigned int i = 0; i < 8; i++) {
						this->geom->uvs.push_back(builtingeometry::quadUV[i]);
					}
					
					this->geom->update();
					
					this->mat = new material::Material();
					this->mat->setDepthWrite(true);
					this->mat->setCullMode(GL_NONE);

					//this->mat->setBlending(true);
					
					if (screen_aligned) {	
						this->type = QUAD_ALIGNED;
					} else {
						this->type = QUAD;
					}
				}
				
				~Quad() {
					delete this->mat;
					delete this->geom;
				}
		};
	}
	
#endif