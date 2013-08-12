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
		
			if (renderTarget->multisampling) {
				glEnable(GL_MULTISAMPLE);
			} else {
				glDisable(GL_MULTISAMPLE);
			}
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
		
		core::Geometry *geom = mesh->geom;
		if (!geom) {
			continue;
		}
		
		material::Material *mat = mesh->mat;
		
		program::Program *prog = mat->prog;
				
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
						glUseProgram(prog->prog);
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

		if (mesh->type != object::QUAD_ALIGNED) {
			glm::mat4 mvp = currCamera->projMatrix * 
							currCamera->viewMatrix *
							mesh->getTransformedMatrix();

			glUniformMatrix4fv(prog->getUniformLocation("mvp"), 1, 
											GL_FALSE, glm::value_ptr(mvp));
		}

		prog->bindVbos();

		prog->setUniform1f("alpha", mesh->opacity);

		const core::Vbo *vVbo = geom->vVbo;

		//mesh->sortTriangles(mvp);

		if (vVbo->usage == GL_DYNAMIC_DRAW) {
			geom->updateVertices();
		} else { // because already bound
			glBindBuffer(GL_ARRAY_BUFFER, vVbo->bufferId);
		}
	
		glVertexAttribPointer(vVbo->attrib_index, vVbo->components, vVbo->data_type, vVbo->normalized, 0, 0);

		if (!mat->textures.empty()) {
			unsigned int textures_count = mat->textures.size();
			for (unsigned int j = 0; j < textures_count; j++) {
				const core::Texture *texture = mat->textures[j];

				glActiveTexture(GL_TEXTURE0+j);
				glBindTexture(texture->target, texture->id);
			}
			
			const core::Vbo *uVbo = geom->uVbo;
			if (uVbo->bufferId) {
				glBindBuffer(GL_ARRAY_BUFFER, uVbo->bufferId);
				glVertexAttribPointer(uVbo->attrib_index, uVbo->components, uVbo->data_type, uVbo->normalized, 0, 0);
			}
		}
				
		if (mesh->vertexColors) {
			const core::Vbo *cVbo = geom->cVbo;
			if (cVbo->usage == GL_DYNAMIC_DRAW) {
				geom->updateColors();
			} else { // already bound
				glBindBuffer(GL_ARRAY_BUFFER, cVbo->bufferId);
			}
			glVertexAttribPointer(cVbo->attrib_index, cVbo->components, cVbo->data_type, cVbo->normalized, 0, 0);
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geom->iVbo->bufferId);
		glDrawElements(geom->type, geom->indicesCount, GL_UNSIGNED_INT, (void*)0);
				
		previousMat = mat;
	}	
}