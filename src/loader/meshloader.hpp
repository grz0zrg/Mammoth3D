#ifndef MESHLOADER_HPP
#define MESHLOADER_HPP

	#include <fstream>
	#include <iostream>
	#include <vector>
	
	namespace loader {
		class MeshLoader {
			public:
				MeshLoader(const std::string &fileName) { 
					indsType = sizeof(unsigned int);

					loadMesh(fileName); 
				};
				
				~MeshLoader() { };
				
			private:
				void loadMesh(const std::string &fileName);

				template <typename T>
				void logPretty(const std::string &str, T param) {
					std::cout << "[MeshLoader] " << str << "\"" << 
								param << "\"" << std::endl;
				}
				
				template <typename T>
				void log(const std::string &str, T param) {
					std::cout << "[MeshLoader] " << str << param << std::endl;
				}
					
				void log(const char *str) {
					std::cout << "[MeshLoader] " << str << std::endl;
				}
				
			char indsType;
			
			std::string name;
			
			public: // temp
			std::vector<unsigned int> indices;
			std::vector<float> vertices;
			std::vector<float> normals;
		};
	}

#endif