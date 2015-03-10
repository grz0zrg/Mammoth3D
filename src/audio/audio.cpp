#include "audio.hpp"

audio::Audio *audio::Audio::_singleton = 0;

audio::Audio::Audio(const char* file_name) {
    _pa_error_code = Pa_Initialize();
					
    _music = 0;
    _music_stream = 0;
					
    _sample_rate = 44100;
                    
    if (_pa_error_code == paNoError) {
        _terminatePa = true;
                        
        loadMusic(file_name);
    } else {
        _terminatePa = false;
            
        logPaError();
    }
}
   

audio::Audio::~Audio() {
    freeMusic();
    if(_terminatePa) {
        _pa_error_code = Pa_Terminate();
        
        logPaError();
    }
}

void audio::Audio::loadMusic(const char* file_name) {
    if (!file_name) return;
                    
    freeMusic();

    log("loading: ", file_name);
					
    OggVorbis_File vf;
                    
    _music = new musicData;
    if(!_music) {
        _music = 0;
        log("alloc error");
                        
        return;
    }
    
    _music->left              = _music->right = 0;
    _music->position          = 0;
    _music->loop              = false;
    _music->paused            = false;
    _music->finished          = false;
    _music->started           = false;
    _music->volume            = 1.0;
    _music->callbackStartTime = 0.0f;
					
    // load & read the ogg file
    int err = ov_fopen(file_name, &vf);
    if(err < 0) {
        log("ov_fopen error code: ", err);
        freeMusic();
					
        return;
    }
					
    unsigned long length = (long)ov_pcm_total(&vf, -1);
                    
    _music->left  = new float[length];
    _music->right = new float[length];
                    
    _music->frames = length;
                    
    if( !_music->left || !_music->right ) {
        log("alloc error");
        ov_clear( &vf );
        freeMusic();
						
        return;
    }
					
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
                _music->left[frame + offset]  = samples[0][frame];
                _music->right[frame + offset] = samples[1][frame];

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
					
    const PaDeviceInfo *deviceInfo;

    // open a stream
    /*
        * TODO: This is not working under Linux.
        * 

            PaStreamParameters psto;
            psto.device = Pa_GetDefaultOutputDevice();
            psto.sampleFormat = paFloat32;
            psto.channelCount = 2;
            psto.suggestedLatency = Pa_GetDeviceInfo(psto.device)->defaultLowOutputLatency;
                        
            int framesPerBuffer = 512;
                        
                        
                         
            _pa_error_code = Pa_OpenStream( &_music_stream, NULL, 
                                            &psto, _sample_rate, 
                                            framesPerBuffer, paNoFlag,
                                            &Audio::paCallback, _music);
    */
                    
    _pa_error_code = Pa_OpenDefaultStream( &_music_stream, 0, 2, 
                                            paFloat32, _sample_rate, 
                                            paFramesPerBufferUnspecified,
                                            &Audio::paCallback, _music);

    logPaError();
}

void audio::Audio::playMusic(bool loop, float volume) {
	setMusicVolume(volume);
	
    if(_music ) {
        _pa_error_code = Pa_StartStream(_music_stream);
                
        if (_pa_error_code == paNoError) {
            _music->paused = false;
            _music->loop = loop;
                            
            log("playing...");
        } else {
            logPaError();
        }
    }
}

void audio::Audio::pauseMusic() {
    if(_music) {
        _pa_error_code = Pa_StopStream(_music_stream);
        logPaError();
                        
        _music->paused = true;
                        
        log("paused...");
    }
}

void audio::Audio::freeMusic() {
    if(_music_stream) {
        _pa_error_code = Pa_CloseStream(_music_stream);
        logPaError();
                        
        _music_stream = 0;
    }

    if(_music) {
        if (_music->left) {
            delete[] _music->left;
        }
							
        if (_music->right) {
            delete[] _music->right;
        }
                        
        delete _music;
        _music = 0;
    }
}
             
int audio::Audio::paCallback( const void *input_buffer, void *output_buffer,
                           unsigned long frames_per_buffer,
                           const PaStreamCallbackTimeInfo* time_info,
                           PaStreamCallbackFlags status_flags,
                           void *user_data )
{ 
    float *out = (float*)output_buffer;
    unsigned int i;

    (void) status_flags;
    (void) input_buffer;
	audio::Audio::musicData *data = (audio::Audio::musicData*)user_data;

	/*if(data->finished || data->paused) {
		data->started = false;

		return 0;
	}
	
	if (!data->started) {
		data->callbackStartTime = timeInfo->outputBufferDacTime;
		data->started = true;
	}*/

    unsigned long start = data->position;
    float *left = data->left;
    float *right = data->right;

	long end;
    if(data->position + frames_per_buffer >= data->frames - 1) {
        end = data->frames - 1;
    } else {
        end = data->position + frames_per_buffer;
    }
	
	data->position = end;
	
	if(data->position >= data->frames - 1) {
		if(data->loop) {
			data->position = 0;
		} else {
			data->finished = true;
			data->position = 0;
		}
	}

    for(int j = start; j < end; ++j ) {
        *out = left[j] * data->volume;
        out++;
        *out = right[j] * data->volume;
        out++;
    }
	
	// clamp to [-1.0,1.0], it seem portaudio do it already? 
    /*
    unsigned long j;
    for(j = 0; j < framesPerBuffer; ++j) {
        if( out[j] > 1.0 ) out[j] = 1.0;
        if( out[j] < -1.0 ) out[j] = -1.0;
    }
    */
	
	return 0;
}