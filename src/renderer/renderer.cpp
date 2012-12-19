#include "renderer.hpp"

renderer::Renderer *renderer::Renderer::_singleton = 0;

void renderer::Renderer::render(const loader::MeshLoader *mesh, int count) {
	glPolygonMode(GL_FRONT_AND_BACK, mesh->polyMode);
	if (mesh->cullMode == GL_NONE) {
		glDisable(GL_CULL_FACE);
	} else {
		glEnable(GL_CULL_FACE);
		glCullFace(mesh->cullMode);
	}
	
	if (mesh->depthWrite) {
		glDepthMask(GL_TRUE);
	} else {
		glDepthMask(GL_FALSE);
	}

	if (mesh->depthTest) {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
	} else {
		if (mesh->depthWrite) {
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_ALWAYS);
		} else {
			glDisable(GL_DEPTH_TEST);
		}
	}

	glUseProgram(mesh->program);

	if (count > 1) {
		
	} else {
		glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, mesh->indicesCount, 
						GL_UNSIGNED_INT, (void*)0);
		
		glDisableVertexAttribArray(0);
	}
}