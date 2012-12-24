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
					
					if (createBuffers() != 1) {
						indices.resize(0);
						vertices.resize(0);
						normals.resize(0);
						indicesCount = 0;
						std::cout << "Failed to create mesh buffers." << std::endl;
					}
				}
				
				~Mesh() { 
					glDeleteBuffers(1, &ibo);
					glDeleteBuffers(1, &vbo);
				}
				
				void setMaterial(material::Material *mat) {
					this->mat = mat;
				}
				
				int createBuffers();
				
				material::Material *mat;
				
				GLuint vbo, ibo;
				unsigned int indicesCount;
				
				char indsType;

				std::vector<unsigned int> indices;
				std::vector<float> vertices;
				std::vector<float> normals;
		};
	}
	
#endif