#ifndef MAMMOTH3D_MESH_OBJECT_HPP
#define MAMMOTH3D_MESH_OBJECT_HPP

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
					
					iz  = verticesTransformed[i[0]+2];
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
				Mesh(core::Geometry *geom = 0, 
                        material::Material *mat = 0, 
                        const std::map<core::Texture *, unsigned int> &textures = std::map<core::Texture *, unsigned int>()) { 
					_geom = 0;
					_mat = 0;
                    
                    _i_count = 0;
                    
                    _i_mat_vbo = 0;
					
					_matrix_auto_update = true;

					_vertex_colors = false;
					
					_model_matrix = glm::mat4(1.0f);
					
					_opacity = 1.0f;
                    
					_type = DEFAULT;
                    
					_x  = _y  = _z  = 0.0f;
					_sx = _sy = _sz = 1.0f;
					_rx = _ry = _rz = 0.0f;
                    
                    setGeometry(geom);
                    setMaterial(mat);
                    
					for (auto it = textures.begin(); it != textures.end(); ++it) {
						setTexture(it->first, it->second);
					}
				}
				
				~Mesh() { 
                    disableInstancing();
				}
				
				void sortTriangles(glm::mat4 matrix) {
					//glDeleteBuffers(1, &ibo);
					
					std::vector<float> verticesTransformed = _geom->_vertices;
					for (unsigned int i=0; i<_geom->_indices.size(); i++) {
						float x = verticesTransformed[_geom->_indices[i]], y = verticesTransformed[_geom->_indices[i]+1], z = verticesTransformed[_geom->_indices[i]+2];

						//verticesTransformed[geom->indices[i]+2] = matrix.m[2] * x + matrix.m[6] * y + matrix.m[10] * z + matrix.m[14];
					}
					
					std::vector<unsigned int *> triangles;
					for (unsigned int i=0; i<_geom->_indices.size(); i+=3) {
						unsigned int *tri = new unsigned int[3];
						tri[0] = _geom->_indices[i];
						tri[1] = _geom->_indices[i+1];
						tri[2] = _geom->_indices[i+2];
						triangles.push_back(tri);
					}
					
					Sorting sort;
					sort.verticesTransformed = verticesTransformed;
					
					std::sort(triangles.begin(), triangles.end(), sort);
					
					_geom->_indices.clear();
					
					for (unsigned int i=0; i<triangles.size(); i++) {
						_geom->_indices.push_back(triangles[i][0]);
						_geom->_indices.push_back(triangles[i][1]);
						_geom->_indices.push_back(triangles[i][2]);
					}
					
					for (unsigned int i=0; i<triangles.size(); i++) {
						delete[] triangles[i];
					}

					//createIndicesBuffer();
				}
				
				void log(const char *str) const {
					std::cout << "[Mesh] " << str << std::endl;
				}
				
				void setMaterial(material::Material *mat) {
                    if (mat) {
                        _mat = mat;
                    }
				}
				
				void setPosition(float x, float y, float z) {
					_x = x;
					_y = y;
					_z = z;
				}
				
				void setRotation(float rx, float ry, float rz) {
					_rx = rx;
					_ry = ry;
					_rz = rz;
				}
				
				void setScale(float sx, float sy, float sz) {
					_sx = sx;
					_sy = sy;
					_sz = sz;
				}
				
				void setScale(float s) {
					_sx = s;
					_sy = s;
					_sz = s;
				}
				
				inline glm::mat4 getTransformedMatrix() const {
					if (_matrix_auto_update == true) {
						glm::mat4 tMatrix;
						
						tMatrix  = glm::translate(_model_matrix,   glm::vec3(_x, _y, _z));
						tMatrix *= glm::rotate(_model_matrix, _rx, glm::vec3(1, 0, 0));
						tMatrix *= glm::rotate(_model_matrix, _ry, glm::vec3(0, 1, 0));
						tMatrix *= glm::rotate(_model_matrix, _rz, glm::vec3(0, 0, 1));
						tMatrix *= glm::scale(_model_matrix,       glm::vec3(_sx, _sy, _sz));
						
						return tMatrix;
					}
					
					return _model_matrix;
				}
                
                void disableInstancing() {
                    for (auto it = _i_meshs.begin(); it != _i_meshs.end(); ++it) {
                        delete *it;
                    }
                    
                    _i_mats.clear();
                    
                    if (_i_mat_vbo) {
                        delete _i_mat_vbo;
                        
                        _i_mat_vbo = 0;
                    }
                    
                    _i_count = 0;
                }
                
                const std::vector<Mesh *> enableInstancing(unsigned int count) {
                    disableInstancing();
                    
                    _i_count = count;
                    
                    for (unsigned int i = 0; i < count; i++) {
                        auto mesh = new Mesh();
                        
                        _i_meshs.push_back(mesh);
                    }
                    
                    auto identity_matrix = glm::mat4(1.0f);
                    
                    const float *source = (const float*)glm::value_ptr(identity_matrix);
                    
                    for (unsigned int i = 0; i < count; i++) {
                        for (unsigned int j = 0; j < 16; j++) {
                            _i_mats.push_back(source[j]);
                        }
                    }
    
                    _i_mat_vbo = new core::Vbo();
                    bool ret = _i_mat_vbo->build(count * 16 * sizeof(float), &_i_mats.front(),
                                                    GL_ARRAY_BUFFER,
                                                    GL_DYNAMIC_DRAW);
                    
                    if (!ret) {
                        log("Instance Matrix VBO creation failed.");
                        
                        disableInstancing();
                    } else {
                        for (unsigned int i = 0; i < 4 ; i++) {
                            glEnableVertexAttribArray(4 + i);
                            
                            glVertexAttribDivisor(4 + i, 1);
                        }
                        _i_mat_vbo->_attrib_index = 4;
                        _i_mat_vbo->setComponents(4);
                    }
              
                    return _i_meshs;
                }
				
				void setGeometry(core::Geometry *geom) {
					if (geom) {
						_geom = geom;
					}
				}
				
				void setTexture(core::Texture *texture, unsigned int index = 0) {
					if (_mat) {
						_mat->setTexture(texture, index);
					}
				}
				
				void setProgram(program::Program *program) {
					if (_mat) {
						_mat->setProgram(program);
					}
				}
				
				core::Texture *getTexture(unsigned int index = 0) {
					return _mat->getTexture(index);
				}
                
                material::Material *getMaterial() const {
                    return _mat;
                }
                
                core::Geometry *getGeometry() const {
                    return _geom;
                }
				
                float getOpacity() const {
                    return _opacity;
                }
                
				core::Geometry *cloneGeometry() const {
					if (_geom) {
						return _geom->clone();
					}
					
					return 0;
				}
				
				core::Geometry *_geom;

				material::Material *_mat;
		
				glm::mat4 _model_matrix;
                
                core::Vbo *_i_mat_vbo;
                
                std::vector<float> _i_mats;
                
                unsigned int _i_count;

				float _x, _y, _z;
				float _sx, _sy, _sz;
				float _rx, _ry, _rz;
				float _opacity;
				
				unsigned int _type;

				bool _matrix_auto_update, _vertex_colors;
                
                std::vector<Mesh *> _i_meshs;
		};
	}
	
#endif