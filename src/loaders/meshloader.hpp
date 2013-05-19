#ifndef MESHLOADER_HPP
#define MESHLOADER_HPP

	#include <fstream>
	#include <iostream>
	#include <vector>
	#include <map>
	
	#include <GL/glew.h>
	
	#include "../objects/mesh.hpp"
	#include "collada/collada.hpp"
	
	namespace loader {
		class MeshLoader {
			public:
				MeshLoader() { 
				}
			
				~MeshLoader() { 
					std::map<std::string, std::vector<object::Mesh *> >::iterator meshsIt 
																= meshs.begin();
					for (meshsIt; meshsIt != meshs.end(); ++meshsIt) {
						if (!meshsIt->second.empty()) {
							for (unsigned int i = 0; i < meshsIt->second.size(); i++) {
								delete meshsIt->second[i];
							}
							meshsIt->second.clear();
						}
					}
					
					std::map<std::string, core::Geometry *>::iterator meshsGeomIt 
															= meshsGeom.begin();
					for (meshsGeomIt; meshsGeomIt != meshsGeom.end(); ++meshsGeomIt) {
						if (meshsGeomIt->second) {
							delete meshsGeomIt->second;
						}
						meshsGeomIt->second = 0;
					}
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

				object::Mesh *loadMesh(const std::string &fileName);
				
				std::map<std::string, core::Geometry *> meshsGeom;
				std::map<std::string, std::vector<object::Mesh *> > meshs;
			
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