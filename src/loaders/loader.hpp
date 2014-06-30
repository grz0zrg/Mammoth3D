#ifndef MAMMOTH3D_LOADER_HPP
#define MAMMOTH3D_LOADER_HPP

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
					_shader_loader   = loader::ShaderLoader::getInstance();
					_mesh_loader     = loader::MeshLoader::getInstance();
					_material_loader = loader::MaterialLoader::getInstance();
					_image_loader    = loader::ImageLoader::getInstance();
					_texture_loader  = loader::TextureLoader::getInstance();
				}
				
				~Loader() {
					_shader_loader->free();
					_mesh_loader->free();
					_material_loader->free();
					_image_loader->free();
					_texture_loader->free();
					
					std::map<std::string, font::BitmapFont *>::iterator fontsIt 
																= _bfonts.begin();
					for (fontsIt; fontsIt != _bfonts.end(); ++fontsIt) {
						if (fontsIt->second) {
							delete fontsIt->second;
						}
					}
					_bfonts.clear();
				}
				
				Loader(const Loader&);
				void operator=(const Loader&);
				static Loader *_singleton;
				
				loader::ShaderLoader *_shader_loader;
				loader::MeshLoader *_mesh_loader;
				loader::MaterialLoader *_material_loader;
				loader::ImageLoader *_image_loader;
				loader::TextureLoader *_texture_loader;
				
				std::map<std::string, font::BitmapFont *> _bfonts;

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
					_shader_loader->compileShaderFile(GL_VERTEX_SHADER, 
																vertexShader);
					_shader_loader->compileShaderFile(GL_FRAGMENT_SHADER, 
																fragmentShader);
					return _shader_loader->buildProgram();
				}
				
				scenegraph::MeshNode *getMeshNode(const std::string &fileName) {
					return _mesh_loader->loadMesh(fileName);
				}
				
				material::Material *getNewMaterial() {
					return _material_loader->createMaterial();
				}
				
				core::Texture *getNewTexture(int width, int height) {
					return _texture_loader->createEmptyTexture(width, height);
				}
				
				core::Texture *getTexture(const std::string &fileName) {
					return _texture_loader->loadTexture(_image_loader->loadImage(fileName));
				}
				
				font::BitmapFont *getBitmapFont(const std::string &fileName) {
					std::map<std::string, font::BitmapFont *>::iterator fontElement = 
													_bfonts.find(fileName);
					
					if (fontElement != _bfonts.end()) {
						return fontElement->second;
					}
					
					font::BitmapFont *_font = new font::BitmapFont(getTexture(fileName));
					_bfonts[fileName] = _font;
					
					return _font;
				}
		};
	}
	
#endif