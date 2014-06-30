#ifndef MAMMOTH3D_AUDIO_HPP
#define MAMMOTH3D_AUDIO_HPP

	#include <iostream>
	#include <cstdlib>
    #include <cmath>

	#include "vorbis/vorbisfile.h"
	#include "portaudio/portaudio.h"
								   
	namespace audio {
		class Audio {
			private:
				Audio(const char* file_name = 0);
				~Audio();
				
				void logPaError() {
					if( _pa_error_code != paNoError ) {
						log("PortAudio error: ", Pa_GetErrorText(_pa_error_code));
					}
				}
				
				template <typename T>
				void log(const std::string &str, T param) {
					std::cout << "[Audio       ] " << str << param << std::endl;
				}

				void log(const std::string &str) {
					std::cout << "[Audio       ] " << str << std::endl;
				}

				struct musicData {
					float *left;
					float *right;
					unsigned long frames;
					unsigned long position;
					double callbackStartTime;
					float volume;
					bool started;
					bool loop;
					bool paused;
					bool finished;
					bool backward;
				};
		
				static int paCallback( const void *input_buffer,
						void *output_buffer,
						unsigned long frames_per_buffer,
						const PaStreamCallbackTimeInfo* time_info,
						PaStreamCallbackFlags status_flags,
						void *user_data);
				
				Audio(const Audio&);
				void operator=(const Audio&);
                
                static Audio *_singleton;
				
				PaError _pa_error_code;
				PaStream *_music_stream;
				musicData *_music;
				
				double _sample_rate;
				
				bool _terminatePa;

			public:
				void loadMusic(const char* file_name);
                void playMusic(bool loop = false);
                void pauseMusic();
                void freeMusic();
				
				void setMusicPosition(unsigned long position) {
					_music->position = position * _sample_rate;
				}
				
				unsigned long getMusicPosition() {
					return _music->position;
				}
				
				double getMusicTime() {
					/*if (music->callbackStartTime <= 0.0) {
						return 0.0;
					}
					
					double stream_time = Pa_GetStreamTime(music_stream)-music->callbackStartTime;
					if (stream_time < 0.0) {
						return 0.0;
					}*/
					
					return ((double)_music->position)/_sample_rate;//stream_time;
				}
				
				void setMusicVolume(float volume) {
					if(_music) {
						_music->volume = volume;
					}
				}
				
				bool isMusicFinished() {
					if (_music->loop) {
						return false;
					}

					return _music->finished;
				}
				
				double getMusicLength() {
					if(_music) {
						return (double)_music->frames/_sample_rate;
					}
					
					return 0;
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