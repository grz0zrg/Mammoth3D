#include "synctracker_controller.hpp"

sync::SyncTrackerController *sync::SyncTrackerController::_singleton = 0;

void sync::SyncTrackerController::update() {
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
		mode++;
		mode %= 2;
		
		std::cout << "Mode: " << mode << std::endl;
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