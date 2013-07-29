#ifndef FBO_HPP
#define FBO_HPP

	#include "texture.hpp"
	
	namespace core {
		class Fbo {
			public:
				Fbo(Texture *texture, bool depth_buffer = true) {
					id = 0;
					id_depthbuffer = 0;
					this->depth_buffer = depth_buffer;
					
					setTexture(texture);
				}
				
				void setTexture(Texture *texture) {
					glDeleteFramebuffers(1, &id);
					glDeleteRenderbuffers(1, &id_depthbuffer);
					
					glGenFramebuffers(1, &id);
					glBindFramebuffer(GL_FRAMEBUFFER, id);

					if (depth_buffer) {
						glGenRenderbuffers(1, &id_depthbuffer);
						glBindRenderbuffer(GL_RENDERBUFFER, id_depthbuffer);
						glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, texture->width, texture->height);
						glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, id_depthbuffer);
					}
					
					glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture->id, 0);
 
					GLenum DrawBuffers[2] = {GL_COLOR_ATTACHMENT0};
					glDrawBuffers(1, &DrawBuffers[0]);
					
					if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
						log("creation failed");
					}
					
					this->texture = texture;
				}
				
				~Fbo() {
					glDeleteFramebuffers(1, &id);
					glDeleteRenderbuffers(1, &id_depthbuffer);
				}
				
				void log(const char *str) {
					std::cout << "[Fbo] " << str << std::endl;
				}
				
				bool depth_buffer;
				GLuint id, id_depthbuffer;
				Texture *texture;
		};
	}
	
#endif