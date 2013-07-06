#ifndef AUDIO_HPP
#define AUDIO_HPP

	#include <iostream>
	#include <cstdlib>

	#include "vorbis/vorbisfile.h"
	#include "portaudio/portaudio.h"
								   
	namespace audio {
		class Audio {
			private:
				Audio() {
					paErr = Pa_Initialize();
					
					paErr != paNoError ? terminatePa = false : terminatePa = true;
					logPaError();
					
					music = 0;
					music_stream = 0;
					
					log("", Pa_GetVersionText());
				};
				
				~Audio() {
					freeMusic();
					if(terminatePa) {
						paErr = Pa_Terminate();
						logPaError();
					}
				};
				
				void logPaError() {
					if( paErr != paNoError ) {
						log("PortAudio error: ", Pa_GetErrorText(paErr));
					}
				}
				
				template <typename T>
				void log(const std::string &str, T param) {
					std::cout << "[Audio] " << str << param << std::endl;
				}

				void log(const std::string &str) {
					std::cout << "[Audio] " << str << std::endl;
				}

				struct musicData {
					float *left;
					float *right;
					unsigned long frames;
					unsigned long position;
					float volume;
					bool loop;
					bool paused;
					bool finished;
				};
		
				static int paCallback( const void *inputBuffer,
						void *outputBuffer,
						unsigned long framesPerBuffer,
						const PaStreamCallbackTimeInfo* timeInfo,
						PaStreamCallbackFlags statusFlags,
						void *userData);
				
				Audio(const Audio&);
				void operator=(const Audio&);
				
				PaError paErr;
				PaStream *music_stream;
				musicData *music;

				int _value;
				static Audio *_singleton;
				
				bool terminatePa;

			public:
				void loadMusic(const char* fileName) {
					if(music_stream) {
						paErr = Pa_CloseStream(music_stream);
						logPaError();
						music_stream = 0;
					}

					log("loading: ", fileName);
					
					OggVorbis_File vf;
					music = 0;
					music = new musicData;
					if(!music) {
						music = 0;
						log("alloc error");
						return;
					}
					music->left = music->right = 0;
					music->position = 0;
					music->loop = false;
					music->paused = false;
					music->finished = false;
					music->volume = 1.0;
					
					int err = ov_fopen(fileName, &vf);
					if(err < 0) {
						log("ov_fopen error code: ", err);
						delete music;
					
						return;
					}
					
					unsigned long length = (long)ov_pcm_total(&vf, -1);
					music->left = new float[length];
					music->right = new float[length];
					music->frames = length;
					if( !music->left || !music->right ) {
						log("alloc error");
						ov_clear( &vf );
						freeMusic();
						
						return;
					}
					
					unsigned long total_in = 0;
					unsigned long offset = 0;
					float **samples;
					long frame = 0;
					int current_section = 0;

					long ret = ov_read_float(&vf, &samples, 1024, &current_section);
					while(ret != 0) {
						if(ret == OV_HOLE || ret == OV_EBADLINK || ret == OV_EINVAL) {
							log("ov_read_float return code: ", ret);
						} else {
							for( frame = 0; frame < ret; ++frame ) {
								music->left[frame + offset] = samples[0][frame];
								music->right[frame + offset] =samples[1][frame];
								total_in += 2;
								if(frame + offset > length) {
									log("error reading music data: (offset + frame) = ", (offset + frame));
									ov_clear( &vf );
									freeMusic();
									
									return;
								}
							}

							offset += ret;
						}
						
						ret = ov_read_float(&vf, &samples, 1024, &current_section);
					}

					ov_clear(&vf);
					
					paErr = Pa_OpenDefaultStream( &music_stream, 0, 2, paFloat32, 44100, 
												paFramesPerBufferUnspecified,
												&Audio::paCallback, music);
					logPaError();
				}
				
				void playMusic(bool loop = false) {
					if(music ) {
						music->loop = loop;
						paErr = Pa_StartStream(music_stream);
						logPaError();
						log("playing...");
						music->paused = false;
					}
				}
				
				void pauseMusic() {
					if(music) {
						paErr = Pa_StopStream(music_stream);
						logPaError();
						log("paused...");
						music->paused = true;
					}
				}
				
				void freeMusic() {
					if(music_stream) {
						paErr = Pa_CloseStream(music_stream);
						logPaError();
						music_stream = 0;
					}

					if(music) {
						if (music->left) {
							delete[] music->left;
						}
							
						if (music->right) {
							delete[] music->right;
						}
						delete music;
						music = 0;
					}
				}
				
				static Audio *getInstance()
				{
					if (!_singleton) {
						_singleton =  new Audio;
					}

					return _singleton;
				}

				static void free()
				{
					if (_singleton)
					{
						delete _singleton;
						_singleton = 0;
					}
				}
		};
	}
#endif