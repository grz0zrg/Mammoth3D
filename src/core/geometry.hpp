#ifndef MAMMOTH3D_GEOMETRY_HPP
#define MAMMOTH3D_GEOMETRY_HPP

	#include <vector>
	#include <iostream>
	
	#include "vbo.hpp"
	
	namespace core {
		typedef enum {
			GEOMETRY_INDICE,
			GEOMETRY_VERTICE,
			GEOMETRY_COLOR,
            GEOMETRY_NORMAL,
            GEOMETRY_UV
		} GeometryData;
		
		class Geometry {
			public:
				Geometry();
				~Geometry();
				
				Geometry *clone();
				
				void generateVbo();
                
				void updateVertices();
				void updateColors();
				void update();
                
                void setInstanced(GeometryData type);
				
				void setDynamic(GeometryData type, bool state = true);
				
				void setType(GLenum type) {
					_type = type;
				}
				
				void log(const char *str) {
					std::cout << "[Geometry] " << str << std::endl;
				}

				unsigned int _indices_count;
				unsigned int _vertices_count;
				unsigned int _normals_count;
				unsigned int _uvs_count;
				unsigned int _colors_count;
				
				GLenum _type;
				
				std::vector<unsigned int> _indices;
				std::vector<float> _vertices;
				std::vector<float> _normals;
				std::vector<float> _uvs;
				std::vector<float> _colors;

				Vbo *_i_vbo, *_v_vbo, *_n_vbo, *_u_vbo, *_c_vbo;
                
                bool _ic_vbo, _iv_vbo, _iu_vbo, _in_vbo;
		};
	}

#endif