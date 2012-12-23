#ifndef MATERIAL_HPP
#define MATERIAL_HPP

	#include <GL/glew.h>

	namespace material {
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
				
				void setProgram(GLuint p) {
					program = p;
				}
			
				GLuint program;
				GLenum polyMode, cullMode;
				bool depthWrite, depthTest;

			private:

		};
	}
	
#endif