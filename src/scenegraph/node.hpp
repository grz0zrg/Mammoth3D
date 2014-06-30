#ifndef MAMMOTH3D_NODE_HPP
#define MAMMOTH3D_NODE_HPP

	#include <vector>
	#include <string>
	
	#include "../include/glm/glm.hpp"
	#include "../include/glm/gtc/matrix_transform.hpp"
	#include "../include/glm/gtc/type_ptr.hpp"
	
	namespace scenegraph {
		enum NodeType
		{
			NODE_SCENE,
			NODE_CAMERA,
			NODE_LIGHT,
			NODE_TRANSFORM,
			NODE_MESH
		};

		class Node {
			public:
				Node(const std::string &name = "") {
					_name = name;
					_visible    = true;
					_parent     = 0;
					
					_acc_matrix  = glm::mat4(1.0f);
					_local_matrix = glm::mat4(1.0f);
					
					_x = _y = _z    = 0.0f;
					_rx = _ry = _rz = 0.0f;
					_sx = _sy = _sz = 0.0f;
					
					_type = NODE_TRANSFORM;
				}
				
				~Node() {
					
				}
				
				void add(Node *node)
				{
					_children.push_back(node);
					node->setParent(this);
				}
				
				void remove(Node *node) {
					for (unsigned int i = 0; i < _children.size(); i++) {
						if (_children[i] == node) {
							_children.erase(_children.begin()+i);
						}
					}
				}
				
				Node *clone() {
					Node *node     = new Node();
					node->_parent   = _parent;
					node->_children = _children;
					node->_visible  = _visible;

					return node;
				}
				
				glm::mat4 getTransformedMatrix() {
					return getLocalMatrix();
					
					_acc_matrix = getLocalMatrix();
					
					Node *curr = this;
					while (curr->getParent() != 0) {
						curr = curr->_parent;
						_acc_matrix *= curr->getLocalMatrix();
					}
					
					return _acc_matrix;
				}
				
				glm::mat4 getLocalMatrix() {
					glm::mat4 matrix;
						
					matrix = glm::translate(_local_matrix, glm::vec3(_x, _y, _z));
					matrix *= glm::rotate(_local_matrix, _rx, glm::vec3(1, 0, 0));
					matrix *= glm::rotate(_local_matrix, _ry, glm::vec3(0, 1, 0));
					matrix *= glm::rotate(_local_matrix, _rz, glm::vec3(0, 0, 1));
					matrix *= glm::scale(_local_matrix, glm::vec3(_sx, _sy, _sz));
					
					return matrix;
				}
				
				bool isVisible() {
					return _visible;
				}
				
				void setVisible(bool visible) {
					_visible = visible;
				}
				
				void findAndRemove(Node *node) {
					for (unsigned int i = 0; i < _children.size(); i++) {
						if (_children[i] == node) {
							_children.erase(_children.begin() + i);
						}
						
						_children[i]->findAndRemove(node);
					}
				}
				
				void clear() {
					_children.clear();
				}
				
				void setParent(Node *node) {
					_parent = node;
				}
				
				Node *getParent() {
					return _parent;
				}
				
				glm::mat4 _acc_matrix;
				glm::mat4 _local_matrix;

				float _x, _y, _z;
				float _sx, _sy, _sz;
				float _rx, _ry, _rz;
				
				bool _visible;
			
				Node *_parent;
				
				std::vector<Node *> _children;
				
				NodeType _type;
				
				std::string _name;
		};
	}

#endif