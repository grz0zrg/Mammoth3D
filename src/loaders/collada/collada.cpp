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
		const Mesh *mesh = meshIt->second;
		
		if(mesh) {
			std::string filename = directory+mesh->name+meshFilenameExt;
			std::ofstream file(filename.c_str(), std::ios::out | std::ios::binary);
			if (!file.good() || !file.is_open()) {
				log("Failed to export Mesh: ", filename);
			}

			unsigned int numMeshes = 1;
			file.write((char*)&numMeshes, sizeof(numMeshes));

			unsigned int indicesCount = mesh->indices.size();
			unsigned int verticesCount = mesh->vertices.size();
			unsigned int normalsCount = mesh->normals.size();
			unsigned int texcoordsCount = mesh->texcoord.size();
			file.write((char*)&indicesCount, sizeof(indicesCount));
			file.write((char*)&verticesCount, sizeof(verticesCount));
			file.write((char*)&normalsCount, sizeof(normalsCount));
			file.write((char*)&texcoordsCount, sizeof(texcoordsCount));

			for (unsigned int i = 0; i < indicesCount; i++) {
				file.write((char*)&mesh->indices[i], sizeof(unsigned int));
			}

			for (unsigned int i = 0; i < verticesCount; i++) {
				file.write((char*)&mesh->vertices[i], sizeof(float));
			}

			for (unsigned int i = 0; i < normalsCount; i++) {
				file.write((char*)&mesh->normals[i], sizeof(float));
			}

			for (unsigned int i = 0; i < texcoordsCount; i++) {
				file.write((char*)&mesh->texcoord[i], sizeof(float));
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
					
	if (!rootElement->Attribute("version")) {
		log("Missing or empty version attribute, aborting.");
		return;
	}
	
	std::string version = rootElement->Attribute("version");
					
	if(version.substr(0, 3) != "1.4") {
		log("Warning. Collada version != 1.4.x, current: ", version);
	}
					
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
		if(elementName != "effect" || 
				element->NoChildren() ||
				!element->Attribute("id")) {
			continue;
		}
		
		std::string effectId = element->Attribute("id");
		
		data->effect[effectId] = new Effect;
						
		// parse <effect ...> childs
		const tinyxml2::XMLElement* effChild = element->FirstChildElement();
		while(effChild) {
			std::string childName = effChild->Name();
			if(childName == "profile_COMMON") {
				parseEffectData(effectId, effChild->FirstChildElement());
			} else if(childName == "extra") {
				
			}
							
			effChild = effChild->NextSiblingElement();
		}
	}							
}

void loader::Collada::parseEffectData(const std::string &effectId,
										const tinyxml2::XMLElement* element) {
	while(element) {
		const tinyxml2::XMLElement* child = element->FirstChildElement();
		const std::string elementName = element->Name();
		
		// <technique ...>
		if (elementName == "technique") {
			while(child) {
				const std::string techChildName = child->Name();
				if (techChildName == "phong") { // <phong>
					const tinyxml2::XMLElement* matChild = 
												child->FirstChildElement();
					while (matChild) {
						const std::string matChildName = matChild->Name();
						data->effect[effectId]->effectData[matChildName] = 
																	new Param;
						
						const tinyxml2::XMLElement* matDataChild = 
												matChild->FirstChildElement();
						while (matDataChild) {	
							const std::string dataTypeName = 
														matDataChild->Name();
							
							// <color ...>
							if (dataTypeName == "color") {
								const char *values = matDataChild->GetText();
								if (values) {
									std::vector<std::string> colorData = 
															splitText(values);
									unsigned int size = colorData.size();
									if (colorData.size() <= 4) {
										for(unsigned int i=0; i<size; i++) {
											data->effect[effectId]
												->effectData[matChildName]
												->rgba[i] = 
											strtof(colorData[i].c_str(), NULL);	
										}
									}
								}

							// <texture ...>
							} else if (dataTypeName == "texture") { 
								const std::string textureName = 
											matDataChild->Attribute("texture");
								const std::string texcoord = 
											matDataChild->Attribute("texcoord");
								if (texcoord == "UVMap") {
									data->effect[effectId]
										->effectData[matChildName]
										->texcoord = 0;									
								}
								
								// for the sake of simplicity we just get
								// the image name at this point instead of
								// parsing <newparam ...> sampler/surface tags...
								std::size_t found = textureName.find("-sampler");
								if (found != std::string::npos) {
									data->effect[effectId]
										->effectData[matChildName]
										->image = textureName.substr(0,found);
								} else {
									data->effect[effectId]
										->effectData[matChildName]
										->image = textureName;									
								}
								
							// <float ...>
							} else if (dataTypeName == "float") {
								const char *value = matDataChild->GetText();
								data->effect[effectId]
											->effectData[matChildName]
											->v1 = 
											strtof(value, NULL);						
							}
							
							matDataChild = matDataChild->NextSiblingElement();
						}
						
						matChild = matChild->NextSiblingElement();
					}
				}
				
				child = child->NextSiblingElement();
			}

		// <newparam ...> not supported
		} else if (elementName == "newparam") {
			
		// <extra ...>
		} else if (elementName == "extra") {
			
		}
		
		element = element->NextSiblingElement();
	}
}

void loader::Collada::parseMaterials(const tinyxml2::XMLElement* element) {
	const tinyxml2::XMLElement* child = element;
	while(child) {
		element = child;
		child = child->NextSiblingElement();

		const std::string elementName = element->Name();
		if(elementName != "material" || 
				element->NoChildren() ||
				!element->Attribute("id")) {
			continue;
		}
		
		std::string materialId = element->Attribute("id");

		data->material[materialId] = new Material;
		
		// parse <material ...> childs
		const tinyxml2::XMLElement* matChild = element->FirstChildElement();
		while(matChild) {
			std::string childName = matChild->Name();
			if(childName == "instance_effect") {
				std::string effectUrl = matChild->Attribute("url");
				if (!effectUrl.empty()) {
					std::size_t found = effectUrl.find("#")+1;
					if (found != std::string::npos) {
						effectUrl = effectUrl.substr(found);
					}
					data->material[materialId]->effect = effectUrl;
				}
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
		if(elementName != "image" || 
				element->NoChildren() ||
				!element->Attribute("id")) {
			continue;
		}
		
		std::string imageId = element->Attribute("id");
						
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
		
		if(elementName != "geometry" || 
				element->NoChildren() ||
				!element->Attribute("id")) {
			continue;
		}
		
		std::string meshId = element->Attribute("id");
						
		// parse <geometry ...> childs
		const tinyxml2::XMLElement* geomChild = element->FirstChildElement();
		while(geomChild) {
			std::string childName = geomChild->Name();
			
			// <mesh> content
			if(childName == "mesh") {
				data->mesh[meshId] = new Mesh;
				if(element->Attribute("name")) {
					data->mesh[meshId]->name = element->Attribute("name");;
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

void loader::Collada::parseMeshData(const std::string &str,
										const tinyxml2::XMLElement* element) {
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
				if(element->Attribute("id")) {
					std::string id = element->Attribute("id");
					while(sourceChild) {	
						// <input ...>
						const std::string sourceChildName = sourceChild->Name();
						if (sourceChildName == "input") {
							if(sourceChild->Attribute("semantic") 
								&& sourceChild->Attribute("source")) {
								std::string semantic = 
										sourceChild->Attribute("semantic");
								std::string source = 
										sourceChild->Attribute("source");
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
				const char *materialName = element->Attribute("material");
				if (materialName) {
					data->mesh[str]->material = std::string(materialName);
				}
				
				while(sourceChild) {
					const std::string sourceChildName = sourceChild->Name();
					// <input ...>
					if (sourceChildName == "input") {
						int offset = sourceChild->IntAttribute("offset");
							
						if(sourceChild->Attribute("semantic") 
							&& sourceChild->Attribute("source")) {
							std::string semantic = sourceChild->Attribute("semantic");
							std::string source = sourceChild->Attribute("source");
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

						// should probably check collada expected size?
						unsigned int realSize = polysData.size();
						if(!polysData.empty()) {
							for(unsigned int i=0; i<realSize; i++) {
								polysArrays[str].push_back(strtoul(polysData[i].c_str(), NULL, 0));
							}

							// calculate expected data size
							int semanticCount = 0;
							if(inputSemantic["VERTEX"]) {
								semanticCount++;
							}
							if(inputSemantic["NORMAL"]) {
								semanticCount++;
							}	
							if(inputSemantic["TEXCOORD"]) {
								semanticCount++;
							}
							
							unsigned int expectedSize = 0;
							for(unsigned int i=0; i<vcountArrays[str].size(); i++) {
								expectedSize += vcountArrays[str][i]*semanticCount;
							}

							unsigned int vPerFaces = 3; // <vcount> always 3 vertices/faces
							if (vcountArrays[str].empty()) {
								expectedSize = count*2;
								vPerFaces = 2; // hacky <lines> support
							}
							
							// parse data, maximum ugliness start here :)
							if(expectedSize == realSize) {
								data->mesh[str]->vertices = floatArrays[inputSemantic["POSITION"]->source];
								
								int vOffset = inputSemantic["VERTEX"]->offset;
								int nOffset = 1, tOffset = 2;
								
								std::string texcoordSource;
								if(inputSemantic["TEXCOORD"]) {
									tOffset = inputSemantic["TEXCOORD"]->offset;
									texcoordSource = inputSemantic["TEXCOORD"]->source;
									data->mesh[str]->texcoord = floatArrays[inputSemantic["TEXCOORD"]->source];
								}
								std::string normalSource;
								if(inputSemantic["NORMAL"]) {
									nOffset = inputSemantic["NORMAL"]->offset;
									normalSource = inputSemantic["NORMAL"]->source;
									data->mesh[str]->normals = floatArrays[inputSemantic["NORMAL"]->source];
									/*data->mesh[str]->vertices.reserve(floatArrays[normalSource].size());
									if(inputSemantic["TEXCOORD"]) {
										data->mesh[str]->texcoord.resize(floatArrays[normalSource].size(), 0.0f);
									}
									data->mesh[str]->normals.resize(floatArrays[normalSource].size(), 0.0f);
									data->mesh[str]->vertices.resize(floatArrays[normalSource].size(), 0.0f);*/
								}
	
								unsigned long int face[9] = {0,0,0,
															 0,0,0,
															 0,0,0};
															 
								//unsigned long int face_tmp[3] = {0,0,0};
								
								std::vector<float> vertices;
								std::vector<float> normals;
								std::vector<float> texcoord;
								std::vector<unsigned int> indices;
					
								//unsigned int verticesCount = data->mesh[str]->vertices.size()/vPerFaces;
								for(unsigned int i=0; i<realSize; i+=(semanticCount*vPerFaces)) {
									// indices
									face[0] = polysArrays[str][i+vOffset];
									face[1] = polysArrays[str][i+semanticCount+vOffset];
									face[2] = polysArrays[str][i+semanticCount*2+vOffset];
									
									if (inputSemantic["NORMAL"]) {
										face[3] = polysArrays[str][i+nOffset];
										face[4] = polysArrays[str][i+semanticCount+nOffset];
										face[5] = polysArrays[str][i+semanticCount*2+nOffset];
									}
									
									if (inputSemantic["TEXCOORD"]) {
										face[6] = polysArrays[str][i+tOffset];
										face[7] = polysArrays[str][i+semanticCount+tOffset];
										face[8] = polysArrays[str][i+semanticCount*2+tOffset];
									}
									
									for (unsigned int e=0; e<vPerFaces; e++) {
										int indicesSize = indices.size();
										int indv = -1;
										int indn = -1;
										int indt = -1;
										for (int j=0; j<indicesSize; j++) {
											if (vertices.size() > (indices[j]*vPerFaces+1)) {
												if (vertices[indices[j]*vPerFaces] == data->mesh[str]->vertices[face[e]*vPerFaces] &&
													vertices[indices[j]*vPerFaces+1] == data->mesh[str]->vertices[face[e]*vPerFaces+1] &&
													vertices[indices[j]*vPerFaces+2] == data->mesh[str]->vertices[face[e]*vPerFaces+2]) {
													indv = j;
								
													if (inputSemantic["NORMAL"]) {
														if (normals.size() > (indices[j]*vPerFaces+1)) {
															if (normals[indices[j]*vPerFaces] == data->mesh[str]->normals[face[e+3]*vPerFaces] &&
																normals[indices[j]*vPerFaces+1] == data->mesh[str]->normals[face[e+3]*vPerFaces+1] &&
																normals[indices[j]*vPerFaces+2] == data->mesh[str]->normals[face[e+3]*vPerFaces+2]) {
																indn = j;
															} else {
																indn = -1;
															}
														}
													} else {
														indn = 0;
													}
													
													if (inputSemantic["TEXCOORD"]) {
														if (texcoord.size() > (indices[j]*vPerFaces)) {
															if (texcoord[indices[j]*vPerFaces] == data->mesh[str]->texcoord[face[e+6]*vPerFaces] &&
																texcoord[indices[j]*vPerFaces+1] == data->mesh[str]->texcoord[face[e+6]*vPerFaces+1]) {
																indt = j;
															} else {
																indt = -1;
															}
														}
													} else {
														indt = 0;
													}
												}
												
												if (indv != -1 && indn != -1 && indt != -1) break; else indv = -1;
											}
										}
										
										if (indv == -1 || indn == -1 || indt == -1) {
											vertices.push_back(data->mesh[str]->vertices[face[e]*vPerFaces]);
											vertices.push_back(data->mesh[str]->vertices[face[e]*vPerFaces+1]);
											vertices.push_back(data->mesh[str]->vertices[face[e]*vPerFaces+2]);
											
											if (inputSemantic["NORMAL"]) {
												normals.push_back(data->mesh[str]->normals[face[e]*vPerFaces]);
												normals.push_back(data->mesh[str]->normals[face[e]*vPerFaces+1]);
												normals.push_back(data->mesh[str]->normals[face[e]*vPerFaces+2]);
											}
											if (inputSemantic["TEXCOORD"]) {
												texcoord.push_back(data->mesh[str]->texcoord[face[e]*vPerFaces]);
												texcoord.push_back(data->mesh[str]->texcoord[face[e]*vPerFaces+1]);
											}
											indices.push_back((vertices.size()/vPerFaces)-1);
										} else {
											indices.push_back(indices[indv]);
										}
									}
									
									/*
									face_tmp[0] = face[0];
									face_tmp[1] = face[1];
									face_tmp[2] = face[2];
									
									if (inputSemantic["NORMAL"]) {
										face[3] = polysArrays[str][i+nOffset];
										face[4] = polysArrays[str][i+semanticCount+nOffset];
										face[5] = polysArrays[str][i+semanticCount*2+nOffset];
										
										for (int e=3; e<6; e++) {
											unsigned int normalIndice = face[e];
											if(normalIndice >= verticesCount) {
												for (unsigned int j=0; j<vPerFaces; j++) {
													data->mesh[str]->vertices[face_tmp[e-3]*vPerFaces+j] = data->mesh[str]->vertices[face[j]*vPerFaces+e];
												}
												face_tmp[e-3] = normalIndice;
											}
										}
										
										for (unsigned int e=0; e<vPerFaces; e++) {
											data->mesh[str]->normals[face_tmp[0]*vPerFaces+e] = floatArrays[normalSource][face[3]*vPerFaces+e];
											data->mesh[str]->normals[face_tmp[1]*vPerFaces+e] = floatArrays[normalSource][face[4]*vPerFaces+e];
											data->mesh[str]->normals[face_tmp[2]*vPerFaces+e] = floatArrays[normalSource][face[5]*vPerFaces+e];
										}
									}
									
									// TODO
									if (inputSemantic["TEXCOORD"]) {
										face[6] = polysArrays[str][i+tOffset];
										face[7] = polysArrays[str][i+semanticCount+tOffset];
										face[8] = polysArrays[str][i+semanticCount*2+tOffset];
									}
									
									for (unsigned int e=0; e<vPerFaces; e++) {
										data->mesh[str]->indices.push_back(face_tmp[e]);
									}*/
								}
								
								data->mesh[str]->normals = normals;
								data->mesh[str]->vertices = vertices;
								data->mesh[str]->texcoord = texcoord;
								data->mesh[str]->indices = indices;
							} else {
								log(str, " polylist <p> count does not match expected size, mesh ignored");
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