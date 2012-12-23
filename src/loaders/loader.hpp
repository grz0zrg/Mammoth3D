#ifndef LOADER_HPP
#define LOADER_HPP

	#include <string>
	#include <map>

	#include "../loaders/meshloader.hpp"
	#include "../loaders/shaderloader.hpp"
	#include "../objects/mesh.hpp"

	namespace loader {
		class Loader {
			private:
				Loader() { 
					shaderloader = loader::ShaderLoader::getInstance();
				}
				
				~Loader() {
					std::map<std::string, loader::MeshLoader *>::iterator itMeshs;
					for(itMeshs=meshs.begin() ; itMeshs != meshs.end(); itMeshs++) {
						if ((*itMeshs).second != 0) {
							delete (*itMeshs).second;
						}
					}
					shaderloader->free();
				}
				
				Loader(const Loader&);
				void operator=(const Loader&);
				static Loader *_singleton;
				
				loader::ShaderLoader *shaderloader;
				std::map<std::string, loader::MeshLoader *> meshs;
				
			public:
				static Loader *getInstance()
				{
					if (!_singleton) {
						_singleton =  new Loader;
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
				
				GLuint loadProgram(const std::string &vertexShader, 
									const std::string &fragmentShader) {
					shaderloader->compileShaderFile(GL_VERTEX_SHADER, vertexShader);
					shaderloader->compileShaderFile(GL_FRAGMENT_SHADER, fragmentShader);
					return shaderloader->buildProgram();
				}
				
				object::Mesh *loadMesh(const std::string &fileName) {
					if (meshs.find(fileName) == meshs.end()) {
						meshs[fileName] = 0;
						meshs[fileName] = new loader::MeshLoader(fileName);
						return (new object::Mesh(meshs[fileName]));
					}
					return (new object::Mesh(meshs[fileName]));
				}
		};
	}
	
#endif