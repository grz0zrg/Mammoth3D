#ifndef MAMMOTH3D_MESHLOADER_HPP
#define MAMMOTH3D_MESHLOADER_HPP

	#include <fstream>
	#include <iostream>
	#include <vector>
	#include <map>
	
	#include <GL/glew.h>
	
	#include "../loaders/materialloader.hpp"	
	#include "../loaders/imageloader.hpp"
	#include "../loaders/textureloader.hpp"
	#include "../scenegraph/meshnode.hpp"
	
	namespace loader {
		class MeshLoader {
			public:
				MeshLoader() { 
				}
			
				~MeshLoader();
				
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

				scenegraph::MeshNode *loadMesh(const std::string &fileName);
				
				std::map<std::string, std::vector<core::Geometry *> > _meshs_geom;
				std::map<std::string, std::vector<object::Mesh *> > _meshs;
				std::map<std::string, std::vector<scenegraph::MeshNode *> > _mesh_nodes;
				std::map<std::string, scenegraph::MeshNode *> _nodes;
			
			private:
				template <typename T>
				void log(const std::string &str, T param) {
					std::cout << "[MeshLoader  ] " << str << param << std::endl;
				}
					
				void log(const char *str) {
					std::cout << "[MeshLoader  ] " << str << std::endl;
				}
				
			MeshLoader(const MeshLoader&);
			void operator=(const MeshLoader&);
			static MeshLoader *_singleton;
		};
	}

#endif