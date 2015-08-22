#include "renderer.hpp"

renderer::Renderer *renderer::Renderer::_singleton = 0;

void renderer::Renderer::render() {
	core::Fbo *previousRenderTarget = 0;

	unsigned int rQueueLength = _render_queue.size();
	for (unsigned int i = 0; i < rQueueLength; i++) {
		Renderable *renderable = _render_queue[i];
		
		core::Fbo *renderTarget = renderable->renderTarget;
		
		scenegraph::Node *root = renderable->getRoot();

		if (previousRenderTarget != 0) {
			if (previousRenderTarget->_multisampling) {
				//glDisable(GL_MULTISAMPLE);
/*
				glBindFramebuffer(GL_READ_FRAMEBUFFER, previousRenderTarget->id);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glBlitFramebuffer(
					0, 0, viewportWidth, viewportHeight, 
					0, 0, viewportWidth, viewportHeight, 
					GL_COLOR_BUFFER_BIT, GL_NEAREST);*/
			}
		}

		if (renderTarget == 0) {
			//glDisable(GL_MULTISAMPLE);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, _viewport_width, _viewport_height);
		} else {
			glBindFramebuffer(GL_FRAMEBUFFER, renderTarget->_id);
			glViewport(0, 0, renderTarget->_texture->_width, 
                                            renderTarget->_texture->_height);
		
			if (renderTarget->_multisampling) {
				glEnable(GL_MULTISAMPLE);
			}
		}
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		//glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
		
		render(root);

		previousRenderTarget = renderTarget;
	}
    
    _frame += 1;
    
    auto time_diff = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - _time);
    if (time_diff.count() >= 1.0) {
        _fps = _frame;
        
        _frame = 0;
        
        _time = std::chrono::high_resolution_clock::now();
    }
}

void renderer::Renderer::render(scenegraph::Node *node) {
	for (unsigned int j = 0; j < node->_children.size(); j++) {
		scenegraph::Node *node_ = node->_children[j];
		
		if (node_->_type == scenegraph::NODE_CAMERA) {
			_curr_camera = ((scenegraph::CameraNode *)node_)->_camera;
			if (_curr_camera) {
				if (_curr_camera->_aspect != _aspect_ratio &&
					_curr_camera->_type == camera::PERSPECTIVE) {
					_curr_camera->_aspect = _aspect_ratio;
					_curr_camera->buildProjection();
				}
			}
		} else if (node_->_type == scenegraph::NODE_MESH && _curr_camera) {
			render((scenegraph::MeshNode *)node_);
		} else {
			continue;
		}
		
		if (node_->isVisible()) {
			render(node_);
		}
	}
}

void renderer::Renderer::render(scenegraph::MeshNode *node) {
	for (unsigned int i = 0; i < node->_meshs.size(); i++) {
		object::Mesh *mesh = node->_meshs[i];
		
		core::Geometry *geom = mesh->getGeometry();
		if (!geom) {
			continue;
		}
		
		material::Material *mat = mesh->getMaterial();
		
		program::Program *prog = mat->getProgram();
				
		if (_previous_mat != mat) {
			mesh->getMaterial()->update();
		}

		if (mat) {
			while (!mat->_states.empty()) {
				material::StateChangeType state = mat->_states.top();
				mat->_states.pop();
						
				switch (state) {
					case material::POLY_MODE:
						glPolygonMode(GL_FRONT_AND_BACK, mat->_poly_mode);
						if (mat->_poly_mode == GL_LINE) {
							glLineWidth(mat->_line_width);
						}
						break;
							
					case material::CULL_MODE:
						if (mat->_cull_mode == GL_NONE) {
							glDisable(GL_CULL_FACE);
						} else {
							glEnable(GL_CULL_FACE);
							glCullFace(mat->_cull_mode);
							glFrontFace(GL_CW);
						}
						break;
								
					case material::DEPTH_WRITE:
						if (mat->_depth_write) {
							glDepthMask(GL_TRUE);
						} else {
							glDepthMask(GL_FALSE);
						}
						break;
								
					case material::DEPTH_TEST:
						if (mat->_depth_test) {
							glEnable(GL_DEPTH_TEST);
							glDepthMask(GL_TRUE);
							glDepthFunc(GL_LEQUAL);
							glDepthRange(0.0f, 1.0f);
						} else {
							if (mat->_depth_write) {
								glEnable(GL_DEPTH_TEST);
								glDepthFunc(GL_ALWAYS);
							} else {
								glDisable(GL_DEPTH_TEST);
							}
						}
						break;
								
					case material::PROGRAM:
						glUseProgram(prog->_id);
						break;
								
					case material::BLENDING:
						if (mat->_blending) {
							glEnable(GL_BLEND);
							
							glBlendFunc(mat->_blend_func_src, mat->_blend_func_dst);
							glBlendEquation(mat->_blend_equation);
						} else {
							glDisable(GL_BLEND);
						}
						break;
								
					default:
						break;
				}
			}
		}

		if (mesh->_type != object::QUAD_ALIGNED) {
			mvp = _curr_camera->_proj_matrix * 
                  _curr_camera->_view_matrix *
                  /*node->getTransformedMatrix();*/mesh->getTransformedMatrix();

            const float *pSource = (const float*)glm::value_ptr(mvp);
            /*for (int i = 0; i < 16; ++i) {
                std::cout << pSource[i] << " ";
                if (!(i%4)) std::cout << std::endl;
            }
            std::cout << std::endl;
            */
			glUniformMatrix4fv(prog->getUniformLocation("mvp"), 1, 
											GL_FALSE, glm::value_ptr(mvp));
		}

		prog->bindVbos();

		prog->setUniform1f("alpha", mesh->getOpacity());

		const core::Vbo *vVbo = geom->_v_vbo;

		//mesh->sortTriangles(mvp);

		if (vVbo->_usage == GL_DYNAMIC_DRAW) {
			geom->updateVertices();
		} else { // because already bound
			glBindBuffer(GL_ARRAY_BUFFER, vVbo->_buffer_id);
		}
	
		glVertexAttribPointer(vVbo->_attrib_index, vVbo->_components, vVbo->_data_type, vVbo->_normalized, 0, 0);

		if (!mat->_textures.empty()) {
			unsigned int textures_count = mat->_textures.size();
			for (unsigned int j = 0; j < textures_count; j++) {
				const core::Texture *texture = mat->_textures[j];

				glActiveTexture(GL_TEXTURE0+j);
				glBindTexture(texture->_target, texture->_id);
			}
			
			const core::Vbo *uVbo = geom->_u_vbo;
			if (uVbo->_buffer_id) {
				glBindBuffer(GL_ARRAY_BUFFER, uVbo->_buffer_id);
				glVertexAttribPointer(uVbo->_attrib_index, uVbo->_components, uVbo->_data_type, uVbo->_normalized, 0, 0);
			}
		}
				
		if (mesh->_vertex_colors) {
			const core::Vbo *cVbo = geom->_c_vbo;
			if (cVbo->_usage == GL_DYNAMIC_DRAW) {
				geom->updateColors();
			} else { // already bound
				glBindBuffer(GL_ARRAY_BUFFER, cVbo->_buffer_id);
			}
			glVertexAttribPointer(cVbo->_attrib_index, cVbo->_components, cVbo->_data_type, cVbo->_normalized, 0, 0);
		}
        
        if (mesh->_i_count == 0) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geom->_i_vbo->_buffer_id);
            glDrawElements(geom->_type, geom->_indices_count, GL_UNSIGNED_INT, (void*)0);
        } else {
            auto _i_meshs   = mesh->_i_meshs;
            auto _i_mats    = mesh->_i_mats;
            auto _i_mat_vbo = mesh->_i_mat_vbo;
            
            for (unsigned int i = 0; i < mesh->_i_count; i++) {
                unsigned int k = i * 16;
                
                auto matrix = _i_meshs[i]->getTransformedMatrix();
                
                mvp = _curr_camera->_proj_matrix * _curr_camera->_view_matrix * matrix;
                
                const float *source = (const float*)glm::value_ptr(mvp);
                for (unsigned int j = 0; j < 16; j++) {
                    _i_mats[k + j] = source[j];
                }
            }
            
            if (_i_mat_vbo) {
                _i_mat_vbo->update(mesh->_i_count * 16 * sizeof(float), &_i_mats.front());
            }
            
            for (unsigned int i = 0; i < 4 ; i++) {
                glVertexAttribPointer(_i_mat_vbo->_attrib_index + i, _i_mat_vbo->_components, _i_mat_vbo->_data_type, _i_mat_vbo->_normalized, 
                                        16 * sizeof(float), (void *)((4 * i) * sizeof(float)));
            }
            
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geom->_i_vbo->_buffer_id);
            glDrawElementsInstanced(geom->_type, geom->_indices_count, GL_UNSIGNED_INT, (void*)0, mesh->_i_count);
        }
				
		_previous_mat = mat;
	}	
}