#ifndef QUAD_OBJECT_HPP
#define QUAD_OBJECT_HPP
	
	#include "mesh.hpp"

	#include "../window/window.hpp"

	namespace object {
		class Quad : public Mesh {
			public:
				Quad(bool screen_aligned = false) {
					this->geom = new core::Geometry();
					
					float v[] = {-1.0f, -1.0f, 0.0f,
								 1.0f, -1.0f, 0.0f,
								 1.0f, 1.0f, 0.0f,
								 -1.0f, 1.0f, 0.0f};
					
					float c[] = {0.0f, 0.0f, 
								 1.0f, 0.0f,
								 1.0f, 1.0f,
								 0.0f, 1.0f};
								 
					for (unsigned int i = 0; i < 12; i++) {
						this->geom->vertices.push_back(v[i]);
					}
									
					for (unsigned int i = 0; i < 8; i++) {
						this->geom->uvs.push_back(c[i]);
					}
					
					this->geom->generateVbo();
					
					this->mat = new material::Material();
					this->mat->setDepthWrite(true);

					//this->mat->setBlending(true);
					
					screen_aligned_texture = 0;
					
					if (screen_aligned) {	
						window::Window *screen = window::Window::getInstance();
						
						core::Image *empty_image = new core::Image(screen->getWidth()*screen->getAASamples(), 
																screen->getHeight()*screen->getAASamples());
						screen_aligned_texture = new core::Texture(empty_image, false, false, false);
						delete empty_image;
					
						this->type = QUAD_ALIGNED;
						this->mat->setCullMode(GL_NONE);
						this->mat->setTexture(screen_aligned_texture);
					} else {
						this->type = QUAD;
					}
				}
				
				void setTextureSize(int w, int h) {
					if (this->type == QUAD_ALIGNED) {
						if (screen_aligned_texture) {
							delete screen_aligned_texture;
						}
						
						core::Image *empty_image = new core::Image(w, h);
						screen_aligned_texture = new core::Texture(empty_image, false, false, false);
						delete empty_image;
						
						this->mat->setTexture(screen_aligned_texture);
					}
				}
				
				~Quad() {
					if (screen_aligned_texture) {
						delete screen_aligned_texture;
					}
					
					delete this->mat;
					delete this->geom;
				}
				
				core::Texture *screen_aligned_texture;
		};
	}
	
#endif