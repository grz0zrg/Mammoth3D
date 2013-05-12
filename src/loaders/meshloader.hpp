#ifndef MESHLOADER_HPP
#define MESHLOADER_HPP

	#include <fstream>
	#include <iostream>
	#include <vector>
	
	#include <GL/glew.h>
	
	namespace loader {
		class MeshLoader {
			public:
				MeshLoader() { 
					indsType = sizeof(unsigned int);
					loaded = false;
				}
			
				~MeshLoader() { 
				}
				
				static MeshLoader *getInstance()
				{
					if (!_singleton) {
						_singleton =  new MeshLoader;
					}

					return _singleton;
				}

				static void free()
				{
					if (_singleton)
					{
						delete _singleton;
						_singleton = 0;
					}
				}

				GLuint vbo, ibo;
				unsigned int indicesCount;
				
				void loadMesh(const std::string &fileName);

				char indsType;
				
				std::string name;
				std::vector<unsigned int> indices;
				std::vector<float> vertices;
				std::vector<float> normals;
				std::vector<float> texcoords;
				
				bool loaded;
			
			private:
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
				
			MeshLoader(const MeshLoader&);
			void operator=(const MeshLoader&);
			static MeshLoader *_singleton;
		};
	}

#endif