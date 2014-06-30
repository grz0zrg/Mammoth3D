#ifndef MAMMOTH3D_GEOMETRY_UTILS_HPP
#define MAMMOTH3D_GEOMETRY_UTILS_HPP
	
	#include "geometry.hpp"
	#include "../objects/mesh.hpp"
	
	namespace core {
		namespace geometryutils {
			void merge(core::Geometry *geom, object::Mesh *mesh);
		}
	}

#endif