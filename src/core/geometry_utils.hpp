#ifndef GEOMETRY_UTILS_HPP
#define GEOMETRY_UTILS_HPP
	
	#include "geometry.hpp"
	#include "../objects/mesh.hpp"
	
	namespace core {
		namespace geometryutils {
			void merge(core::Geometry *geom, object::Mesh *mesh) {
				if (!geom || !mesh) {
					return;
				}
				
				core::Geometry *mesh_geom = mesh->geom;
				
				if (!mesh_geom) {
					return;
				}
				
				glm::mat4 mesh_m = mesh->getTransformedMatrix();
				
				geom->indices.reserve(geom->indices.size()+mesh_geom->indices.size());
				geom->vertices.reserve(geom->vertices.size()+mesh_geom->vertices.size());
				geom->normals.reserve(geom->normals.size()+mesh_geom->normals.size());
				geom->uvs.reserve(geom->uvs.size()+mesh_geom->uvs.size());
				geom->colors.reserve(geom->colors.size()+mesh_geom->colors.size());
	
				unsigned int in_indices_count = geom->vertices.size() / 3;
				for (unsigned int i = 0; i < mesh_geom->indices.size(); i++) {
					geom->indices.push_back(mesh_geom->indices[i]+in_indices_count);
				}

				for (unsigned int i = 0; i < mesh_geom->vertices.size(); i += 3) {
					glm::vec4 vertex = glm::vec4(mesh_geom->vertices[i], 
												 mesh_geom->vertices[i+1],
												 mesh_geom->vertices[i+2], 1.0f);
					
					vertex = mesh_m * vertex;

					geom->vertices.push_back(vertex.x);
					geom->vertices.push_back(vertex.y);
					geom->vertices.push_back(vertex.z);
				}
				
				for (unsigned int i = 0; i < mesh_geom->normals.size(); i++) {
					geom->normals.push_back(mesh_geom->normals[i]);
				}
				
				for (unsigned int i = 0; i < mesh_geom->uvs.size(); i++) {
					geom->uvs.push_back(mesh_geom->uvs[i]);
				}
				
				for (unsigned int i = 0; i < mesh_geom->colors.size(); i++) {
					geom->colors.push_back(mesh_geom->colors[i]);
				}
			}
		}
	}

#endif