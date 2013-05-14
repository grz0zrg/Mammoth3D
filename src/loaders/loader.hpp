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
				}
				
				~Loader() {
					shaderloader->free();
					meshloader->free();
				}
				
				Loader(const Loader&);
				void operator=(const Loader&);
				static Loader *_singleton;
				
				loader::ShaderLoader *shaderloader;
				loader::MeshLoader *meshloader;

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
				
				program::Program *loadProgram(const std::string &vertexShader, 
									const std::string &fragmentShader) {
					shaderloader = loader::ShaderLoader::getInstance();
					shaderloader->compileShaderFile(GL_VERTEX_SHADER, vertexShader);
					shaderloader->compileShaderFile(GL_FRAGMENT_SHADER, fragmentShader);
					return shaderloader->buildProgram();
				}
				
				object::Mesh *loadMesh(const std::string &fileName) {
					meshloader = loader::MeshLoader::getInstance();
					meshloader->loadMesh(fileName);
					if (meshloader->loaded) {
						return (new object::Mesh(meshloader));
					} else {
						return 0;
					}
				}
		};
	}
	
#endif