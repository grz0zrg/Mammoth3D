#ifndef MATERIAL_HPP
#define MATERIAL_HPP

	#include <stack>
	#include <GL/glew.h>
	
	#include "../programs/program.hpp"
	
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
				}
				
				~Material() { 

				}
				
				void setProgram(program::Program *prog) {
					if (prog != 0) {
						this->prog = prog;
						this->prog->registerUniform("alpha");
						this->prog->registerUniform("mvp");
					}
				}
				
				void setPolyMode(GLenum polyMode) {
					this->polyMode = polyMode;
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
				bool depthWrite, depthTest, blending;
				
				std::stack<StateChangeType> states;

			private:

		};
	}
	
#endif