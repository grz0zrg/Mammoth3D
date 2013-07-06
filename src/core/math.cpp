#include "math.hpp"

double core::math::PI = 3.141592654;

float core::math::deg2rad(float deg) {
	return deg * PI / 180;
}

float core::math::rad2deg(float rad) {
	return rad * 180 / PI;
}

float core::math::clamp(float x, float a, float b)
{
    return x < a ? a : (x > b ? b : x);
}

bool core::math::isPowerOfTwo(int x) {
	return (!(x & (x - 1)) && x);
}