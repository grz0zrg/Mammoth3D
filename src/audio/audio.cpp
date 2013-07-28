#include "audio.hpp"
#include <cmath>

audio::Audio *audio::Audio::_singleton = 0;

int audio::Audio::paCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{ 
    float *out = (float*)outputBuffer;
    unsigned int i;

    (void) statusFlags;
    (void) inputBuffer;
	audio::Audio::musicData *data = (audio::Audio::musicData*)userData;

	if(data->finished || data->paused) {
		data->started = false;

		return 0;
	}
	
	if (!data->started) {
		data->callbackStartTime = timeInfo->outputBufferDacTime;
		data->started = true;
	}

    unsigned long start = data->position;
    float *left = data->left;
    float *right = data->right;

	long end;
    if(data->position + framesPerBuffer >= data->frames - 1) {
        end = data->frames - 1;
    } else {
        end = data->position + framesPerBuffer;
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
	
	// clamp to [-1.0,1.0] domain, seem portaudio do it already? 
    /*unsigned long j;
    for(j = 0; j < framesPerBuffer; ++j) {
        if( out[j] > 1.0 ) out[j] = 1.0;
        if( out[j] < -1.0 ) out[j] = -1.0;
    }*/
	
	return 0;
}