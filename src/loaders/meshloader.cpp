#include "meshloader.hpp"

loader::MeshLoader *loader::MeshLoader::_singleton = 0;

loader::MeshLoader::~MeshLoader() { 
    std::map<std::string, std::vector<object::Mesh *> >::iterator meshsIt 
                                                        = _meshs.begin();
    for (meshsIt; meshsIt != _meshs.end(); ++meshsIt) {
        if (!meshsIt->second.empty()) {
            for (unsigned int i = 0; i < meshsIt->second.size(); i++) {
                delete meshsIt->second[i];
            }
            meshsIt->second.clear();
        }
    }
					
    std::map<std::string, std::vector<core::Geometry *> >::iterator meshsGeomIt 
                                                        = _meshs_geom.begin();
    for (meshsGeomIt; meshsGeomIt != _meshs_geom.end(); ++meshsGeomIt) {
        for (unsigned int i = 0; i < meshsGeomIt->second.size(); i++) {
            core::Geometry *geom = meshsGeomIt->second[i];
            if (geom) {
                delete geom;
            }
            geom = 0;
        }
    }
					
    std::map<std::string, std::vector<scenegraph::MeshNode *> >::iterator meshNodesIt 
                                                        = _mesh_nodes.begin();
    for (meshNodesIt; meshNodesIt != _mesh_nodes.end(); ++meshNodesIt) {
        for (unsigned int i = 0; i < meshNodesIt->second.size(); i++) {
            scenegraph::MeshNode *m_node = meshNodesIt->second[i];
            if (m_node) {
                delete m_node;
            }
            m_node = 0;
        }
    }
					
    std::map<std::string, scenegraph::MeshNode *>::iterator nodesIt 
                                                        = _nodes.begin();
    for (nodesIt; nodesIt != _nodes.end(); ++nodesIt) {
        scenegraph::Node *node = nodesIt->second;
        if (node) {
            delete node;
        }
        node = 0;
    }
}
                
scenegraph::MeshNode *loader::MeshLoader::loadMesh(const std::string &fileName) {
	if (_nodes.find(fileName) != _nodes.end()) {
		return (scenegraph::MeshNode *)_nodes[fileName];
	}

	size_t lastSlash = fileName.rfind("/");
	std::string name, folder = fileName.substr(0, lastSlash+1);
	if (lastSlash != std::string::npos) {
		name = fileName.substr(lastSlash+1, fileName.length());
	} else { name = fileName; }
	
	if (name.empty()) {
		log("Aborted, empty filename.");
		return 0;
	}
	
	scenegraph::MeshNode *root_node;

	std::ifstream file;
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		
	try {
		file.open(fileName.c_str(), std::ios::in|std::ios::binary);

		if (file.is_open()) {
			log("loading: ", name);

			file.seekg(0);

			int meshs_count = 0;
				
			unsigned int numNodes = 0;

			file.read((char*)&numNodes, sizeof(numNodes));
	
			if (numNodes > 0) {
				root_node = new scenegraph::MeshNode(name);
			}

			for (unsigned int n = 0; n < numNodes; n++) {
				scenegraph::MeshNode *node = new scenegraph::MeshNode();

				unsigned int numMeshes = 0;
				file.read((char*)&numMeshes, sizeof(numMeshes));
				
				// ignore others types of nodes
				float tmp_matrix[16];
				glm::mat4 tmatrix = glm::mat4(1.0f);
				if (numMeshes > 0) {
					file.read((char*)&tmp_matrix, sizeof(float) * 16);
				}
				
				for (unsigned int m = 0; m < numMeshes; m++) {
					unsigned int indicesCount  = 0;
					unsigned int verticesCount = 0;
					unsigned int normalsCount  = 0;
					unsigned int uvsCount      = 0;
					unsigned int vColorsCount  = 0;
						
					core::Geometry *meshGeom = new core::Geometry();
						
					file.read((char*)&indicesCount,  sizeof(indicesCount));
					file.read((char*)&verticesCount, sizeof(verticesCount));
					file.read((char*)&normalsCount,  sizeof(normalsCount));
					file.read((char*)&uvsCount,      sizeof(uvsCount));
					file.read((char*)&vColorsCount,  sizeof(vColorsCount));

					meshGeom->_indices.reserve(indicesCount);
					meshGeom->_vertices.reserve(verticesCount);
					meshGeom->_normals.reserve(normalsCount);
					meshGeom->_uvs.reserve(uvsCount);
					meshGeom->_colors.reserve(vColorsCount);
						
					unsigned int indice = 0;
					float vertice = 0;
					float normal = 0;
					float uv = 0;
					float color = 0;
						
					meshGeom->_indices_count = indicesCount;
					
					for (unsigned int i = 0; i < indicesCount; i++) {
						file.read((char*)&indice, sizeof(indice));
						meshGeom->_indices.push_back(indice);
					}

					for (unsigned int i = 0; i < verticesCount; i++) {
						file.read((char*)&vertice, sizeof(vertice));
						meshGeom->_vertices.push_back(vertice);
					}

					for (unsigned int i = 0; i < normalsCount; i++) {
						file.read((char*)&normal, sizeof(normal));
						meshGeom->_normals.push_back(normal);
					}

					for (unsigned int i = 0; i < vColorsCount; i++) {
						file.read((char*)&color, sizeof(color));
						meshGeom->_colors.push_back(color);
					}
						
					for (unsigned int i = 0; i < uvsCount; i++) {
						file.read((char*)&uv, sizeof(uv));
						meshGeom->_uvs.push_back(uv);
					}

					meshGeom->update();
										
					object::Mesh *mesh = new object::Mesh();
					mesh->setGeometry(meshGeom);
						
					glm::mat4 gm = glm::make_mat4(tmp_matrix);
					if (meshs_count > 0){
						tmatrix *= glm::make_mat4(tmp_matrix);
						std::cout << gm[1].x <<std::endl;
						std::cout << gm[1].y <<std::endl;
						std::cout << gm[1].z <<std::endl;
					}
					
					mesh->_model_matrix = tmatrix;

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
					
					_meshs_geom[fileName].push_back(meshGeom);
					_meshs[fileName].push_back(mesh);
						
					node->addMesh(mesh);

					meshs_count++;
				}
					
				root_node->add((scenegraph::Node *)node);
					
				_mesh_nodes[fileName].push_back(node);
			}
				
			_nodes[fileName] = root_node;
				
			file.close();
		} else { 
			log("Failed to import Mesh: ", fileName);
		}
	} catch (std::ifstream::failure e) {
		log("Exception, failed to import Mesh: ", fileName);
	}
	
	return (scenegraph::MeshNode *)root_node;
}