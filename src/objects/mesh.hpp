#ifndef MESH_OBJECT_HPP
#define MESH_OBJECT_HPP

	#include "../loaders/meshloader.hpp"
	#include "../materials/material.hpp"

	namespace object {
		class Mesh {
			public:
				Mesh(loader::MeshLoader *meshData = 0, 
					 material::Material *mat = 0) { 
					this->mat = mat;
					
					indsType = meshData->indsType;
					indicesCount = meshData->indicesCount;
					indices = meshData->indices;
					vertices = meshData->vertices;
					normals = meshData->normals;
					texcoords = meshData->texcoords;
					
					x = y = z = 0.0f;
					sx = sy = sz = 1.0f;
					
					if (createBuffers() != 1) {
						indices.resize(0);
						vertices.resize(0);
						normals.resize(0);
						texcoords.resize(0);
						indicesCount = 0;
						std::cout << "Mesh buffers creation failure." << std::endl;
					}
				}
				
				~Mesh() { 
					glDeleteBuffers(1, &ibo);
					glDeleteBuffers(1, &vbo);
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
				
				int createBuffers();
				
				material::Material *mat;
				
				GLuint vbo, ibo;
				unsigned int indicesCount;
				
				float x, y, z;
				float sx, sy, sz;
				
				char indsType;

				std::vector<unsigned int> indices;
				std::vector<float> vertices;
				std::vector<float> normals;
				std::vector<float> texcoords;
		};
	}
	
#endif