#include "collada.hpp"

const std::string loader::Collada::meshFilenameExt = ".mm";

const char *loader::Collada::libNames[] = { "asset", "scene",
									"library_animations",
									"library_controllers",
									"library_cameras",
									"library_effects",
									"library_geometries",
									"library_images",
									"library_lights",
									"library_materials",
									"library_visual_scenes"};
													
const loader::Collada::e_libType loader::Collada::libTypes[] = { 
										ASSET, SCENE, ANIMATIONS,
										CONTROLLERS, CAMERAS, EFFECTS,
										GEOMETRIES, IMAGES, LIGHTS,
										MATERIALS, VISUAL_SCENES, NONE};

loader::Collada::Collada(const char *fileName) {
	data = 0;

	loadFile(fileName);
};

loader::Collada::~Collada() {
	floatArrays.clear();
	vcountArrays.clear();
	polysArrays.clear();
					
	clearInputSemanticData();

	if(data) {
		std::map<std::string, Mesh*>::iterator meshIt;
		meshIt = data->mesh.begin();
		while(meshIt != data->mesh.end()) {
			if(meshIt->second) {
				delete meshIt->second;
			}
			meshIt++;
		}
		
		std::map<std::string, Material*>::iterator materialIt;
		materialIt = data->material.begin();
		while(materialIt != data->material.end()) {
			if(materialIt->second) {
				delete materialIt->second;
			}
			materialIt++;
		}
		
		std::map<std::string, Effect*>::iterator effectIt;
		effectIt = data->effect.begin();
		while(effectIt != data->effect.end()) {
			if(effectIt->second) {
				delete effectIt->second;
			}
			effectIt++;
		}
		delete data;
	}
};
				
void loader::Collada::exportMeshsTo(const char *dir) {
	std::string directory(dir);
	if (directory.at(directory.length()-1) != '/') {
		directory.append("/");
	}

	std::map<std::string, Mesh*>::iterator meshIt;
	meshIt = data->mesh.begin();
	while(meshIt != data->mesh.end()) {
		if(meshIt->second) {
			std::string filename = directory+meshIt->second->name+meshFilenameExt;
			std::ofstream file(filename.c_str(), std::ios::out | std::ios::binary);
			if (!file.good() || !file.is_open()) {
				log("Failed to export Mesh: ", filename);
			}

			// write mesh datas
			char indicesType = 0;
			file.write((char*)&indicesType, sizeof(indicesType));

			unsigned int indicesCount = meshIt->second->indices.size();
			unsigned int verticesCount = meshIt->second->vertices.size();
			unsigned int normalsCount = meshIt->second->normals.size();
			file.write((char*)&indicesCount, sizeof(indicesCount));
			file.write((char*)&verticesCount, sizeof(verticesCount));
			file.write((char*)&normalsCount, sizeof(normalsCount));

			for (unsigned int i = 0; i < indicesCount; i++) {
				file.write((char*)&meshIt->second->indices[i], sizeof(unsigned int));
			}

			for (unsigned int i = 0; i < verticesCount; i++) {
				file.write((char*)&meshIt->second->vertices[i], sizeof(float));
			}

			for (unsigned int i = 0; i < normalsCount; i++) {
				file.write((char*)&meshIt->second->normals[i], sizeof(float));
			}

			file.close();
		}
		meshIt++;
	}
}

void loader::Collada::loadFile(const char *fileName) {
	log("Parsing: ", fileName);
	int ret = doc.LoadFile(fileName);
	if (ret != tinyxml2::XML_NO_ERROR) {
		log("Unable to load file: ", ret);
		return;
	}
						
	tinyxml2::XMLElement* rootElement = doc.FirstChildElement("COLLADA");
	if (!rootElement) {
		log("Missing COLLADA tag, aborting.");
		return;
	}
					
	std::string version = rootElement->Attribute("version");
	if (version.empty()) {
		log("Missing or empty version attribute, aborting.");
		return;
	}
					
	if(version.substr(0, 3) != "1.4") {
		log("Invalid file. Must be version 1.4.x, current: ", version);
		return;
	}
					
	log("Version ", version);
					
	data = new Data;
						
	// traverse all libraries
	int i = 0;
	e_libType libType = libTypes[i];
	while(libType != NONE) {
	tinyxml2::XMLElement* element = rootElement->FirstChildElement(libNames[i]);
	if(element) {
		log("Parsing element: ", libNames[i]);
		if(libType == GEOMETRIES) {
			parseGeometries(element->FirstChildElement());
		} else if(libType == IMAGES) {
			parseImages(element->FirstChildElement());
		} else if(libType == EFFECTS) {
			parseEffects(element->FirstChildElement());
		} else if(libType == MATERIALS) {
			parseMaterials(element->FirstChildElement());
		}
	}
	libType = libTypes[++i];
	}
					
	log("Done.");
}

void loader::Collada::parseEffects(const tinyxml2::XMLElement* element) {
	const tinyxml2::XMLElement* child = element;
	while(child) {
		element = child;
		child = child->NextSiblingElement();

		const std::string elementName = element->Name();
		const std::string effectId = element->Attribute("id");
		if(elementName != "effect" || 
				element->NoChildren() ||
				effectId.empty()) {
			continue;
		}
		
		data->effect[effectId] = new Effect;
						
		// parse <effect ...> childs
		const tinyxml2::XMLElement* effChild = element->FirstChildElement();
		while(effChild) {
			std::string childName = effChild->Name();
			if(childName == "profile_COMMON") {
								
			} else if(childName == "extra") {
								
			}
							
			effChild = effChild->NextSiblingElement();
		}
	}							
}

void loader::Collada::parseMaterials(const tinyxml2::XMLElement* element) {
	const tinyxml2::XMLElement* child = element;
	while(child) {
		element = child;
		child = child->NextSiblingElement();

		const std::string elementName = element->Name();
		const std::string materialId = element->Attribute("id");
		if(elementName != "material" || 
				element->NoChildren() ||
				materialId.empty()) {
			continue;
		}

		data->material[materialId] = new Material;
		
		// parse <material ...> childs
		const tinyxml2::XMLElement* matChild = element->FirstChildElement();
		while(matChild) {
			std::string childName = matChild->Name();
			if(childName == "instance_effect") {
				std::string effectUrl = matChild->Attribute("url");
				effectUrl.erase(0, 1); // erase "#" char
				data->material[materialId]->effect = effectUrl;
			}
			matChild = matChild->NextSiblingElement();
		}
	}							
}

void loader::Collada::parseImages(const tinyxml2::XMLElement* element) {
	const tinyxml2::XMLElement* child = element;
	while(child) {
		element = child;
		child = child->NextSiblingElement();

		const std::string elementName = element->Name();
		const std::string imageId = element->Attribute("id");
		if(elementName != "image" || 
				element->NoChildren() ||
				imageId.empty()) {
			continue;
		}
						
		// parse <image ...> childs
		const tinyxml2::XMLElement* imgChild = element->FirstChildElement();
		while(imgChild) {
			std::string childName = imgChild->Name();
			if(childName == "init_from") {
				const char *text = imgChild->GetText();
				data->image[imageId] = std::string(text);
			}
							
			imgChild = imgChild->NextSiblingElement();
		}
	}					
}

void loader::Collada::parseGeometries(const tinyxml2::XMLElement* element) {
	const tinyxml2::XMLElement* child = element;
	while(child) {
		element = child;
		child = child->NextSiblingElement();

		const std::string elementName = element->Name();
		const std::string meshId = element->Attribute("id");
		const std::string meshName = element->Attribute("name");
		if(elementName != "geometry" || 
				element->NoChildren() ||
				meshId.empty()) {
			continue;
		}
						
		// parse <geometry ...> childs
		const tinyxml2::XMLElement* geomChild = element->FirstChildElement();
		while(geomChild) {
			std::string childName = geomChild->Name();
			
			// <mesh> content
			if(childName == "mesh") {
				data->mesh[meshId] = new Mesh;
				if(!meshName.empty()) {
					data->mesh[meshId]->name = meshName;
				}
				inputSemantic["VERTEX"] = 0;
				inputSemantic["POSITION"] = 0;
				inputSemantic["NORMAL"] = 0;
								
				parseMeshData(meshId, geomChild->FirstChildElement());
			} else if (childName == "extra") { // <extra>
			}

			geomChild = geomChild->NextSiblingElement();
		}
	}
}

void loader::Collada::parseMeshData(const std::string &str, const tinyxml2::XMLElement* element) {
	while(element) {
		const tinyxml2::XMLElement* sourceChild = element->FirstChildElement();
		const std::string elementName = element->Name();
		
		// <source>
		if (elementName == "source") {
			const char* id = element->Attribute("id");
			while(sourceChild) {
				//<float_array ...>
				const std::string sourceChildName = sourceChild->Name();
				if (sourceChildName == "float_array") {
					// parse&store float array
					const char *text = sourceChild->GetText();
					if(id && text) {
						std::vector<std::string> floatData = splitText(text);
										
						unsigned int count = sourceChild->IntAttribute("count");
						unsigned int realSize = floatData.size();
						if(count) {
							if(count == realSize) {
								for(unsigned int i=0; i<realSize; i++) {
									floatArrays[id].push_back((float)strtod(floatData[i].c_str(), NULL));
								}
							} else {
								log(id, " float array size does not match count attribute");
							}
						}
					}
				}
				sourceChild = sourceChild->NextSiblingElement();
			}
			
			// <vertices>
			} else if (elementName == "vertices") {
				const std::string id = element->Attribute("id");
				if(!id.empty()) {
					while(sourceChild) {	
						// <input ...>
						const std::string sourceChildName = sourceChild->Name();
						if (sourceChildName == "input") {
							const std::string semantic = 
										sourceChild->Attribute("semantic");
							std::string source = 
										sourceChild->Attribute("source");
										
							if(!semantic.empty() && !source.empty()) {
								inputSemantic[semantic] = new InputSemanticData;
								inputSemantic[semantic]->source = source.erase(0,1);
								inputSemantic[semantic]->offset = -1;
							}
						}
									
						sourceChild = sourceChild->NextSiblingElement();
					}
				}
				
			// <polylist> <triangles> etc...
			} else if (elementName == "polylist" ||
					elementName == "triangles" ||
					elementName == "lines" ||
					elementName == "linestrips" ||
					elementName == "polygons" ||
					elementName == "trifans" ||
					elementName == "tristrips") {
				unsigned int count = element->IntAttribute("count");
				while(sourceChild) {
					const std::string sourceChildName = sourceChild->Name();
					// <input ...>
					if (sourceChildName == "input") {
						const std::string semantic = sourceChild->Attribute("semantic");
						std::string source = sourceChild->Attribute("source");
						int offset = sourceChild->IntAttribute("offset");
							
						if(!semantic.empty() && !source.empty()) {
							inputSemantic[semantic] = new InputSemanticData;
							inputSemantic[semantic]->source = source.erase(0,1);
							inputSemantic[semantic]->offset = offset;
						}
					// <vcount>
					} else if (sourceChildName == "vcount") {
						const char *text = sourceChild->GetText();
						if (text) {
							std::vector<std::string> vcountData = splitText(text);

							unsigned int realSize = vcountData.size();
							if(count) {
								if(count == realSize) {
									for(unsigned int i=0; i<realSize; i++) {
										vcountArrays[str].push_back(strtoul(vcountData[i].c_str(), NULL, 0));
									}
								} else {
									log(str, " vcount array size does not match count attribute");
								}
							}
						}
					// <p>
					} else if (sourceChildName == "p") {
						const char *text = sourceChild->GetText();
						if (text) {
							std::vector<std::string> polysData = splitText(text);

						// should possibly check collada expected size?
						unsigned int realSize = polysData.size();
						if(!polysData.empty()) {
							for(unsigned int i=0; i<realSize; i++) {
								polysArrays[str].push_back(strtoul(polysData[i].c_str(), NULL, 0));
							}
											
							int semanticCount = 0;
							if(inputSemantic["VERTEX"]) {
								semanticCount++;
							}
							if(inputSemantic["NORMAL"]) {
								semanticCount++;
							}			
							unsigned int expectedSize = (count*3)*semanticCount;
											
							if(expectedSize == realSize) {
								// process mesh
								data->mesh[str]->vertices = floatArrays[inputSemantic["POSITION"]->source];
								const std::string normalSource = inputSemantic["NORMAL"]->source;
								//data->mesh[str]->normals = floatArrays[inputSemantic["NORMAL"]->source];
								data->mesh[str]->vertices.resize(floatArrays[normalSource].size(), 0.0f);
								data->mesh[str]->normals.reserve(floatArrays[normalSource].size());
												
								unsigned int verticesCount = data->mesh[str]->vertices.size()/3;
								for(unsigned int i=0; i<realSize; i+=(semanticCount*3)) {
									// vertices indices
									unsigned long int iv1 = polysArrays[str][i];
									unsigned long int iv2 = polysArrays[str][i+2];
									unsigned long int iv3 = polysArrays[str][i+4];

									// normals indices
									unsigned long int in1 = polysArrays[str][i+1];
									unsigned long int in2 = polysArrays[str][i+3];
									unsigned long int in3 = polysArrays[str][i+5];
													
									// duplicates vertices to match normals
									if(in1 >= verticesCount) {
										data->mesh[str]->vertices[in1] = data->mesh[str]->vertices[iv1];
										iv1 = in1;
									}
									if(in2 >= verticesCount) {
										data->mesh[str]->vertices[in2] = data->mesh[str]->vertices[iv2];
										iv2 = in2;
									}
									if(in3 >= verticesCount) {
										data->mesh[str]->vertices[in3] = data->mesh[str]->vertices[iv3];
										iv3 = in3;
									}
													
									data->mesh[str]->indices.push_back(iv1);
									data->mesh[str]->indices.push_back(iv2);
									data->mesh[str]->indices.push_back(iv3);
													
									// re-build opengl-correct normals etc... ie: unify indices
									data->mesh[str]->normals[iv1] = floatArrays[normalSource][in1];
									data->mesh[str]->normals[iv2] = floatArrays[normalSource][in2];
									data->mesh[str]->normals[iv3] = floatArrays[normalSource][in3];
								}
								//log("",data->mesh[str]->vertices.size()/3);
							} else {
								log(str, " polylist <p> count does not match expected size");
							}
						}
					}
				}

				sourceChild = sourceChild->NextSiblingElement();
			}
		}
						
		element = element->NextSiblingElement();
	}
					
	clearInputSemanticData();
}