#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

	#include <vector>
	#include <iostream>
	
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
					
					iVbo = vVbo = nVbo = uVbo = cVbo = 0;

					type = GL_TRIANGLES;
				}
				
				~Geometry() {
					if (iVbo) {
						delete iVbo;
					}
					
					if (vVbo) {
						delete vVbo;
					}
					
					if (nVbo) {
						delete nVbo;
					}

					if (uVbo) {
						delete uVbo;
					}
					
					if (cVbo) {
						delete cVbo;
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
					bool ret = false;
					if (!indices.empty()) {
						if (iVbo) {
							delete iVbo;
						}
						iVbo = new Vbo();
						ret = iVbo->build(indices.size() * sizeof(unsigned int), 
									&indices.front(),
									GL_ELEMENT_ARRAY_BUFFER,
									GL_STATIC_DRAW);
						if (!ret) {
							log("Indices VBO creation failed.");
						}
					}
					
					if (!vertices.empty()) {
						if (vVbo) {
							delete vVbo;
						}
						vVbo = new Vbo();
						ret = vVbo->build(vertices.size() * sizeof(float), 
									&vertices.front(),
									GL_ARRAY_BUFFER,
									GL_STATIC_DRAW);
						vVbo->attrib(0);
						vVbo->setComponents(3);
						if (!ret) {
							log("Vertices VBO creation failed.");
						}
					}
					
					if (!uvs.empty()) {
						if (uVbo) {
							delete uVbo;
						}
						uVbo = new Vbo();
						ret = uVbo->build(uvs.size() * sizeof(float), 
									&uvs.front(),
									GL_ARRAY_BUFFER,
									GL_STATIC_DRAW);
						uVbo->attrib(1);
						uVbo->setComponents(2);
						if (!ret) {
							log("UVs VBO creation failed.");
						}
					}
					
					if (!normals.empty()) {
						if (nVbo) {
							delete nVbo;
						}
						nVbo = new Vbo();
						ret = nVbo->build(normals.size() * sizeof(float), 
									&normals.front(),
									GL_ARRAY_BUFFER,
									GL_STATIC_DRAW);
						nVbo->attrib(2);
						nVbo->setComponents(3);
						if (!ret) {
							log("Normals VBO creation failed.");
						}
					}
					
					if (!colors.empty()) {
						if (cVbo) {
							delete cVbo;
						}
						cVbo = new Vbo();
						ret = cVbo->build(uvs.size() * sizeof(float), 
									&uvs.front(),
									GL_ARRAY_BUFFER,
									GL_STATIC_DRAW);
						cVbo->attrib(3);
						cVbo->setComponents(4);
						if (!ret) {
							log("Colors VBO creation failed.");
						}
					}
				}

				void updateVertices() {
					vVbo->update(vertices.size() * sizeof(float), &vertices.front());
					verticesCount = vertices.size();
				}
				
				void updateColors() {
					cVbo->update(colors.size() * sizeof(float), &colors.front());
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
						if (cVbo) {
							cVbo->setUsage(usage);
						}
					} else if (type == GEOMETRY_VERTICE) {
						if (vVbo) {
							vVbo->setUsage(usage);
						}
					}
				}
				
				void setType(GLenum type) {
					this->type = type;
				}
				
				void log(const char *str) {
					std::cout << "[Geometry] " << str << std::endl;
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

				Vbo *iVbo, *vVbo, *nVbo, *uVbo, *cVbo;
		};
	}

#endif