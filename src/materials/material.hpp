#ifndef MAMMOTH3D_MATERIAL_HPP
#define MAMMOTH3D_MATERIAL_HPP

	#include <stack>
	#include <GL/glew.h>
	
	#include "../programs/program.hpp"
	#include "../core/texture.hpp"
	#include "../core/utils.hpp"

	namespace material {
		typedef enum {
			POLY_MODE,
			CULL_MODE,
			DEPTH_WRITE,
			DEPTH_TEST,
			PROGRAM,
			BLENDING
		}StateChangeType;
		
		class Material {
			public:
				Material() { 
					_poly_mode = GL_FILL;
					_cull_mode = GL_BACK;
					_depth_write = true;
					_depth_test = false;
					_blending = false;
					_prog = 0;	
					_line_width = 1.0f;
					
					_blend_func_src = GL_SRC_ALPHA;
					_blend_func_dst = GL_ONE_MINUS_SRC_ALPHA;
					_blend_equation = GL_FUNC_ADD;
				}
				
				~Material() { 

				}
				
				void setProgram(program::Program *prog) {
					if (prog != 0) {
						_prog = prog;
						_prog->registerUniform("alpha");
						_prog->registerUniform("mvp");
						
						bindTextures();
					}
				}
				
				void setPolyMode(GLenum polyMode) {
					_poly_mode = polyMode;
				}
				
				void setLineWidth(GLfloat width) {
					_line_width = width;
				}
				
				void setCullMode(GLenum cullMode) {
					_cull_mode = cullMode;
				}
				
				void setDepthWrite(bool depthWrite) {
					_depth_write = depthWrite;
				}
				
				void setDepthTest(bool depthTest) {
					_depth_test = depthTest;
				}
				
				void setBlending(bool blending) {
					_blending = blending;
				}
				
				void setBlendFunc(GLenum src, GLenum dst) {
					_blend_func_src = src;
					_blend_func_dst = dst;
				}
				
				void setBlendEquation(GLenum equation) {
					_blend_equation = equation;
				}
				
				void setTexture(core::Texture *texture, unsigned int id = 0) {
					_textures.resize(id+1);
					_textures[id] = texture;
					
					bindTextures();
				}
				
				core::Texture *getTexture(unsigned int index = 0) const {
					if (index < _textures.size()) {
						return _textures[index];
					}
				}
                
                program::Program *getProgram() const {
                    return _prog;
                }
				
				void bindTextures() {
					if (_prog) {
						glUseProgram(_prog->_id);

						for (unsigned int i = 0; i < _textures.size(); i++) {
							std::string prefix = "t";
							prefix = prefix + core::utils::toString(i);
							
							_prog->registerUniform(prefix);
							_prog->setUniform1i(prefix, i);
						}
					}
				}
				
				void update() {
					while (!_states.empty())
						_states.pop();

					_states.push(PROGRAM);
					_states.push(POLY_MODE);
					_states.push(CULL_MODE);
					_states.push(DEPTH_WRITE);
					_states.push(DEPTH_TEST);
					_states.push(BLENDING);
				}
			
				program::Program *_prog;
				GLenum _poly_mode, _cull_mode;
				GLfloat _line_width;
				bool _depth_write, _depth_test, _blending;
				
				GLenum _blend_func_src, _blend_func_dst, _blend_equation;
				
				std::vector<core::Texture *> _textures;
				
				std::stack<StateChangeType> _states;
		};
	}
	
#endif