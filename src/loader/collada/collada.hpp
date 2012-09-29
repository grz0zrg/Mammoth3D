#ifndef COLLADA_HPP
#define COLLADA_HPP

	#include <iostream>  
	#include <vector>
	#include <map>
	#include <cstring>
	#include <cstdlib>
	#include <cctype>

	#include "tinyxml2/tinyxml2.h"
	
	namespace loader {
		class Collada : public tinyxml2::XMLVisitor {
			public:
				Collada(const char *fileName) {
					data = 0;

					loadFile(fileName);
				};
				
				~Collada() {
					cleanFloatArraysMap();

					if(data) {
						meshIt = data->mesh.begin();
						while(meshIt != data->mesh.end()) {
							if(meshIt->second) {
								delete meshIt->second;
							}
						}
						delete data;
					}
				};
				
			private:
				void loadFile(const char *fileName) {
					log("Parsing: ", fileName);
					int ret = doc.LoadFile(fileName);
					if (ret != tinyxml2::XML_NO_ERROR) {
						log("LoadFile error code: ", ret);
						return;
					}
						
					tinyxml2::XMLElement* rootElement = doc.FirstChildElement("COLLADA");
					const char *version = rootElement->Attribute("version");
					if(version) {
						log("Collada schema version: ", version);
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
							}
						}
						libType = libTypes[++i];
					}
					
					// important step, this will resolve collada informations into basic mesh data
					processData();
				}
				
				void parseGeometries(const tinyxml2::XMLElement* element) {
					const tinyxml2::XMLElement* child = element;
					while(child) {
						element = child;
						child = child->NextSiblingElement();

						const char *elementName = element->Name();
						const char *meshId = element->Attribute("id");
						const char *meshName = element->Attribute("name");
						if(strcmp(elementName, "geometry") != 0 || 
											element->NoChildren() ||
											!meshId) {
							continue;
						}
						
						// parse <geometry ...> childs
						const tinyxml2::XMLElement* geomChild = 
												element->FirstChildElement();
						while(geomChild) {
							const char *childName = geomChild->Name();

							// <mesh> content
							if(!strcmp(childName, "mesh")) {
								std::string idStr(meshId);
								data->mesh[idStr] = new Mesh;
								if(meshName) {
									data->mesh[idStr]->name = meshName;
								}
								
								parseMeshData(idStr, geomChild->FirstChildElement());
							} else if (!strcmp(childName, "extra")) { // <extra>
							}

							geomChild = geomChild->NextSiblingElement();
						}
					}
				}
				
				void parseMeshData(const std::string &str, const tinyxml2::XMLElement* element) {
					while(element) {
						const tinyxml2::XMLElement* sourceChild = 
												element->FirstChildElement();
						// <source>
						if (!strcmp(element->Name(), "source")) {
							const char* id = element->Attribute("id");
							while(sourceChild) {
								//<float_array ...>
								if (!strcmp(sourceChild->Name(), "float_array")) {
									// parse&store float array
									const char *text = sourceChild->GetText();
									if(id && text) {
										std::vector<std::string> floatData = 
																splitText(text);
										
										unsigned int count = sourceChild->IntAttribute("count");
										unsigned int realSize = floatData.size();
										if(count) {
											if(count == realSize) {
												floatArrays[id] = new float[realSize];
												for(unsigned int i=0; i<realSize; i++) {
													floatArrays[id][i] = (float)atof(floatData[i].c_str());
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
						} else if (!strcmp(element->Name(), "vertices")) {
							while(sourceChild) {	
								// <input ...>
								if (!strcmp(sourceChild->Name(), "input")) {
									const char* semantic = element->Attribute("semantic");
									const char* source = element->Attribute("source");
									if(semantic && source) {
										semanticLink[std::string(semantic)] = std::string(source);
									}
								}
								
								sourceChild = sourceChild->NextSiblingElement();
							}
						// <polylist> <triangles> etc...
						} else if (!strcmp(element->Name(), "polylist") ||
									!strcmp(element->Name(), "triangles") ||
									!strcmp(element->Name(), "lines") ||
									!strcmp(element->Name(), "linestrips") ||
									!strcmp(element->Name(), "polygons") ||
									!strcmp(element->Name(), "trifans") ||
									!strcmp(element->Name(), "tristrips")) {
							unsigned int count = element->IntAttribute("count");
							while(sourceChild) {
								// <input ...>
								if (!strcmp(sourceChild->Name(), "input")) {
									const char* semantic = element->Attribute("semantic");
									const char* source = element->Attribute("source");
									if(semantic && source) {
										semanticLink[std::string(semantic)] = std::string(source);
									}
								// <vcount>
								} else if (!strcmp(sourceChild->Name(), "vcount")) {
									const char *text = sourceChild->GetText();
									if (text) {
										std::vector<std::string> floatData = 
																splitText(text);										
									}
								// <p>
								} else if (!strcmp(sourceChild->Name(), "p")) {
									const char *text = sourceChild->GetText();
									if (text) {
										std::vector<std::string> floatData = 
																splitText(text);										
									}
								}

								sourceChild = sourceChild->NextSiblingElement();
							}
						}
						
						element = element->NextSiblingElement();
					}
				}
				
				// 
				void processData() {
					
				}
				
				void cleanFloatArraysMap() {
					floatArraysIt = floatArrays.begin();
					while(floatArraysIt != floatArrays.end()) {
						if(floatArraysIt->second) {
							delete[] floatArraysIt->second;
						}
					}
					floatArrays.clear();
				}
				
				template <typename T>
				void log(const std::string &str, T param) {
					std::cout << "[COLLADA] " << str << param << std::endl;
				}
					
				void log(const char *str) {
					std::cout << "[COLLADA] " << str << std::endl;
				}
				
				// split text, delimited by whitespaces
				std::vector<std::string> splitText(const char *str) {
					std::string line;
					std::vector<std::string> tokens;

					while(*str) {
						while(isspace(*str) && *str) str++;

						line += *str++;
						
						if(isspace(*str) || !*str) {
							tokens.push_back(line);
							line.clear();
						}
					}

					return tokens;
				}

				typedef struct {
					const char *name;

					//std::map<std::string, std::string> positions;
				} Mesh;

				typedef struct {
					std::map<std::string, Mesh*> mesh;
				} Data;
				
				// used to store float arrays with their source link
				std::map<std::string, float*> floatArrays;
				
				// used to store semantic-source link
				std::map<std::string, std::string> semanticLink;
				
				// iterators
				std::map<std::string, float*>::iterator floatArraysIt;
				std::map<std::string, Mesh*>::iterator meshIt;

				enum e_libType{ ANIMATIONS, ASSET, CONTROLLERS, CAMERAS,
								EFFECTS, GEOMETRIES, IMAGES, LIGHTS,
								MATERIALS, SCENE, VISUAL_SCENES, NONE};
								
				tinyxml2::XMLDocument doc;
				Data *data;
				
				// define collada libraries to parse
				const static char *libNames[];
				const static e_libType libTypes[];
		};
	}

#endif