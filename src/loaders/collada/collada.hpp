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
				Collada(const char *fileName);
				~Collada();
				
				// this export meshs datas to a binary format (.mm)
				void exportMeshsTo(const char *dir);
				
			private:
				void loadFile(const char *fileName);
				
				void parseEffects(const tinyxml2::XMLElement* element);

				void parseMaterials(const tinyxml2::XMLElement* element);
				
				void parseImages(const tinyxml2::XMLElement* element);
				
				void parseGeometries(const tinyxml2::XMLElement* element);
				
				void parseMeshData(const std::string &str, 
									const tinyxml2::XMLElement* element);
						
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

				} Effect;
				
				typedef struct {
					std::string effect;
				} Material;

				typedef struct {
					std::map<std::string, Mesh*> mesh;
					std::map<std::string, Effect*> effect;
					std::map<std::string, Material*> material;
					std::map<std::string, std::string> image;
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