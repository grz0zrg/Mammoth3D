#ifndef MATERIAL_HPP
#define MATERIAL_HPP

	#include <stack>
	#include <GL/glew.h>
	
	namespace material {
		typedef enum {
			POLY_MODE,
			CULL_MODE,
			DEPTH_WRITE,
			DEPTH_TEST,
			PROGRAM
		}StateChangeType;
		
		class Material {
			public:
				Material() { 
					polyMode = GL_FILL;
					cullMode = GL_BACK;
					depthWrite = false;
					depthTest = false;
					program = 0;	
				}
				
				~Material() { 

				}
				
				void setProgram(GLuint program) {
					if (program != 0) {
						this->program = program;
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
				
				void update() {
					while (!states.empty())
						states.pop();

					states.push(PROGRAM);
					states.push(POLY_MODE);
					states.push(CULL_MODE);
					states.push(DEPTH_WRITE);
					states.push(DEPTH_TEST);
				}
			
				GLuint program;
				GLenum polyMode, cullMode;
				bool depthWrite, depthTest;
				
				std::stack<StateChangeType> states;

			private:

		};
	}
	
#endif