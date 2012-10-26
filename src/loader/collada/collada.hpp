#ifndef COLLADA_HPP
#define COLLADA_HPP

	#include <iostream>  
	#include <vector>
	#include <map>
	#include <fstream>

	#include "tinyxml2/tinyxml2.h"
	
	/* quick&dirty collada importer
	 * triangles are the only primitives supported
	 * <vcount> always assumed to be 3 in case of <polylist> tag
	 * also don't support <accessor ...> and many others things to simplify :)
	 */
	namespace loader {
		class Collada : public tinyxml2::XMLVisitor {
			public:
				Collada(const char *fileName) {
					data = 0;

					loadFile(fileName);
				};
				
				~Collada() {
					floatArrays.clear();
					vcountArrays.clear();
					polysArrays.clear();
					
					clearInputSemanticData();

					if(data) {
						meshIt = data->mesh.begin();
						while(meshIt != data->mesh.end()) {
							if(meshIt->second) {
								delete meshIt->second;
							}
							meshIt++;
						}
						delete data;
					}
				};
				
				// this export meshs datas to a binary format (.mm)
				void exportMeshsTo(const char *dir) {
					std::string directory(dir);
					if (directory.at(directory.length()-1) != '/') {
						directory.append("/");
					}
					
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
				
			private:
				void loadFile(const char *fileName) {
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
							}
						}
						libType = libTypes[++i];
					}
					
					log("Done.");
				}
				
				void parseGeometries(const tinyxml2::XMLElement* element) {
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
						const tinyxml2::XMLElement* geomChild = 
												element->FirstChildElement();
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
				
				void parseMeshData(const std::string &str, const tinyxml2::XMLElement* element) {
					while(element) {
						const tinyxml2::XMLElement* sourceChild = 
												element->FirstChildElement();
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
										std::vector<std::string> floatData = 
																splitText(text);
										
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
										const std::string semantic = sourceChild->Attribute("semantic");
										std::string source = sourceChild->Attribute("source");
										
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
										std::vector<std::string> vcountData = 
																splitText(text);

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
										std::vector<std::string> polysData = 
																splitText(text);

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
						
				void clearInputSemanticData() {
					inputSemanticIt = inputSemantic.begin();
					while(inputSemanticIt != inputSemantic.end()) {
						if(inputSemanticIt->second) {
							delete inputSemanticIt->second;
						}							
						inputSemanticIt++;
					}	
					inputSemantic.clear();
				}
				
				template <typename T>
				void log(const std::string &str, T param) {
					std::cout << "[Collada] " << str << param << std::endl;
				}
					
				void log(const char *str) {
					std::cout << "[Collada] " << str << std::endl;
				}
				
				// split text, delimited by whitespaces
				std::vector<std::string> splitText(const char *str) {
					std::string line;
					std::vector<std::string> tokens;

					while(1) {
						while(isspace(*str) && *str) str++;
						
						if(!*str) {
							break;
						}

						line += *str++;
						
						if(isspace(*str) || !*str) {
							tokens.push_back(line);
							line.clear();
						}
					}

					return tokens;
				}

				typedef struct {
					std::string name;
					std::vector<float> vertices;
					std::vector<float> normals;
					std::vector<unsigned /*long*/ int> indices;

					//std::map<std::string, std::string> positions;
				} Mesh;

				typedef struct {
					std::map<std::string, Mesh*> mesh;
				} Data;
				
				typedef struct {
					std::string source;
					int offset;
				} InputSemanticData;
				
				// used to store float arrays with their source link
				std::map<std::string, std::vector<float> > floatArrays;
				// used to store vcount/polys arrays with their mesh name
				std::map<std::string, std::vector<unsigned long int> > vcountArrays;
				std::map<std::string, std::vector<unsigned long int> > polysArrays;
				
				// used to store <input semantic ...> data per meshs
				std::map<std::string, InputSemanticData*> inputSemantic;
				
				// iterators
				std::map<std::string, std::vector<float> >::iterator floatArraysIt;
				std::map<std::string, Mesh*>::iterator meshIt;
				std::map<std::string, InputSemanticData*>::iterator inputSemanticIt;
				

				enum e_libType{ ANIMATIONS, ASSET, CONTROLLERS, CAMERAS,
								EFFECTS, GEOMETRIES, IMAGES, LIGHTS,
								MATERIALS, SCENE, VISUAL_SCENES, NONE};
								
				tinyxml2::XMLDocument doc;
				Data *data;
				
				// define collada libraries to parse
				const static char *libNames[];
				const static e_libType libTypes[];
				
				const static std::string meshFilenameExt;
		};
	}

#endif