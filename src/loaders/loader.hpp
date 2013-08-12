#ifndef LOADER_HPP
#define LOADER_HPP

	#include <string>
	#include <map>

	#include "../loaders/materialloader.hpp"
	#include "../loaders/meshloader.hpp"
	#include "../loaders/shaderloader.hpp"
	#include "../loaders/imageloader.hpp"
	#include "../loaders/textureloader.hpp"
	#include "../objects/mesh.hpp"
	#include "../font/bitmapfont.hpp"

	namespace loader {
		class Loader {
			private:
				Loader() { 
					shaderLoader   = loader::ShaderLoader::getInstance();
					meshLoader     = loader::MeshLoader::getInstance();
					materialLoader = loader::MaterialLoader::getInstance();
					imageLoader    = loader::ImageLoader::getInstance();
					textureLoader  = loader::TextureLoader::getInstance();
				}
				
				~Loader() {
					shaderLoader->free();
					meshLoader->free();
					materialLoader->free();
					imageLoader->free();
					textureLoader->free();
					
					std::map<std::string, font::BitmapFont *>::iterator fontsIt 
																= bfonts.begin();
					for (fontsIt; fontsIt != bfonts.end(); ++fontsIt) {
						if (fontsIt->second) {
							delete fontsIt->second;
						}
					}
					bfonts.clear();
				}
				
				Loader(const Loader&);
				void operator=(const Loader&);
				static Loader *_singleton;
				
				loader::ShaderLoader *shaderLoader;
				loader::MeshLoader *meshLoader;
				loader::MaterialLoader *materialLoader;
				loader::ImageLoader *imageLoader;
				loader::TextureLoader *textureLoader;
				
				std::map<std::string, font::BitmapFont *> bfonts;

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
				
				core::Texture *getNewTexture(int width, int height) {
					return textureLoader->createEmptyTexture(width, height);
				}
				
				core::Texture *getTexture(const std::string &fileName) {
					return textureLoader->loadTexture(imageLoader->loadImage(fileName));
				}
				
				font::BitmapFont *getBitmapFont(const std::string &fileName) {
					std::map<std::string, font::BitmapFont *>::iterator fontElement = 
													bfonts.find(fileName);
					
					if (fontElement != bfonts.end()) {
						return fontElement->second;
					}
					
					font::BitmapFont *_font = new font::BitmapFont(getTexture(fileName));
					bfonts[fileName] = _font;
					
					return _font;
				}
		};
	}
	
#endif