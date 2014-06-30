#include "fbo.hpp"

core::Fbo::Fbo(Texture *texture, bool multisampling, bool depth_buffer) {
    _id = 0;
    _id_depthbuffer = 0;
    _multisampling  = multisampling;
    _depth_buffer   = depth_buffer;
					
    glGenFramebuffers(1, &_id);
                    
    if (depth_buffer) {
        glGenRenderbuffers(1, &_id_depthbuffer);
    }
					
    setTexture(texture);
}

core::Fbo::~Fbo() {
    glDeleteFramebuffers(1, &_id);
    glDeleteRenderbuffers(1, &_id_depthbuffer);
}

void core::Fbo::setTexture(Texture *texture) {
    if (!texture)
        return;
						
    glBindFramebuffer(GL_FRAMEBUFFER, _id);

    if (_depth_buffer) {
        glBindRenderbuffer(GL_RENDERBUFFER, _id_depthbuffer);
        if (_multisampling) {
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, texture->_samples, 
                        GL_DEPTH_COMPONENT, texture->_width, texture->_height);
        } else {
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 
                                            texture->_width, texture->_height);
        }
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
                                            GL_RENDERBUFFER, _id_depthbuffer);
    }
					
    if (_multisampling) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                                    GL_TEXTURE_2D_MULTISAMPLE, texture->_id, 0);
    } else {
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                                    texture->_id, 0);
    }
					
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, &DrawBuffers[0]);

    GLenum frameBufferState = glCheckFramebufferStatus(GL_FRAMEBUFFER);
                
    switch (frameBufferState) {
        case GL_FRAMEBUFFER_COMPLETE:
        break;
                            
        case GL_FRAMEBUFFER_UNDEFINED:
            log("creation failed (GL_FRAMEBUFFER_UNDEFINED)");
            break;
                            
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            log("creation failed (GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)");
            break;

        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            log("creation failed (GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)");
            break;
                            
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            log("creation failed (GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER)");
            break;
                            
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            log("creation failed (GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)");
            break;
                        
        case GL_FRAMEBUFFER_UNSUPPORTED:
            log("creation failed (GL_FRAMEBUFFER_UNSUPPORTED)");
            break;
                            
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            log("creation failed (GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE)");
            break;
                            
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
            log("creation failed (GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS)");
            break;
                            
        default: 
            log("creation failed");
            break;
    }
					
    _texture = texture;
}