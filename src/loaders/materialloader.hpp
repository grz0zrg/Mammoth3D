#ifndef MAMMOTH3D_MATERIALLOADER_HPP
#define MAMMOTH3D_MATERIALLOADER_HPP

	#include <vector>
	
	#include "../materials/material.hpp"
	
	namespace loader {
		class MaterialLoader {
			public:
				MaterialLoader() { 
				}
			
				~MaterialLoader() { 
					for (unsigned int i = 0; i < _materials.size(); i++) {
						delete _materials[i];
					}
					_materials.clear();
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
					_materials.push_back(material);
					return material;
				}
				
				std::vector<material::Material *> _materials;
			
			private:
				MaterialLoader(const MaterialLoader&);
				void operator=(const MaterialLoader&);
				static MaterialLoader *_singleton;
		};
	}

#endif