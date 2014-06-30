#include "geometry_utils.hpp"

void core::geometryutils::merge(core::Geometry *geom, object::Mesh *mesh) {
	if (!geom || !mesh) {
		return;
	}
				
	core::Geometry *mesh_geom = mesh->_geom;
				
	if (!mesh_geom) {
		return;
	}
				
	glm::mat4 mesh_m = mesh->getTransformedMatrix();
				
	geom->_indices.reserve(geom->_indices.size()+mesh_geom->_indices.size());
	geom->_vertices.reserve(geom->_vertices.size()+mesh_geom->_vertices.size());
	geom->_normals.reserve(geom->_normals.size()+mesh_geom->_normals.size());
	geom->_uvs.reserve(geom->_uvs.size()+mesh_geom->_uvs.size());
	geom->_colors.reserve(geom->_colors.size()+mesh_geom->_colors.size());
	
	unsigned int in_indices_count = geom->_vertices.size() / 3;
	for (unsigned int i = 0; i < mesh_geom->_indices.size(); i++) {
		geom->_indices.push_back(mesh_geom->_indices[i]+in_indices_count);
	}

	for (unsigned int i = 0; i < mesh_geom->_vertices.size(); i += 3) {
		glm::vec4 vertex = glm::vec4(mesh_geom->_vertices[i], 
									 mesh_geom->_vertices[i+1],
									 mesh_geom->_vertices[i+2], 1.0f);
					
		vertex = mesh_m * vertex;

		geom->_vertices.push_back(vertex.x);
		geom->_vertices.push_back(vertex.y);
		geom->_vertices.push_back(vertex.z);
	}
				
	for (unsigned int i = 0; i < mesh_geom->_normals.size(); i++) {
		geom->_normals.push_back(mesh_geom->_normals[i]);
	}
				
	for (unsigned int i = 0; i < mesh_geom->_uvs.size(); i++) {
		geom->_uvs.push_back(mesh_geom->_uvs[i]);
	}
				
	for (unsigned int i = 0; i < mesh_geom->_colors.size(); i++) {
		geom->_colors.push_back(mesh_geom->_colors[i]);
	}
}