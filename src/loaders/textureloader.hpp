#ifndef MAMMOTH3D_TEXTURELOADER_HPP
#define MAMMOTH3D_TEXTURELOADER_HPP

	#include <fstream>
	#include <iostream>
	#include <vector>
	#include <map>
	
	#include <GL/glew.h>

	#include "../core/math.hpp"
	#include "../core/texture.hpp"
	#include "../core/image.hpp"
	
	namespace loader {
		class TextureLoader {
			public:
				TextureLoader() { 
				}
			
				~TextureLoader() { 
					for (unsigned int i = 0; i < _textures.size(); i++) {
						if (_textures[i]) {
							delete _textures[i];
						}
					}
				}
				
				static TextureLoader *getInstance()
				{
					if (!_singleton) {
						_singleton =  new TextureLoader;
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
				
				core::Texture *loadTexture(core::Image *image);
				core::Texture *createEmptyTexture(int width, int height);
				
				std::vector<core::Texture *> _textures;

			private:
				template <typename T>
				void log(const std::string &str, T param) {
					std::cout << "[TextureLoader ] " << str << param << std::endl;
				}
					
				void log(const char *str) {
					std::cout << "[TextureLoader ] " << str << std::endl;
				}
				
			TextureLoader(const TextureLoader&);
			void operator=(const TextureLoader&);
			static TextureLoader *_singleton;
		};
	}

#endif