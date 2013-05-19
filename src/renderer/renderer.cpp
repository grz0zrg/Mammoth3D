#include "renderer.hpp"

renderer::Renderer *renderer::Renderer::_singleton = 0;

void renderer::Renderer::render() {
	unsigned int rQueueLength = render_queue.size();
	for (unsigned int i = 0; i < rQueueLength; i++) {
		unsigned int mQueueLength = render_queue[i].size();
		for (unsigned int e = 0; e < mQueueLength; e++) {
			object::Mesh *mesh = 0;
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
								glDisable(GL_CULL_FACE);
								//glEnable(GL_CULL_FACE);
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
								glDepthMask(GL_TRUE);
								glDepthFunc(GL_LEQUAL);
								glDepthRange(0.0f, 1.0f);
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
							glUseProgram(mat->prog->prog);
							break;
							
						case material::BLENDING:
							if (mat->blending) {
								glEnable(GL_BLEND);
								//alpha
								glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
								glBlendEquation(GL_FUNC_ADD);
								
								//add
								//glBlendFunc(GL_ONE, GL_ONE);
								//glBlendEquation(GL_FUNC_ADD);
								
								//sub
								//glBlendFunc(GL_ONE, GL_ONE);
								//glBlendEquation(GL_FUNC_SUBTRACT);
							} else {
								glDisable(GL_BLEND);
							}
							break;
							
						default:
							break;
					}
				}
			}

			mesh->updateMatrix();

			core::Matrix4 mvp = currCamera->projMatrix * 
								currCamera->viewMatrix * 
								mesh->modelMatrix;
								
			glUniform1f(mat->prog->getUniformLocation("alpha"), mesh->opacity);
			glUniformMatrix4fv(mat->prog->getUniformLocation("mvp"), 1, 
															GL_FALSE, mvp.m);
			//mesh->sortTriangles(mvp);
			if (mesh->geom->vbo->verticeBufferUsage == GL_DYNAMIC_DRAW) {
				mesh->geom->updateVertices();
			}
			
			glBindBuffer(GL_ARRAY_BUFFER, mesh->geom->vbo->verticeBufferId);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 
											mesh->geom->vbo->indiceBufferId);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			
			if (mesh->vertexColors) {
				if (mesh->geom->vbo->colorBufferUsage == GL_DYNAMIC_DRAW) {
					mesh->geom->updateColors();
				}

				glEnableVertexAttribArray(1);
				glBindBuffer(GL_ARRAY_BUFFER, mesh->geom->vbo->colorBufferId);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			}

			glDrawElements(GL_TRIANGLES, mesh->geom->indicesCount, 
								GL_UNSIGNED_INT, (void*)0);
				
			//glDisableVertexAttribArray(0);
			//glDisableVertexAttribArray(1);
			
			previousMat = mat;
		}
	}
}