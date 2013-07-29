#ifndef MESHNODE_HPP
#define MESHNODE_HPP

	#include "node.hpp"
	#include "../objects/mesh.hpp"

	namespace scenegraph {
		class MeshNode : public Node {
			public:
				MeshNode() {
					this->type = NODE_DRAWABLE;
				}
				
				~MeshNode() {
					
				}
				
				void addMesh(object::Mesh *mesh) {
					if (mesh) {
						meshs.push_back(mesh);
					}
				}
				
				void removeMesh(object::Mesh *mesh) {
					for (unsigned int i = 0; i < meshs.size(); i++) {
						if (meshs[i] == mesh) {
							meshs.erase(meshs.begin() + i);
						}
					}
				}
				
				std::vector<object::Mesh *> meshs;
		};
	}

#endif