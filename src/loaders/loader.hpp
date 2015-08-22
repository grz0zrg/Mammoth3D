#ifndef MAMMOTH3D_LOADER_HPP
#define MAMMOTH3D_LOADER_HPP

	#include <string>
	#include <map>
	#include <unordered_set>
	
	#include "../loaders/materialloader.hpp"
	#include "../loaders/meshloader.hpp"
	#include "../loaders/shaderloader.hpp"
	#include "../loaders/imageloader.hpp"
	#include "../loaders/textureloader.hpp"
	#include "../programs/program.hpp"
	#include "../scenegraph/meshnode.hpp"
	#include "../core/geometry.hpp"
	#include "../core/fbo.hpp"
    #include "../core/tbo.hpp"
	#include "../core/uniform_block.hpp"
	#include "../cameras/camera.hpp"
    #include "../objects/quad.hpp"
	#include "../objects/mesh.hpp"
	#include "../objects/bitmaptext.hpp"
	#include "../objects/screen_aligned_quad.hpp"
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
					
					for (auto fontsIt = _bfonts.begin(); fontsIt != _bfonts.end(); ++fontsIt) {
						if (fontsIt->second) {
							delete fontsIt->second;
						}
					}
					
					for (auto it = _geoms.begin(); it != _geoms.end(); ++it) {
						delete *it;
					}
					
					for (auto it = _mnodes.begin(); it != _mnodes.end(); ++it) {
						delete *it;
					}
					
					for (auto it = _ublocks.begin(); it != _ublocks.end(); ++it) {
						delete *it;
					}
                    
					for (auto it = _meshs.begin(); it != _meshs.end(); ++it) {
						delete *it;
					}
					
					for (auto it = _btexts.begin(); it != _btexts.end(); ++it) {
						delete *it;
					}
					
					for (auto it = _cams.begin(); it != _cams.end(); ++it) {
						delete *it;
					}
                    
					for (auto it = _tbos.begin(); it != _tbos.end(); ++it) {
						delete *it;
					}
					
					for (auto it = _fbos.begin(); it != _fbos.end(); ++it) {
						delete *it;
					}
					
					for (auto it = _saqs.begin(); it != _saqs.end(); ++it) {
						delete *it;
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
				std::unordered_set<scenegraph::MeshNode *> _mnodes;
				std::unordered_set<core::UniformBlock *> _ublocks;
				std::unordered_set<object::BitmapText *> _btexts;
				std::unordered_set<object::ScreenAlignedQuad *> _saqs;
                std::unordered_set<object::Mesh *> _meshs;
				std::unordered_set<camera::Camera *> _cams;
				std::unordered_set<core::Fbo *> _fbos;
                std::unordered_set<core::Tbo *> _tbos;
				std::unordered_set<core::Geometry *> _geoms;
				
			public:
				static Loader *getInstance()
				{
					if (!_singleton) {
						_singleton =  new Loader;
						
						std::atexit(free);
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
                
                template <class T>
                core::Texture *getNewTextureBuffer(const std::vector<T>& data, GLenum format = GL_R32F, 
                                            GLenum usage = GL_STATIC_DRAW) {
                    getNewTextureBuffer(getNewTbo(data, format, usage));
                }                                            

                core::Texture *getNewTextureBuffer(const core::Tbo *tbo = 0) {
                    auto texture = _texture_loader->createEmptyTexture(0, 0, GL_TEXTURE_BUFFER);
                    
                    if (tbo) {
                        texture->bindBuffer(tbo->_format, tbo->_id);
                    }
                    
                    return texture;
                }
                
				core::Texture *getNewTexture(int width, int height, GLenum type = GL_TEXTURE_2D) {
					return _texture_loader->createEmptyTexture(width, height, type);
				}
				
				core::Texture *getTexture(const std::string &fileName) {
					return _texture_loader->loadTexture(_image_loader->loadImage(fileName));
				}
				
				scenegraph::MeshNode *getNewMeshNode() {
					auto mnode = new scenegraph::MeshNode();
					
					_mnodes.insert(mnode);
					
					return mnode;
				}
                
				object::Mesh *getNewMesh(core::Geometry *geom = 0, material::Material *mat = 0, const std::map<core::Texture *, unsigned int> &textures = std::map<core::Texture *, unsigned int>()) {
					auto m = new object::Mesh(geom, mat, textures);
					
					_meshs.insert(m);
					
					return m;
				}
                
				object::Mesh *getNewQuad(const std::map<core::Texture *, unsigned int> &textures = std::map<core::Texture *, unsigned int>()) {
					auto q = new object::Quad();
                    
					for (auto it = textures.begin(); it != textures.end(); ++it) {
						q->setTexture(it->first, it->second);
					}
					
					_meshs.insert(q);
					
					return q;
				}
				
				core::UniformBlock *getNewUniformBlock(const std::string &bname, 
                                                        const std::map<std::string, float> &uniforms = std::map<std::string, float>(),
														program::Program *target = 0,
                                                        bool dynamic = false) {

					auto ublock = new core::UniformBlock(bname);
					
					for (auto it = uniforms.begin(); it != uniforms.end(); ++it) {
						ublock->set(it->first, it->second);
					}
                    
					ublock->initialize(dynamic);
                    ublock->update();

					if (target) {
						target->bindUniformBlock(ublock);
					}
	
					_ublocks.insert(ublock);
					
					return ublock;
				}
                
				object::BitmapText *getNewBitmapText(const std::string &fileName, const std::string& text = "", 
                                                        int r = 255, int g = 255, int b = 255, float a = 0.5f) {
					getNewBitmapText(getBitmapFont(fileName), text, r, g, b, a);
				}
				
				object::BitmapText *getNewBitmapText(font::BitmapFont *font, const std::string& text = "", 
                                                        int r = 255, int g = 255, int b = 255, float a = 0.5f) {
					auto btext = new object::BitmapText(font);
                    btext->setColor(r, g, b);
                    btext->setAlphaTreshold(a);
                    
                    if (!text.empty()) {
                        btext->setText(text);
                    }
					
					_btexts.insert(btext);
					
					return btext;
				}
				
				core::Geometry *getNewGeometry() {
					auto geom = new core::Geometry();
					
					_geoms.insert(geom);
					
					return geom;
				}
				
				core::Fbo *getNewFbo(core::Texture *texture, bool multisampling = false, 
                                                    bool depth_buffer = true) {
					auto fbo = new core::Fbo(texture, multisampling, depth_buffer);
					
					_fbos.insert(fbo);
					
					return fbo;
				}
                
                template <class T>
				core::Tbo *getNewTbo(const std::vector<T>& data, GLenum format = GL_R32F, 
                                            GLenum usage = GL_STATIC_DRAW) {
					auto tbo = new core::Tbo(data, format, usage);
					
					_tbos.insert(tbo);
					
					return tbo;
				}
				
				camera::Camera *getNewCamera(camera::Type type, float fov = 60.0f, float aspect = 1.0f, 
						float near = 0.1f, float far = 10000.0f) {
					auto cam = new camera::Camera(type, fov, aspect, near, far);
					
					_cams.insert(cam);
					
					return cam;
				}
				
				object::ScreenAlignedQuad *getNewScreenAlignedQuad(program::Program *p = 0) {
					auto saq = new object::ScreenAlignedQuad();
					
					if (p) {
						saq->setProgram(p);
					}
					
					_saqs.insert(saq);
					
					return saq;
				}
				
				font::BitmapFont *getBitmapFont(const std::string &fileName) {
					auto fontElement = _bfonts.find(fileName);
					
					if (fontElement != _bfonts.end()) {
						return fontElement->second;
					}
					
					auto _font = new font::BitmapFont(getTexture(fileName));
					_bfonts[fileName] = _font;
					
					return _font;
				}
		};
	}
	
#endif