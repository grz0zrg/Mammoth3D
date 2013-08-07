#ifndef MATERIAL_HPP
#define MATERIAL_HPP

	#include <stack>
	#include <GL/glew.h>
	
	#include "../programs/program.hpp"
	#include "../core/texture.hpp"

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
					polyMode = GL_FILL;
					cullMode = GL_BACK;
					depthWrite = true;
					depthTest = false;
					blending = false;
					prog = 0;	
					lineWidth = 1.0f;
					
					blendFuncSrc = GL_SRC_ALPHA;
					blendFuncDst = GL_ONE_MINUS_SRC_ALPHA;
					blendEquation = GL_FUNC_ADD;
				}
				
				~Material() { 

				}
				
				void setProgram(program::Program *prog) {
					if (prog != 0) {
						this->prog = prog;
						this->prog->registerUniform("alpha");
						this->prog->registerUniform("mvp");
						
						bindTextures();
					}
				}
				
				void setPolyMode(GLenum polyMode) {
					this->polyMode = polyMode;
				}
				
				void setLineWidth(GLfloat width) {
					lineWidth = width;
				}
				
				void setCullMode(GLenum cullMode) {
					this->cullMode = cullMode;
				}
				
				void setDepthWrite(bool depthWrite) {
					this->depthWrite = depthWrite;
				}
				
				void setDepthTest(bool depthTest) {
					this->depthTest = depthTest;
				}
				
				void setBlending(bool blending) {
					this->blending = blending;
				}
				
				void setBlendFunc(GLenum src, GLenum dst) {
					blendFuncSrc = src;
					blendFuncDst = dst;
				}
				
				void setBlendEquation(GLenum equation) {
					blendEquation = equation;
				}
				
				void setTexture(core::Texture *texture, unsigned int id = 0) {
					textures.resize(id+1);
					textures[id] = texture;
					
					bindTextures();
				}
				
				void bindTextures() {
					if (prog) {
						for (unsigned int i = 0; i < textures.size(); i++) {
							glUniform1i(glGetUniformLocation(prog->prog, "t"+i), i);
						}
					}
				}
				
				void update() {
					while (!states.empty())
						states.pop();

					states.push(PROGRAM);
					states.push(POLY_MODE);
					states.push(CULL_MODE);
					states.push(DEPTH_WRITE);
					states.push(DEPTH_TEST);
					states.push(BLENDING);
				}
			
				program::Program *prog;
				GLenum polyMode, cullMode;
				GLfloat lineWidth;
				bool depthWrite, depthTest, blending;
				
				GLenum blendFuncSrc, blendFuncDst, blendEquation;
				
				std::vector<core::Texture *> textures;
				
				std::stack<StateChangeType> states;
		};
	}
	
#endif