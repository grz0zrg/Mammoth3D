#include "synctracker_controller.hpp"

sync::SyncTrackerController *sync::SyncTrackerController::_singleton = 0;
											
void sync::SyncTrackerController::update() {
	joystick->update();
			
	if (stracker->_tracks.empty()) {
		track = -1;
		return;
	}
					
	if (joystick->buttonReleased(B_L1)) {
		if (track+1 < (int)stracker->_tracks.size()) {
			track++;
			logState();
		}
	}
					
	if (joystick->buttonReleased(B_R1)) {
		if (track-1 >= 0) {
			track--;
			logState();
		}
	}
					
	if (joystick->buttonReleased(B_L2)) { // L (7 = T)
		mode++;
		mode %= 2;
		
		std::cout << "Mode: " << mode << std::endl;
	}
					
	if (joystick->buttonReleased(B_SELECT)) { // back
		stracker->save();
	}
					
	if (joystick->buttonReleased(B_START)) { // start
		audio->setMusicPosition(0);
	}
					
	if (joystick->buttonReleased(B_1)) {
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
					
	if (joystick->buttonReleased(B_3)) {
		if (stracker->deleteRow(stracker->_tracks[track]->_name, irow)) {
			std::cout << "Row deleted: " << irow << std::endl;
		}
	}
			
	if (joystick->buttonReleased(B_2)) {
		stracker->setRow(stracker->_tracks[track]->_name, irow, value, interpolation_type);
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
					
	if (joystick->buttonReleased(B_4)) {
		block_loop = !block_loop;
				
		if (block_loop) {
			block_loop_start = irow;
			std::cout << "Block loop: " << block_loop_start << " to " << (block_loop_start+block_loop_length) << std::endl;
		}
	}
					
	if (block_loop) {
		int prow = (int)floor(stracker->getRow(audio->getMusicTime()));
		if (prow > block_loop_start+block_loop_length /*|| 
			prow < block_loop_start*/) {
			audio->setMusicPosition(stracker->getSongPosition(block_loop_start));
		}
	}
}