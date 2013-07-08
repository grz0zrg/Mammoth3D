#ifndef QUAD_OBJECT_HPP
#define QUAD_OBJECT_HPP
	
	#include "mesh.hpp"
	
	namespace object {
		class Quad : public Mesh {
			public:
				Quad(bool screen_aligned = false) {
					this->geom = new core::Geometry();
					
					float v[] = {-1.0f, -1.0f, 0.0f,
								 1.0f, -1.0f, 0.0f,
								 1.0f, 1.0f, 0.0f,
								 -1.0f, 1.0f, 0.0f};
					
					float c[] = {0.0f, 0.0f, 
								 1.0f, 0.0f,
								 1.0f, 1.0f,
								 0.0f, 1.0f};
								 
					for (unsigned int i = 0; i < 12; i++) {
						this->geom->vertices.push_back(v[i]);
					}
									
					for (unsigned int i = 0; i < 8; i++) {
						this->geom->uvs.push_back(c[i]);
					}
					
					this->geom->generateVbo();
					
					if (screen_aligned) {
						this->type = QUAD_ALIGNED;
					} else {
						this->type = QUAD;
					}
					
					this->mat = new material::Material();
					this->mat->setBlending(true);
				}
				
				~Quad() {
					delete this->mat;
					delete this->geom;
				}
		};
	}
	
#endif