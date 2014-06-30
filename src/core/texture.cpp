#include "texture.hpp"

void core::Texture::create(int width, int height, GLenum type) {
    _width   = width;
    _height  = height;
    _samples = 1;

    glDeleteTextures(1, &_id);
    glGenTextures(1, &_id);

    _target = type;
					
    glBindTexture(_target, _id);
		
    if (_target == GL_TEXTURE_2D) {
        //std::vector<unsigned char> fill;
        //fill.resize(width*height*4, 0);
        glTexImage2D(_target, 0, GL_RGBA, _width, _height, 0, GL_RGBA, 
                                                        GL_UNSIGNED_BYTE, 0);
        //glGenerateMipmap(target);
    }
}

void core::Texture::create(Image *image, GLenum type) {
    if (!image) {
        return;
    }
					
    _image = image;
					
    _width = image->_width;
    _height = image->_height;
					
    glDeleteTextures(1, &_id);
    glGenTextures(1, &_id);

    _target = type;

    glBindTexture(_target, _id);
					
    if (_image->_data.empty()) {
        glTexImage2D(_target, 0, GL_RGBA, _width, _height, 0, GL_RGBA, 
                                            GL_UNSIGNED_BYTE, 0);
    } else {
        glTexImage2D(_target, 0, GL_RGBA, _width, _height, 0, GL_RGBA, 
                                            GL_UNSIGNED_BYTE, &image->_data[0]);
    }

    //glGenerateMipmap(target);
}

void core::Texture::setTextureSize(int width, int height) {
    glBindTexture(_target, _id);
					
    if (_target == GL_TEXTURE_2D) {
        if (_image) { // the image still need to be resized before
            glTexImage2D(_target, 0, GL_RGBA, _width, _height, 0, GL_RGBA, 
                                        GL_UNSIGNED_BYTE, &_image->_data[0]);
        } else {
            glTexImage2D(_target, 0, GL_RGBA, _width, _height, 0, GL_RGBA, 
                                        GL_UNSIGNED_BYTE, 0);
        }
        //glGenerateMipmap(target);
    }
}