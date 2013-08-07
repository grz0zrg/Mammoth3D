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
				input_mode = false;
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
			bool input_mode;
			
			public:
				void update() {
					joystick->update();
					
					if (stracker->tracks.empty()) {
						track = -1;
						return;
					}
					
					if (joystick->isButtonReleased(13)) {
						if (track+1 < (int)stracker->tracks.size()) {
							track++;
							logState();
						}
					}
					
					if (joystick->isButtonReleased(15)) {
						if (track-1 >= 0) {
							track--;
							logState();
						}
					}
					
					if (joystick->isButtonReleased(5)) { // R (7 = T)
					}
					
					if (joystick->isButtonReleased(8)) { // back
						stracker->save();
					}
					
					if (joystick->isButtonReleased(9)) { // start
						audio->setSongPosition(0);
					}
					
					if (joystick->isButtonReleased(0)) {
						if (interpolation_type == STEP) {
							interpolation_type = LINEAR;
							std::cout << "Interpolation type: LINEAR" << std::endl;
						} else if (interpolation_type == LINEAR) {
							interpolation_type = SMOOTH;
							std::cout << "Interpolation type: SMOOTH" << std::endl;
						} else if (interpolation_type == SMOOTH) {
							interpolation_type = RAMP;
							std::cout << "Interpolation type: RAMP" << std::endl;
						} else if (interpolation_type == RAMP) {
							interpolation_type = STEP;
							std::cout << "Interpolation type: STEP" << std::endl;
						}
					}
					
					if (joystick->isButtonReleased(3)) {
						if (stracker->deleteRow(stracker->tracks[track]->name, irow)) {
							std::cout << "Row deleted: " << irow << std::endl;
						}
					}
					
					if (joystick->isButtonReleased(2)) {
						stracker->setRow(stracker->tracks[track]->name, irow, value, interpolation_type);
						logState();
						std::cout << "Row value set to: " << value << std::endl;
					}
					
					lastRow = (int)floorf(row);
					
					float a1_y = joystick->getAxe(1);
					float a2_y = joystick->getAxe(2);
					
					if (a1_y > 0.25f || a1_y < -0.25f) {
						row -= a1_y / 15.0f;
						if (row < 0.0f) {
							row = 0.0f;
						}
							
						irow = (int)floorf(row);
						if (irow != lastRow) {
							logState();
						}
					}
					
					if (a2_y > 0.25f || a2_y < -0.25f) {
						value += a2_y / 150.0f;
						std::cout << "Value: " << value << std::endl;
					}
					
					if (joystick->isButtonReleased(1)) {
						block_loop = !block_loop;
						
						if (block_loop) {
							block_loop_start = irow;
							std::cout << "Block loop: " << block_loop_start << " to " << (block_loop_start+block_loop_length) << std::endl;
						}
					}
					
					if (block_loop) {
						int prow = (int)floor(stracker->getRow(audio->getSongTime()));
						if (prow > block_loop_start+block_loop_length /*|| 
							prow < block_loop_start*/) {
							audio->setSongPosition(stracker->getSongPosition(block_loop_start));
						}
					}
				}
				
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