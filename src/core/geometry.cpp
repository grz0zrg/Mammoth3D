#include "geometry.hpp"

core::Geometry::Geometry() {
    _indices_count  = 0;
    _vertices_count = 0;
    _normals_count  = 0;
    _uvs_count      = 0;
    _colors_count   = 0;
					
    _i_vbo = _v_vbo = _n_vbo = _u_vbo = _c_vbo = 0;
    _ic_vbo = _iv_vbo = _iu_vbo = _in_vbo = false;
    
    _type = GL_TRIANGLES;
}

core::Geometry::~Geometry() {
    if (_i_vbo) {
        delete _i_vbo;
    }
					
    if (_v_vbo) {
        delete _v_vbo;
    }
					
    if (_n_vbo) {
        delete _n_vbo;
    }

    if (_u_vbo) {
        delete _u_vbo;
    }
					
    if (_c_vbo) {
        delete _c_vbo;
    }
}

core::Geometry *core::Geometry::clone() {
    auto geom = new Geometry();

    geom->_indices  = _indices;
    geom->_vertices = _vertices;
    geom->_normals  = _normals;
    geom->_uvs      = _uvs;
    geom->_colors   = _colors;
    geom->update();

    return geom;
}

void core::Geometry::generateVbo() {
    bool ret = false;
    if (!_indices.empty()) {
        if (_i_vbo) {
            delete _i_vbo;
        }
        _i_vbo = new Vbo();
        ret = _i_vbo->build(_indices.size() * sizeof(unsigned int), 
                            &_indices.front(),
                            GL_ELEMENT_ARRAY_BUFFER,
                            GL_STATIC_DRAW);
        if (!ret) {
            log("Indices VBO creation failed.");
        }
    }
					
    if (!_vertices.empty()) {
        if (_v_vbo) {
            delete _v_vbo;
        }
        _v_vbo = new Vbo();
        ret = _v_vbo->build(_vertices.size() * sizeof(float), 
                            &_vertices.front(),
                            GL_ARRAY_BUFFER,
                            GL_STATIC_DRAW);
        _v_vbo->attrib(0);
        _v_vbo->setComponents(3);
        
        if (_iv_vbo) {
            _v_vbo->setAttribDivisor(1);
        }
        
        if (!ret) {
            log("Vertices VBO creation failed.");
        }
    }
					
    if (!_uvs.empty()) {
        if (_u_vbo) {
            delete _u_vbo;
        }
        _u_vbo = new Vbo();
        ret = _u_vbo->build(_uvs.size() * sizeof(float), 
                            &_uvs.front(),
                            GL_ARRAY_BUFFER,
                            GL_STATIC_DRAW);
        _u_vbo->attrib(1);
        _u_vbo->setComponents(2);
        
        if (_iu_vbo) {
            _u_vbo->setAttribDivisor(1);
        }
        
        if (!ret) {
            log("UVs VBO creation failed.");
        }
    }
					
    if (!_normals.empty()) {
        if (_n_vbo) {
            delete _n_vbo;
        }
        _n_vbo = new Vbo();
        ret = _n_vbo->build(_normals.size() * sizeof(float), 
                            &_normals.front(),
                            GL_ARRAY_BUFFER,
                            GL_STATIC_DRAW);
        _n_vbo->attrib(2);
        _n_vbo->setComponents(3);
        
        if (_in_vbo) {
            _n_vbo->setAttribDivisor(1);
        }
        
        if (!ret) {
            log("Normals VBO creation failed.");
        }
    }
            
    if (!_colors.empty()) {
        if (_c_vbo) {
            delete _c_vbo;
        }
        _c_vbo = new Vbo();
        ret = _c_vbo->build(_colors.size() * sizeof(float), 
                            &_colors.front(),
                            GL_ARRAY_BUFFER,
                            GL_STATIC_DRAW);
        _c_vbo->attrib(3);
        _c_vbo->setComponents(4);
        
        _c_vbo->setNormalized(GL_TRUE);
        
        if (_ic_vbo) {
            _c_vbo->setAttribDivisor(1);
        }
        
        if (!ret) {
            log("Colors VBO creation failed.");
        }
    }
}

void core::Geometry::setDynamic(GeometryData type, bool state) {
    GLenum usage = GL_DYNAMIC_DRAW;
    if (!state) {
        usage = GL_STATIC_DRAW;
    }
    
    switch (type) {
        case GEOMETRY_COLOR:
            if (_c_vbo) {
                _c_vbo->setUsage(usage);
            }
            break;
            
        case GEOMETRY_VERTICE:
            if (_v_vbo) {
                _v_vbo->setUsage(usage);
            }
            break;
            
        case GEOMETRY_NORMAL:
            if (_n_vbo) {
                _n_vbo->setUsage(usage);
            }
            break;
            
        case GEOMETRY_UV:
            if (_u_vbo) {
                _u_vbo->setUsage(usage);
            }
            break;
            
        default:
            break;
    }
}

void core::Geometry::setInstanced(GeometryData type) {
    switch (type) {
        case GEOMETRY_COLOR:
            if (_c_vbo) {
                _c_vbo->setAttribDivisor(1);
                _ic_vbo = true;
            }
            break;
            
        case GEOMETRY_VERTICE:
            if (_v_vbo) {
                _v_vbo->setAttribDivisor(1);
                _iv_vbo = true;
            }
            break;
            
        case GEOMETRY_NORMAL:
            if (_n_vbo) {
                _n_vbo->setAttribDivisor(1);
                _in_vbo = true;
            }
            break;
            
        case GEOMETRY_UV:
            if (_u_vbo) {
                _u_vbo->setAttribDivisor(1);
                _iu_vbo = true;
            }
            break;
            
        default:
            break;
    }
}

void core::Geometry::updateVertices() {
    _v_vbo->update(_vertices.size() * sizeof(float), &_vertices.front());
    _vertices_count = _vertices.size();
}

void core::Geometry::updateColors() {
    _c_vbo->update(_colors.size() * sizeof(float), &_colors.front());
    _colors_count = _colors.size();
}

void core::Geometry::update() {
    _indices_count  = _indices.size();
    _vertices_count = _vertices.size();
    _normals_count  = _normals.size();
    _uvs_count      = _uvs.size();
    _colors_count   = _colors.size();
					
    generateVbo();
}