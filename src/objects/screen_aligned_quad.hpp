#ifndef MAMMOTH3D_SCREEN_ALIGNED_QUAD_OBJECT_HPP
#define MAMMOTH3D_SCREEN_ALIGNED_QUAD_OBJECT_HPP
	
	#include "mesh.hpp"
	
	namespace object {
		class ScreenAlignedQuad : public Mesh {
			public:
				ScreenAlignedQuad() {
					_geom = new core::Geometry();
					
					for (unsigned int i = 0; i < 6; i++) {
						_geom->_indices.push_back(builtingeometry::quadIndice[i]);
					}
				 
					for (unsigned int i = 0; i < 12; i++) {
						_geom->_vertices.push_back(builtingeometry::quadVertice[i]);
					}
									
					for (unsigned int i = 0; i < 8; i++) {
						_geom->_uvs.push_back(builtingeometry::quadUV[i]);
					}
					
					_geom->update();
					
					_mat = new material::Material();
					_mat->setDepthWrite(true);
					_mat->setCullMode(GL_NONE);

					//this->mat->setBlending(true);
					
					_type = QUAD_ALIGNED;
				}
				
				~ScreenAlignedQuad() {
					delete _mat;
					delete _geom;
				}
		};
	}
	
#endif