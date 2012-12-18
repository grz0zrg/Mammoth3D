#ifndef MESHLOADER_HPP
#define MESHLOADER_HPP

	#include <fstream>
	#include <iostream>
	#include <vector>
	
	#include <GL/glew.h>
	
	namespace loader {
		class MeshLoader {
			public:
				MeshLoader(const std::string &fileName) { 
					indsType = sizeof(unsigned int);

					loadMesh(fileName); 
				};
				
				~MeshLoader() { 
					glDeleteBuffers(1, &ibo);
					glDeleteBuffers(1, &vbo);
				};
				
				GLuint vbo, ibo;
				unsigned int indicesCount;

			private:
				void loadMesh(const std::string &fileName);
				
				int createBuffers();

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
			public:
			std::vector<unsigned int> indices;
			std::vector<float> vertices;
			std::vector<float> normals;
		};
	}

#endif