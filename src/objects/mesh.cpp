#include "mesh.hpp"

int object::Mesh::createIndicesBuffer() {
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), 
		&indices.front(), GL_STATIC_DRAW);
		
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		log("IBO creation failure.");
		glDeleteBuffers(1, &ibo);
		return -1;
	}
	
	return 1;
}

int object::Mesh::createVerticesBuffer() {
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), 
		&vertices.front(), GL_STATIC_DRAW);

	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		log("VBO creation failure.");
		glDeleteBuffers(1, &vbo);
		return -1;
	}
	
	return 1;
}

int object::Mesh::createColorsBuffer() {
	glGenBuffers(1, &cbo);
	glBindBuffer(GL_ARRAY_BUFFER, cbo);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), 
		&colors.front(), GL_STATIC_DRAW);
		
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		vertexColors = false;
		log("CBO (colors buffer) creation failure, vertex colors disabled.");
		glDeleteBuffers(1, &cbo);
		return -1;
	}
	
	return 1;
}