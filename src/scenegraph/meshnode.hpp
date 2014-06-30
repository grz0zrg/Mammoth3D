#ifndef MAMMOTH3D_MESHNODE_HPP
#define MAMMOTH3D_MESHNODE_HPP

	#include "node.hpp"
	#include "../objects/mesh.hpp"

	namespace scenegraph {
		class MeshNode : public Node {
			public:
				MeshNode(const std::string &name = "") {
					_name = name;
					_type = NODE_MESH;
				}
				
				~MeshNode() {
					
				}
				
				void setProgram(program::Program *prog) {
					for (unsigned int i = 0; i < _meshs.size(); i++) {
						_meshs[i]->setProgram(prog);
						_meshs[i]->_z = -100.0f;
					}
					
					for (unsigned int i = 0; i < _children.size(); i++) {
						Node *node = _children[i];
						
						if (node->_type == NODE_MESH) {
							MeshNode *mesh_node = (MeshNode *)node;
							mesh_node->setProgram(prog);
						}
					}
				}
				
				// temporary (belong to node class when nodes matrix will work)
				void setScale(float scale) {
					for (unsigned int i = 0; i < _meshs.size(); i++) {
						_meshs[i]->setScale(scale,scale,scale);
						_meshs[i]->_mat->setCullMode(GL_NONE);
						_meshs[i]->setRotation(0, 190.0f,0);
						_meshs[i]->_mat->setDepthWrite(false);
						_meshs[i]->_z = -10.0f;
					}
					
					for (unsigned int i = 0; i < _children.size(); i++) {
						Node *node = _children[i];
						
						if (node->_type == NODE_MESH) {
							MeshNode *mesh_node = (MeshNode *)node;
							mesh_node->setScale(scale);
						}
					}
				}
				
				void addMesh(object::Mesh *mesh) {
					if (mesh) {
						_meshs.push_back(mesh);
					}
				}
				
				void removeMesh(object::Mesh *mesh) {
					for (unsigned int i = 0; i < _meshs.size(); i++) {
						if (_meshs[i] == mesh) {
							_meshs.erase(_meshs.begin() + i);
						}
					}
				}
				
				std::vector<object::Mesh *> _meshs;
		};
	}

#endif