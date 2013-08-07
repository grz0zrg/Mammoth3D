#ifndef MESH_OBJECT_HPP
#define MESH_OBJECT_HPP

	#include <iostream>
	#include <algorithm>

	#include "../include/glm/glm.hpp"
	#include "../include/glm/gtc/matrix_transform.hpp"
	#include "../include/glm/gtc/type_ptr.hpp"
	
	#include "../core/math.hpp"
	#include "../core/geometry.hpp"
	#include "../core/vbo.hpp"
	#include "../core/texture.hpp"
	#include "../misc/builtingeometry.hpp"
	#include "../materials/material.hpp"

	namespace object {
		class Sorting {
			public:
				std::vector<float> verticesTransformed;
				
				bool operator() (unsigned int *i, unsigned int *j) {
					double iz, jz;
					
					iz = verticesTransformed[i[0]+2];
					iz += verticesTransformed[i[1]+2];
					iz += verticesTransformed[i[2]+2];
					
					iz /= 3;
					
					jz = verticesTransformed[j[0]+2];
					jz += verticesTransformed[j[1]+2];
					jz += verticesTransformed[j[2]+2];
					
					jz /= 3;
					
					return (iz > jz);
				}
		};
		
		typedef enum {
			DEFAULT,
			QUAD,
			QUAD_ALIGNED,
			BITMAP_TEXT
		}MeshType;
		
		class Mesh {
			public:
				Mesh() { 
					geom = 0;
					mat = 0;
					
					matrixAutoUpdate = true;
					vertexColors = false;
					
					modelMatrix = glm::mat4(1.0f);
					
					opacity = 1.0f;
					
					type = DEFAULT;
					
					x = y = z = 0.0f;
					sx = sy = sz = 1.0f;
					rx = ry = rz = 0.0f;
				}
				
				~Mesh() { 

				}
				
				void sortTriangles(glm::mat4 matrix) {
					//glDeleteBuffers(1, &ibo);
					
					std::vector<float> verticesTransformed = geom->vertices;
					for (unsigned int i=0; i<geom->indices.size(); i++) {
						float x = verticesTransformed[geom->indices[i]], y = verticesTransformed[geom->indices[i]+1], z = verticesTransformed[geom->indices[i]+2];

						//verticesTransformed[geom->indices[i]+2] = matrix.m[2] * x + matrix.m[6] * y + matrix.m[10] * z + matrix.m[14];
					}
					
					std::vector<unsigned int *> triangles;
					for (unsigned int i=0; i<geom->indices.size(); i+=3) {
						unsigned int *tri = new unsigned int[3];
						tri[0] = geom->indices[i];
						tri[1] = geom->indices[i+1];
						tri[2] = geom->indices[i+2];
						triangles.push_back(tri);
					}
					
					Sorting sort;
					sort.verticesTransformed = verticesTransformed;
					
					std::sort(triangles.begin(), triangles.end(), sort);
					
					geom->indices.clear();
					
					for (unsigned int i=0; i<triangles.size(); i++) {
						geom->indices.push_back(triangles[i][0]);
						geom->indices.push_back(triangles[i][1]);
						geom->indices.push_back(triangles[i][2]);
					}
					
					for (unsigned int i=0; i<triangles.size(); i++) {
						delete[] triangles[i];
					}

					//createIndicesBuffer();
				}
				
				void log(const char *str) {
					std::cout << "[Mesh] " << str << std::endl;
				}
				
				void setMaterial(material::Material *mat) {
					this->mat = mat;
				}
				
				void setPosition(float x, float y, float z) {
					this->x = x;
					this->y = y;
					this->z = z;
				}
				
				void setRotation(float rx, float ry, float rz) {
					this->rx = rx;
					this->ry = ry;
					this->rz = rz;
				}
				
				void setScale(float sx, float sy, float sz) {
					this->sx = sx;
					this->sy = sy;
					this->sz = sz;
				}
				
				void setScale(float s) {
					this->sx = s;
					this->sy = s;
					this->sz = s;
				}
				
				inline glm::mat4 getTransformedMatrix() {
					if (matrixAutoUpdate == true) {
						glm::mat4 tMatrix;
						
						tMatrix = glm::translate(modelMatrix, glm::vec3(x, y, z));
						tMatrix *= glm::rotate(modelMatrix, rx, glm::vec3(1, 0, 0));
						tMatrix *= glm::rotate(modelMatrix, ry, glm::vec3(0, 1, 0));
						tMatrix *= glm::rotate(modelMatrix, rz, glm::vec3(0, 0, 1));
						tMatrix *= glm::scale(modelMatrix, glm::vec3(sx, sy, sz));
						
						return tMatrix;
					}
					
					return modelMatrix;
				}
				
				void setGeometry(core::Geometry *geom) {
					if (geom) {
						this->geom = geom;
					}
				}
				
				core::Geometry *cloneGeometry() {
					if (geom) {
						return geom->clone();
					}
					
					return 0;
				}
				
				core::Geometry *geom;

				material::Material *mat;
		
				glm::mat4 modelMatrix;

				float x, y, z;
				float sx, sy, sz;
				float rx, ry, rz;
				float opacity;
				
				unsigned int type;

				bool matrixAutoUpdate, vertexColors;
		};
	}
	
#endif