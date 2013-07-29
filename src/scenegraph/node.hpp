#ifndef NODE_HPP
#define NODE_HPP

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
			NODE_DRAWABLE
		};

		class Node {
			public:
				Node(const std::string &name = "") {
					this->name = name;
					visible    = true;
					parent     = 0;
					
					accMatrix  = glm::mat4(1.0f);
					localMatrix = glm::mat4(1.0f);
					
					x = y = z    = 0.0f;
					rx = ry = rz = 0.0f;
					sx = sy = sz = 0.0f;
					
					type = NODE_TRANSFORM;
				}
				
				~Node() {
					
				}
				
				void add(Node *node)
				{
					children.push_back(node);
					node->setParent(this);
				}
				
				void remove(Node *node) {
					for (unsigned int i = 0; i < children.size(); i++) {
						if (children[i] == node) {
							children.erase(children.begin()+i);
						}
					}
				}
				
				Node *clone() {
					Node *node     = new Node();
					node->parent   = parent;
					node->children = children;
					node->visible  = visible;

					return node;
				}
				
				glm::mat4 getTransformedMatrix() {
					return getLocalMatrix();
					
					accMatrix = getLocalMatrix();
					
					Node *curr = this;
					while (curr->getParent() != 0) {
						curr = curr->parent;
						accMatrix *= curr->getLocalMatrix();
					}
					
					return accMatrix;
				}
				
				glm::mat4 getLocalMatrix() {
					glm::mat4 matrix;
						
					matrix = glm::translate(localMatrix, glm::vec3(x, y, z));
					matrix *= glm::rotate(localMatrix, rx, glm::vec3(1, 0, 0));
					matrix *= glm::rotate(localMatrix, ry, glm::vec3(0, 1, 0));
					matrix *= glm::rotate(localMatrix, rz, glm::vec3(0, 0, 1));
					matrix *= glm::scale(localMatrix, glm::vec3(sx, sy, sz));
					
					return matrix;
				}
				
				bool isVisible() {
					return visible;
				}
				
				void setVisible(bool visible) {
					this->visible = visible;
				}
				
				void findAndRemove(Node *node) {
					for (unsigned int i = 0; i < children.size(); i++) {
						if (children[i] == node) {
							children.erase(children.begin() + i);
						}
						
						children[i]->findAndRemove(node);
					}
				}
				
				void clear() {
					children.clear();
				}
				
				void setParent(Node *node) {
					parent = node;
				}
				
				Node *getParent() {
					return parent;
				}
				
				glm::mat4 accMatrix;
				glm::mat4 localMatrix;

				float x, y, z;
				float sx, sy, sz;
				float rx, ry, rz;
				
				bool visible;
			
				Node *parent;
				
				std::vector<Node *> children;
				
				NodeType type;
				
				std::string name;
		};
	}

#endif