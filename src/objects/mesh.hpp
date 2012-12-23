#ifndef MESH_OBJECT_HPP
#define MESH_OBJECT_HPP

	#include "../loaders/meshloader.hpp"
	#include "../materials/material.hpp"

	namespace object {
		class Mesh {
			public:
				Mesh(loader::MeshLoader *meshData = 0, 
					 material::Material *mat = 0) { 
					data = meshData;
					this->mat = mat;
				}
				
				~Mesh() { 

				}
				
				void setMaterial(material::Material *mat) {
					this->mat = mat;
				}
				
				loader::MeshLoader *data;
				material::Material *mat;
		};
	}
	
#endif