#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

	#include <vector>
	
	#include "vbo.hpp"
	
	namespace core {
		typedef enum {
			GEOMETRY_INDICE,
			GEOMETRY_VERTICE,
			GEOMETRY_COLOR
		}GeometryData;
		
		class Geometry {
			public:
				Geometry() {
					indicesCount = 0;
					verticesCount = 0;
					normalsCount = 0;
					uvsCount = 0;
					colorsCount = 0;
					
					vbo = 0;
					
					type = GL_TRIANGLES;
				}
				
				~Geometry() {
					if (vbo) {
						delete vbo;
					}
				}
				
				Geometry *clone() {
					Geometry *geom = new Geometry();

					geom->indices = indices;
					geom->vertices = vertices;
					geom->normals = normals;
					geom->uvs = uvs;
					geom->colors = colors;
					geom->update();

					return geom;
				}
				
				void generateVbo() {
					if (vbo) {
						delete vbo;
						vbo = 0;
					}

					vbo = new Vbo();
					if (!indices.empty()) {
						vbo->buildIndiceBuffer(indices.size() * 
								sizeof(unsigned int), &indices.front());
					}
					
					if (!vertices.empty()) {
						vbo->buildVerticeBuffer(vertices.size() * 
									sizeof(float), &vertices.front());
					}
								
					if (!normals.empty()) {
						vbo->buildNormalBuffer(normals.size() * 
									sizeof(float), &normals.front());
					}
					
					if (!uvs.empty()) {
						vbo->buildUvBuffer(uvs.size() * 
									sizeof(float), &uvs.front());
					}

					if (!colors.empty()) {
						vbo->buildColorBuffer(colors.size() * 
									sizeof(float), &colors.front());
					}
				}

				void updateVertices() {
					vbo->updateVerticeBuffer(vertices.size() * 
								sizeof(float), &vertices.front());
					verticesCount = vertices.size();
				}
				
				void updateColors() {
					vbo->updateColorBuffer(colors.size() * 
								sizeof(float), &colors.front());
					colorsCount = colors.size();
				}
				
				void update() {
					indicesCount = indices.size();
					verticesCount = vertices.size();
					normalsCount = normals.size();
					uvsCount = uvs.size();
					colorsCount = colors.size();
					
					generateVbo();
				}
				
				void setDynamic(GeometryData type, bool state = true) {
					GLenum usage = GL_DYNAMIC_DRAW;
					if (!state) {
						usage = GL_STATIC_DRAW;
					}
					
					if (type == GEOMETRY_COLOR) {
						vbo->colorBufferUsage = usage;
					} else if (type == GEOMETRY_VERTICE) {
						vbo->verticeBufferUsage = usage;
					}
				}
				
				void setType(GLenum type) {
					this->type = type;
				}

				unsigned int indicesCount;
				unsigned int verticesCount;
				unsigned int normalsCount;
				unsigned int uvsCount;
				unsigned int colorsCount;
				
				GLenum type;
				
				std::vector<unsigned int> indices;
				std::vector<float> vertices;
				std::vector<float> normals;
				std::vector<float> uvs;
				std::vector<float> colors;
				
				Vbo *vbo;
		};
	}

#endif