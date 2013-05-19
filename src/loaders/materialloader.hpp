#ifndef MATERIALLOADER_HPP
#define MATERIALLOADER_HPP

	#include <vector>
	
	#include "../materials/material.hpp"
	
	namespace loader {
		class MaterialLoader {
			public:
				MaterialLoader() { 
				}
			
				~MaterialLoader() { 
					for (unsigned int i = 0; i < materials.size(); i++) {
						delete materials[i];
					}
					materials.clear();
				}
				
				static MaterialLoader *getInstance()
				{
					if (!_singleton) {
						_singleton =  new MaterialLoader;
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

				material::Material *createMaterial() {
					material::Material *material = new material::Material();
					materials.push_back(material);
					return material;
				}
				
				std::vector<material::Material *> materials;
			
			private:
				MaterialLoader(const MaterialLoader&);
				void operator=(const MaterialLoader&);
				static MaterialLoader *_singleton;
		};
	}

#endif