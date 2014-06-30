#include "vbo.hpp"

bool core::Vbo::build(GLsizeiptr size, const GLvoid *data, GLenum target,
                                                                GLenum usage) {
    if (size <= 0) {
        return false;
    }
					
    if (_buffer_id) {
        destroy();
    }
					
    glGetError(); // clear previous errors

    glGenBuffers(1, &_buffer_id);
    glBindBuffer(target, _buffer_id);
    glBufferData(target, size, data, usage);
						
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        destroy();
        log("Creation failed.");
        return false;
    }
					
    return true;
}

void core::Vbo::update(GLsizeiptr size, const GLvoid *data) {
    if (_buffer_id == 0) {
        return;
    }

    glBindBuffer(_target, _buffer_id);
					
    //glBufferSubData(target, 0, size, data);
    glBufferData(_target, size, data, _usage);
}

void core::Vbo::attrib(GLuint index) {
    if (_buffer_id == 0) {
        return;
    }
					
    glBindBuffer(_target, _buffer_id);
    glEnableVertexAttribArray(index);
					
    _attrib_index = index;
}