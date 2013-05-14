#ifndef MESH_OBJECT_HPP
#define MESH_OBJECT_HPP

	#include <cstdlib>
	#include <algorithm>
	
	#include "../loaders/meshloader.hpp"
	#include "../materials/material.hpp"
	#include "../core/math.hpp"
	#include "../core/matrix4.hpp"

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
		
		class Mesh {
			public:
				Mesh(loader::MeshLoader *meshData = 0, 
					 material::Material *mat = 0) { 
					this->mat = mat;
					
					matrixAutoUpdate = true;
					
					indsType = meshData->indsType;
					indicesCount = meshData->indicesCount;
					indices = meshData->indices;
					vertices = meshData->vertices;
					normals = meshData->normals;
					texcoords = meshData->texcoords;
					
					colors.resize(vertices.size(), 0.0f);
					
					for (unsigned int i=0; i<colors.size(); i+=3) {
						float rcolor = (float)rand()/(float)RAND_MAX;
						float gcolor = (float)rand()/(float)RAND_MAX;
						float bcolor = (float)rand()/(float)RAND_MAX;
						rcolor = sin(vertices[i]);
						gcolor = cos(vertices[i+1]);
						bcolor = sin(vertices[i+2]);
						colors[i] = rcolor;
						colors[i+1] = gcolor;
						colors[i+2] = bcolor;
					}
					
					core::Matrix4 m;
					modelMatrix = m;
					
					alpha = 1.0f;
					
					x = y = z = 0.0f;
					sx = sy = sz = 1.0f;
					rx = ry = rz = 0.0f;
					
					rx = core::math::deg2rad(-80);
					/*ry = core::math::deg2rad(-90);
					rz = core::math::deg2rad(10);*/
					x = 0.0f;
					y = 0.0f;
					z = -4.0f;
					
					if (createBuffers() != 1) {
						indices.resize(0);
						vertices.resize(0);
						normals.resize(0);
						texcoords.resize(0);
						colors.resize(0);
						indicesCount = 0;
						
						log("Mesh buffers creations failures.");
					}
				}
				
				~Mesh() { 
					glDeleteBuffers(1, &ibo);
					glDeleteBuffers(1, &vbo);
					glDeleteBuffers(1, &cbo);
				}
				
				void sortTriangles(core::Matrix4 matrix) {
					glDeleteBuffers(1, &ibo);
					
					verticesTransformed = vertices;
					for (unsigned int i=0; i<indices.size(); i++) {
						float x = verticesTransformed[indices[i]], y = verticesTransformed[indices[i]+1], z = verticesTransformed[indices[i]+2];

						//verticesTransformed[indices[i]+2]   = matrix.m[0] * x + matrix.m[4] * y + matrix.m[8 ] * z + matrix.m[12];
						//verticesTransformed[i+1] = matrix.m[1] * x + matrix.m[5] * y + matrix.m[9 ] * z + matrix.m[13];
						verticesTransformed[indices[i]+2] = matrix.m[2] * x + matrix.m[6] * y + matrix.m[10] * z + matrix.m[14];
					}
					
					std::vector<unsigned int *> triangles;
					for (unsigned int i=0; i<indices.size(); i+=3) {
						unsigned int *tri = new unsigned int[3];
						tri[0] = indices[i];
						tri[1] = indices[i+1];
						tri[2] = indices[i+2];
						triangles.push_back(tri);
					}
					
					Sorting sort;
					sort.verticesTransformed = verticesTransformed;
					
					std::sort(triangles.begin(), triangles.end(), sort);
					
					indices.clear();
					
					for (unsigned int i=0; i<triangles.size(); i++) {
						indices.push_back(triangles[i][0]);
						indices.push_back(triangles[i][1]);
						indices.push_back(triangles[i][2]);
					}
					//std::cout << "done " << std::endl;
					
					for (unsigned int i=0; i<triangles.size(); i++) {
						delete[] triangles[i];
					}

					glGenBuffers(1, &ibo);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), 
						&indices.front(), GL_STATIC_DRAW);
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
				
				void setScale(float sx, float sy, float sz) {
					this->sx = sx;
					this->sy = sy;
					this->sz = sz;
				}
				
				inline void updateMatrix() {
					if (matrixAutoUpdate == true) {
						modelMatrix.makeRotationFromEuler(rx, ry, rz);
						modelMatrix.scale(sx, sy, sz);
						modelMatrix.setPosition(x, y, z);
					}
				}
				
				int createBuffers();
				
				material::Material *mat;
				
				core::Matrix4 modelMatrix;
				
				GLuint vbo, cbo, ibo;
				unsigned int indicesCount;
				
				float x, y, z;
				float sx, sy, sz;
				float rx, ry, rz;
				float alpha;
				
				char indsType;
				
				bool matrixAutoUpdate;

				std::vector<unsigned int> indices;
				std::vector<float> vertices;
				std::vector<float> verticesTransformed;
				std::vector<float> normals;
				std::vector<float> texcoords;
				std::vector<float> colors;
		};
	}
	
#endif