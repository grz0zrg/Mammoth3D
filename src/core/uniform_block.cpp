#include "uniform_block.hpp"

GLuint core::UniformBlock::_binding_point_counter = 0;

void core::UniformBlock::initialize(bool dynamic) {
    glDeleteBuffers(1, &_id);

    glGenBuffers(1, &_id);
    glBindBuffer(GL_UNIFORM_BUFFER, _id);
	
    if (_data.empty()) {
        log("initialize() failed, no data.");
        return;
    }

    if (dynamic) {
        glBufferData(GL_UNIFORM_BUFFER, _data.size() * sizeof(float), &_data[0], 
                                                            GL_DYNAMIC_DRAW);
    } else {
        glBufferData(GL_UNIFORM_BUFFER, _data.size() * sizeof(float), &_data[0], 
                                                            GL_STATIC_DRAW);
    }
					
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        log("\"", _name, "\" creation failed, error code: ", err);
    }
}

void core::UniformBlock::set(const std::map<std::string, float> &uniforms) {
    for (auto it = uniforms.begin(); it != uniforms.end(); ++it) {
        set(it->first, it->second);
    }
    
    update();
}

void core::UniformBlock::set(const std::string& uniform_name, float value) {
    if (_uniform_index.find(uniform_name) == _uniform_index.end()) {
        _uniform_index[uniform_name] = _data.size();
        _data.push_back(value);
        _data_count = _data.size();
    } else {
        _data[_uniform_index[uniform_name]] = value;
    }
}

void core::UniformBlock::update() {
    glBindBufferBase(GL_UNIFORM_BUFFER, _binding_point, _id);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, _data_count * sizeof(float), &_data[0]);
}