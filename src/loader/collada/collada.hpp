#ifndef COLLADA_HPP
#define COLLADA_HPP

	#include <iostream>
	#include <map>

	#include "tinyxml2/tinyxml2.h"
	
	namespace loader {
		class Collada : public tinyxml2::XMLVisitor {
			public:
				Collada(const char *fileName) {
					data = 0;

					loadFile(fileName);
				};
				
				~Collada() {
					if(data) {
						delete data;
					}
				};
				
			private:
				void loadFile(const char *fileName) {
					log("Loading: ", fileName);
					int ret = doc.LoadFile(fileName);
					if (ret != tinyxml2::XML_NO_ERROR) {
						log("LoadFile error code: ", ret);
						return;
					}
						
					tinyxml2::XMLElement* rootElement = doc.FirstChildElement("COLLADA");
					
					data = new Data;
					
					// traverse all libraries
					int i = 0;
					libType = libTypes[i];
					while(libType != NONE) {
						tinyxml2::XMLElement* element = rootElement->FirstChildElement(libNames[i]);
						if(element) {
							log("Parsing element: ", libNames[i]);
							element->Accept(this);
						}
						libType = libTypes[++i];
					}
				}
				
				virtual bool VisitEnter(const tinyxml2::XMLElement& element, 
										const tinyxml2::XMLAttribute* firstAttribute) {
						std::string elementName(element.Name());
						if(&element) {
						//	log(element.Name());
						}
						
						if(firstAttribute) {
							
						}

						switch(libType) {
							case GEOMETRIES:
								//if (elementName == "geometry")
								break;

							default: break;
						}
					/*log(element.Name());
					if(firstAttribute)
					log(firstAttribute->Value());*/
					return true;
				}
				
				virtual bool VisitExit(const tinyxml2::XMLElement &element) {
					return true;
				}
				
				template <typename T>
				void log(const std::string &str, T param) {
					std::cout << "[COLLADA] " << str << param << std::endl;
				}
					
				void log(const char *str) {
					std::cout << "[COLLADA] " << str << std::endl;
				}

				typedef struct {
					std::string id;

					//std::map<std::string, std::string> positions;
				} Mesh;

				typedef struct {
					std::map<std::string, Mesh*> mesh;
				} Data;

				// used to parse each 'libraries'
				enum e_libType{ ANIMATIONS, ASSET, CONTROLLERS, CAMERAS,
								EFFECTS, GEOMETRIES, IMAGES, LIGHTS,
								MATERIALS, SCENE, VISUAL_SCENES, NONE};
								
				tinyxml2::XMLDocument doc;
				Data *data;
				e_libType libType;
				
				const static char *libNames[];
				const static e_libType libTypes[];
		};
	}

#endif