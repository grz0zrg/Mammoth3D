#ifndef MAMMOTH3D_SYNCTRACKER_CONTROLS_HPP
#define MAMMOTH3D_SYNCTRACKER_CONTROLS_HPP

	#include "synctracker.hpp"
	#include "synctracker_xrns.hpp"
	#include "../audio/audio.hpp"
	#include "../joystick/joystick.hpp"
	
	// used to control the synctracker within the app with a gamepad/joystick
	// default mapping is for a PS2 gamepad but can be changed easily by editing e_inputType
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
			
			enum e_inputType{	B_1 = 0, B_2 = 2, B_3 = 3, B_4 = 1,
								B_L1 = 4, B_R1 = 6, B_L2 = 5, B_R2 = 7, B_START = 9, B_SELECT = 8};
			
			public:
				void update();
				
				void logState() {
					std::cout << "T: " << stracker->_tracks[track]->_name << 
								 " Id: " << track <<
								 " Row: "  << irow << std::endl;
				}
			
				static SyncTrackerController *getInstance()
				{
					if (!_singleton) {
						_singleton =  new SyncTrackerController;
						
						std::atexit(free);
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