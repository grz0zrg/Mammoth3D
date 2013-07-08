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
					indicesType = sizeof(unsigned int);
					
					indicesCount = 0;
					
					vbo = 0;
				}
				
				~Geometry() {
					if (vbo) {
						delete vbo;
					}
				}
				
				Geometry *clone() {
					Geometry *geom = new Geometry();

					geom->indicesType = indicesType;
					geom->indices = indices;
					geom->vertices = vertices;
					geom->normals = normals;
					geom->uvs = uvs;
					geom->colors = colors;
					geom->generateVbo();

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
					
					update();
				}

				void updateVertices() {
					vbo->updateVerticeBuffer(vertices.size() * 
								sizeof(float), &vertices.front());
				}
				
				void updateColors() {
					vbo->updateColorBuffer(colors.size() * 
								sizeof(float), &colors.front());
				}
				
				void update() {
					indicesCount = indices.size();
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
				
				char indicesType;
				
				unsigned int indicesCount;
				
				std::vector<unsigned int> indices;
				std::vector<float> vertices;
				std::vector<float> normals;
				std::vector<float> uvs;
				std::vector<float> colors;
				
				Vbo *vbo;
		};
	}

#endif