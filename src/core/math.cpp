#include "math.hpp"

double core::math::PI = 3.141592654;

float core::math::deg2rad(float deg) {
	return deg * PI / 180;
}

float core::math::rad2deg(float rad) {
	return rad * 180 / PI;
}