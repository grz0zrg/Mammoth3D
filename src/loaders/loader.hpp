#ifndef LOADER_HPP
#define LOADER_HPP

	#include <string>
	#include <map>

	#include "../loaders/materialloader.hpp"
	#include "../loaders/meshloader.hpp"
	#include "../loaders/shaderloader.hpp"
	#include "../objects/mesh.hpp"

	namespace loader {
		class Loader {
			private:
				Loader() { 
					shaderLoader   = loader::ShaderLoader::getInstance();
					meshLoader     = loader::MeshLoader::getInstance();
					materialLoader = loader::MaterialLoader::getInstance();
				}
				
				~Loader() {
					shaderLoader->free();
					meshLoader->free();
					materialLoader->free();
				}
				
				Loader(const Loader&);
				void operator=(const Loader&);
				static Loader *_singleton;
				
				loader::ShaderLoader *shaderLoader;
				loader::MeshLoader *meshLoader;
				loader::MaterialLoader *materialLoader;

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
				
				program::Program *getProgram(const std::string &vertexShader, 
									const std::string &fragmentShader) {
					shaderLoader->compileShaderFile(GL_VERTEX_SHADER, 
																vertexShader);
					shaderLoader->compileShaderFile(GL_FRAGMENT_SHADER, 
																fragmentShader);
					return shaderLoader->buildProgram();
				}
				
				object::Mesh *getMesh(const std::string &fileName) {
					return meshLoader->loadMesh(fileName);
				}
				
				material::Material *getNewMaterial() {
					return materialLoader->createMaterial();
				}
		};
	}
	
#endif