#include "meshloader.hpp"

loader::MeshLoader *loader::MeshLoader::_singleton = 0;

object::Mesh *loader::MeshLoader::loadMesh(const std::string &fileName) {
	object::Mesh *mesh = 0;

	if (meshs.find(fileName) != meshs.end()) {
		if (meshsGeom[fileName] != 0) {
			mesh = new object::Mesh();
			mesh->setGeometry(meshsGeom[fileName]);
			meshs[fileName].push_back(mesh);
			return mesh;
		}
	}
	
	core::Geometry *meshGeom = 0;
	
	size_t mm = fileName.rfind(".mm");
	//size_t dae = fileName.rfind(".dae");
	
	size_t lastSlash = fileName.rfind("/");
	std::string name, folder = fileName.substr(0, lastSlash+1);
	if (lastSlash != std::string::npos) {
		name = fileName.substr(lastSlash+1, fileName.length());
	} else { name = fileName; }
	
	if (name.empty()) {
		log("Aborted, empty filename.");
		return 0;
	}

	if (mm != std::string::npos) {
		std::ifstream file;
		file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		file.open(fileName.c_str(), std::ios::in|std::ios::binary);
		try {
			if (file.is_open()) {
				logPretty("loading: ", name);
				
				meshGeom = new core::Geometry();
				
				file.seekg(0);
				file.read((char*)&meshGeom->indicesType, sizeof(meshGeom->indicesType));
				
				unsigned int indicesCount = 0;
				unsigned int verticesCount = 0;
				unsigned int normalsCount = 0;
				unsigned int uvsCount = 0;
				unsigned int vColorsCount = 0;
				
				file.read((char*)&indicesCount, sizeof(indicesCount));
				file.read((char*)&verticesCount, sizeof(verticesCount));
				file.read((char*)&normalsCount, sizeof(normalsCount));
				file.read((char*)&uvsCount, sizeof(uvsCount));
				
				meshGeom->indices.reserve(indicesCount);
				meshGeom->vertices.reserve(verticesCount);
				meshGeom->normals.reserve(normalsCount);
				meshGeom->uvs.reserve(uvsCount);
				
				unsigned int indice = 0;
				float vertice = 0;
				float normal = 0;
				float uv = 0;
				float color = 0;
				
				meshGeom->indicesCount = indicesCount;
				
				for (unsigned int i = 0; i < indicesCount; i++) {
					file.read((char*)&indice, sizeof(indice));
					meshGeom->indices.push_back(indice);
				}

				for (unsigned int i = 0; i < verticesCount; i++) {
					file.read((char*)&vertice, sizeof(vertice));
					meshGeom->vertices.push_back(vertice);
				}
				
				for (unsigned int i = 0; i < normalsCount; i++) {
					file.read((char*)&normal, sizeof(normal));
					meshGeom->normals.push_back(normal);
				}
	
				for (unsigned int i = 0; i < uvsCount; i++) {
					file.read((char*)&uv, sizeof(uv));
					meshGeom->uvs.push_back(uv);
				}
				
				file.read((char*)&vColorsCount, sizeof(vColorsCount));
				meshGeom->colors.reserve(vColorsCount);
				
				for (unsigned int i = 0; i < vColorsCount; i++) {
					file.read((char*)&color, sizeof(color));
					meshGeom->colors.push_back(color);
				}
				
				meshGeom->generateVbo();
								
				mesh = new object::Mesh();
				mesh->setGeometry(meshGeom);
				
				// material
				unsigned int diffuse_length = 0;
				file.read((char*)&diffuse_length, sizeof(diffuse_length));
	
				MaterialLoader *mat_ldr = MaterialLoader::getInstance();
				material::Material *material = mat_ldr->createMaterial();
				
				ImageLoader *img_loader = ImageLoader::getInstance();
				TextureLoader *tex_loader = TextureLoader::getInstance();
				
				if (diffuse_length > 0) {
					char *diffuse_filename = new char[diffuse_length];
					file.read(diffuse_filename, sizeof(char)*diffuse_length);
					
					std::string texture_path = folder;
					texture_path += std::string(diffuse_filename);
					
					delete[] diffuse_filename;

					core::Texture *diffuse_texture = 
							tex_loader->loadTexture(img_loader->loadImage(texture_path));

					material->setTexture(diffuse_texture);
				}
				
				mesh->setMaterial(material);
								
				meshsGeom[fileName] = meshGeom;
				meshs[fileName].push_back(mesh);
				
				file.close();
			} else { 
				logPretty("Failed to import Mesh: ", fileName);
			}
		} catch (std::ifstream::failure e) {
			logPretty("Exception, failed to import Mesh: ", fileName);
		}
	} else {
		logPretty("Invalid Mesh file extension (only *.mm): ", name);
	}
	
	return mesh;
}