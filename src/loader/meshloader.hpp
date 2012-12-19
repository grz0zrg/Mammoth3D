#ifndef MESHLOADER_HPP
#define MESHLOADER_HPP

	#include <fstream>
	#include <iostream>
	#include <vector>
	
	#include <GL/glew.h>
	
	namespace loader {
		class MeshLoader {
			public:
				MeshLoader(const std::string &fileName) { 
					indsType = sizeof(unsigned int);
					polyMode = GL_FILL;
					cullMode = GL_BACK;
					depthWrite = false;
					depthTest = false;
					program = 0;

					loadMesh(fileName); 
				};
				
				~MeshLoader() { 
					glDeleteBuffers(1, &ibo);
					glDeleteBuffers(1, &vbo);
				};
				
				void setPolyMode(GLenum mode) {
					polyMode = mode;
				}
				
				void setCulling(GLenum mode) {
					cullMode = mode;
				}
				
				void disableCulling() {
					cullMode = GL_NONE;
				}
				
				void setDepthWrite(bool enabled) {
					depthWrite = enabled;
				}

				void setDepthTest(bool enabled) {
					depthTest = enabled;
				}
				
				void setProgram(GLuint p) {
					p = program;
				}
				
				GLuint vbo, ibo, program;
				GLenum polyMode, cullMode;
				bool depthWrite, depthTest;
				
				unsigned int indicesCount;

			private:
				void loadMesh(const std::string &fileName);
				
				int createBuffers();

				template <typename T>
				void logPretty(const std::string &str, T param) {
					std::cout << "[MeshLoader] " << str << "\"" << 
								param << "\"" << std::endl;
				}
				
				template <typename T>
				void log(const std::string &str, T param) {
					std::cout << "[MeshLoader] " << str << param << std::endl;
				}
					
				void log(const char *str) {
					std::cout << "[MeshLoader] " << str << std::endl;
				}
				
			char indsType;
			
			std::string name;
			std::vector<unsigned int> indices;
			std::vector<float> vertices;
			std::vector<float> normals;
		};
	}

#endif