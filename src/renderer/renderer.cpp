#include "renderer.hpp"

renderer::Renderer *renderer::Renderer::_singleton = 0;

void renderer::Renderer::render() {
	unsigned int rQueueLength = render_queue.size();
	for (unsigned int i = 0; i < rQueueLength; i++) {
		unsigned int mQueueLength = render_queue[i].size();
		for (unsigned int e = 0; e < mQueueLength; e++) {
			const object::Mesh *mesh = 0;
			mesh = render_queue[i][e];
			
			if (!mesh) {
				continue;
			}
			
			material::Material *mat = 0;
			mat = mesh->mat;
			
			if (previousMat != mat) {
				mesh->mat->update();
			}
			
			if (mat) {
				while (!mat->states.empty()) {
					material::StateChangeType state = mat->states.top();
					mat->states.pop();
					
					switch (state) {
						case material::POLY_MODE:
							glPolygonMode(GL_FRONT_AND_BACK, mat->polyMode);
							break;
						
						case material::CULL_MODE:
							if (mat->cullMode == GL_NONE) {
								glDisable(GL_CULL_FACE);
							} else {
								glEnable(GL_CULL_FACE);
								glCullFace(mat->cullMode);
								glFrontFace(GL_CW);
							}
							break;
							
						case material::DEPTH_WRITE:
							if (mat->depthWrite) {
								glDepthMask(GL_TRUE);
							} else {
								glDepthMask(GL_FALSE);
							}
							break;
							
						case material::DEPTH_TEST:
							if (mat->depthTest) {
								glEnable(GL_DEPTH_TEST);
								glDepthFunc(GL_LEQUAL);
							} else {
								if (mat->depthWrite) {
									glEnable(GL_DEPTH_TEST);
									glDepthFunc(GL_ALWAYS);
								} else {
									glDisable(GL_DEPTH_TEST);
								}
							}
							break;
							
						case material::PROGRAM:
							glUseProgram(mat->program);
							break;
							
						default:
							break;
					}
				}
			}
			
			GLint offsetLocation = glGetUniformLocation(mat->program, "offset");
			GLint scaleLocation = glGetUniformLocation(mat->program, "scale");
			glUniform4f(offsetLocation, mesh->x, mesh->y, mesh->z, 0.0f);
			glUniform4f(scaleLocation, mesh->sx, mesh->sy, mesh->sz, 1.0f);

			glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

			//glDrawArrays(GL_TRIANGLES, 0, 3);
			glDrawElements(GL_TRIANGLES, mesh->indicesCount, 
								GL_UNSIGNED_INT, (void*)0);
				
			glDisableVertexAttribArray(0);
			
			previousMat = mat;
		}
	}
}