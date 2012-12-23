#include "meshloader.hpp"

void loader::MeshLoader::loadMesh(const std::string &fileName) {
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
				
				if (createBuffers() != 1) {
					indices.resize(0);
					vertices.resize(0);
					normals.resize(0);
					indicesCount = 0;
					log("Failed to create mesh buffers.");
					return;
				}

				logPretty("loaded: ", name);
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

int loader::MeshLoader::createBuffers() {
	GLenum err;
	
	glGenBuffers(1, &ibo);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), 
		&vertices.front(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), 
		&indices.front(), GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	err = glGetError();
	if (err != GL_NO_ERROR) {
		glDeleteBuffers(1, &ibo);
		glDeleteBuffers(1, &vbo);
		return -1;
	}
	
	return 1;
}