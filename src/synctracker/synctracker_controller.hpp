#ifndef SYNCTRACKER_CONTROLS_HPP
#define SYNCTRACKER_CONTROLS_HPP

	#include "synctracker.hpp"
	#include "synctracker_xrns.hpp"
	#include "../audio/audio.hpp"
	#include "../joystick/joystick.hpp"
	
	// used to control the synctracker within the app with a gamepad/joystick
	// default mapping is for a PS2 gamepad
	namespace sync {
		class SyncTrackerController {
			SyncTrackerController() {
				stracker = SyncTracker::getInstance();
				audio = audio::Audio::getInstance();
				
				joystick = new joystick::Joystick();
				
				track = 0;
				row = 0.0f;
				lastRow = 0;
				irow = 0;
				value = 0.0f;
				mode = 0;
				block_loop = false;
				block_loop_start = 0;
				block_loop_length = 16;
				interpolation_type = STEP;
			}
			
			~SyncTrackerController() {
				delete joystick;
			}
			
			SyncTrackerController(const SyncTrackerController&);
			void operator=(const SyncTrackerController&);
			static SyncTrackerController *_singleton;
			
			joystick::Joystick *joystick;
			
			audio::Audio *audio;
			
			SyncTracker *stracker;
			
			int track;
			int lastRow;
			float row;
			int irow;
			float value;
			int block_loop_start, block_loop_length;
			InterpolationType interpolation_type;
			bool block_loop;
			int mode;
			
			public:
				void update();
				
				void logState() {
					std::cout << "T: " << stracker->tracks[track]->name << 
								 " Id: " << track <<
								 " Row: "  << irow << std::endl;
				}
			
				static SyncTrackerController *getInstance()
				{
					if (!_singleton) {
						_singleton =  new SyncTrackerController;
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