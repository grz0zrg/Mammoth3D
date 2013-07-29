#include "renderer.hpp"

renderer::Renderer *renderer::Renderer::_singleton = 0;

void renderer::Renderer::render() {
	unsigned int rQueueLength = render_queue.size();
	for (unsigned int i = 0; i < rQueueLength; i++) {
		Renderable *renderable = render_queue[i];
		
		core::Fbo *renderTarget = renderable->renderTarget;
		
		scenegraph::Node *root = renderable->getRoot();
		
		if (renderTarget == 0) {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, viewportWidth, viewportHeight);
		} else {
			glBindFramebuffer(GL_FRAMEBUFFER, renderTarget->id);
			glViewport(0, 0, renderTarget->texture->width, renderTarget->texture->height);
		}
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		//glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
		
		render(root);
	}
}

void renderer::Renderer::render(scenegraph::Node *node) {
	for (unsigned int j = 0; j < node->children.size(); j++) {
		scenegraph::Node *node_ = node->children[j];
			
		if (node_->type != scenegraph::NODE_DRAWABLE ||
			!node_->isVisible()) {
			continue;
		}
			
		render((scenegraph::MeshNode *)node_);
			
		render(node_);
	}
}

void renderer::Renderer::render(scenegraph::MeshNode *node) {
	for (unsigned int i = 0; i < node->meshs.size(); i++) {
		object::Mesh *mesh = node->meshs[i];
				
		material::Material *mat = mesh->mat;
				
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
						if (mat->polyMode == GL_LINE) {
							glLineWidth(mat->lineWidth);
						}
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
							
							glBlendFunc(mat->blendFuncSrc, mat->blendFuncDst);
							glBlendEquation(mat->blendEquation);
						} else {
							glDisable(GL_BLEND);
						}
						break;
								
					default:
						break;
				}
			}
		}
				
		mat->prog->updateDynamicUniforms();
				
		if (mesh->type != object::QUAD_ALIGNED) {
			glm::mat4 mvp = currCamera->projMatrix * 
							currCamera->viewMatrix *
							mesh->getTransformedMatrix();
							
			glUniformMatrix4fv(mat->prog->getUniformLocation("mvp"), 1, 
											GL_FALSE, glm::value_ptr(mvp));
		}
			
		glUniform1f(mat->prog->getUniformLocation("alpha"), mesh->opacity);

		//mesh->sortTriangles(mvp);
		if (mesh->geom->vbo->verticeBufferUsage == GL_DYNAMIC_DRAW) {
			mesh->geom->updateVertices();
		}
				
		glBindBuffer(GL_ARRAY_BUFFER, mesh->geom->vbo->verticeBufferId);
				
		if (mesh->geom->vbo->indiceBufferId != 0) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->geom->vbo->indiceBufferId);
		}

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		if (mat->texture) {
			if (mesh->geom->vbo->uvBufferId) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, mat->texture->id);
				glUniform1i(glGetUniformLocation(mat->prog->prog, 
												"myTextureSampler"), 0);
				glBindBuffer(GL_ARRAY_BUFFER, mesh->geom->vbo->uvBufferId);
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
			}
		}
				
		if (mesh->vertexColors) {
			if (mesh->geom->vbo->colorBufferUsage == GL_DYNAMIC_DRAW) {
				mesh->geom->updateColors();
			}

			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, mesh->geom->vbo->colorBufferId);
			glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);
		}

		if (mesh->type == object::DEFAULT) {
			glDrawElements(GL_TRIANGLES, mesh->geom->indicesCount, 
								GL_UNSIGNED_INT, (void*)0);
		} else {
			glDrawArrays(GL_QUADS, 0, 4);
		}
				
		previousMat = mat;
	}	
}