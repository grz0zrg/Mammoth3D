#include "renderer.hpp"

renderer::Renderer *renderer::Renderer::_singleton = 0;

void renderer::Renderer::render(const object::Mesh *mesh, int count) {
	if (previousPolyMode != mesh->mat->polyMode) {
		glPolygonMode(GL_FRONT_AND_BACK, mesh->mat->polyMode);
	}

	if (previousCullMode != mesh->mat->cullMode) {
		if (mesh->mat->cullMode == GL_NONE) {
			glDisable(GL_CULL_FACE);
		} else {
			glEnable(GL_CULL_FACE);
			glCullFace(mesh->mat->cullMode);
		}
	}
	
	if (previousDepthWrite != mesh->mat->depthWrite) {
		if (mesh->mat->depthWrite) {
			glDepthMask(GL_TRUE);
		} else {
			glDepthMask(GL_FALSE);
		}
	}

	if (mesh->mat->depthTest == true && previousDepthTest != mesh->mat->depthTest) {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
	} else {
		if (previousDepthWrite != mesh->mat->depthWrite) {
			if (mesh->mat->depthWrite) {
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_ALWAYS);
			} else {
				glDisable(GL_DEPTH_TEST);
			}
		}
	}

	if (previousProgram != mesh->mat->program) {
		glUseProgram(mesh->mat->program);
	}

	if (count > 1) {
		
	} else {
		glBindBuffer(GL_ARRAY_BUFFER, mesh->data->vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->data->ibo);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, mesh->data->indicesCount, 
						GL_UNSIGNED_INT, (void*)0);
		
		glDisableVertexAttribArray(0);
	}
	
	previousProgram    = mesh->mat->program;
	previousPolyMode   = mesh->mat->polyMode;
	previousCullMode   = mesh->mat->cullMode;
	previousDepthWrite = mesh->mat->depthWrite;
	previousDepthTest  = mesh->mat->depthTest;
}