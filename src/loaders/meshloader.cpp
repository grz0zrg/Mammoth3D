#include "meshloader.hpp"

loader::MeshLoader *loader::MeshLoader::_singleton = 0;

void loader::MeshLoader::loadMesh(const std::string &fileName) {
	indices.resize(0);
	vertices.resize(0);
	normals.resize(0);
	indicesCount = 0;
	loaded = false;

	size_t mm = fileName.rfind(".mm");
	//size_t dae = fileName.rfind(".dae");
	
	size_t lastSlash = fileName.rfind("/");
	if (lastSlash != std::string::npos) {
		name = fileName.substr(lastSlash+1, fileName.length());
	} else { name = fileName; }
	
	if (name.empty()) {
		log("Aborted, empty filename.");
		return;
	}

	if (mm != std::string::npos) {
		std::ifstream file;
		file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		file.open(fileName.c_str(), std::ios::in|std::ios::binary);
		try {
			if (file.is_open()) {
				file.seekg(0);
				file.read((char*)&indsType, sizeof(indsType));
				
				unsigned int verticesCount = 0, normalsCount = 0;
				indicesCount = 0;
				file.read((char*)&indicesCount, sizeof(indicesCount));
				file.read((char*)&verticesCount, sizeof(verticesCount));
				file.read((char*)&normalsCount, sizeof(normalsCount));
				
				indices.reserve(indicesCount);
				vertices.reserve(verticesCount);
				normals.reserve(normalsCount);
				
				for (unsigned int i = 0; i < indicesCount; i++) {
					unsigned int ind = 0;
					file.read((char*)&ind, sizeof(ind));
					indices.push_back(ind);
				}

				for (unsigned int i = 0; i < verticesCount; i++) {
					float vert = 0;
					file.read((char*)&vert, sizeof(vert));
					vertices.push_back(vert);
				}

				for (unsigned int i = 0; i < normalsCount; i++) {
					float norm = 0;
					file.read((char*)&norm, sizeof(norm));
				}
				
				file.close();

				logPretty("loaded: ", name);
				
				loaded = true;
			} else { 
				logPretty("Failed to import Mesh: ", fileName);
			}
		} catch (std::ifstream::failure e) {
			logPretty("Exception, failed to import Mesh: ", fileName);
		}
	} else {
		logPretty("Invalid Mesh file extension (only *.mm): ", name);
	}
}