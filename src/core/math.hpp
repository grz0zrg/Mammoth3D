#ifndef MATH_HPP
#define MATH_HPP
	
	#include <cmath>
	
	namespace core {
		namespace math {
			extern double PI;
			
			extern float deg2rad(float deg);
			
			extern float rad2deg(float rad);
			
			extern float clamp(float x, float a, float b);
			
			extern bool isPowerOfTwo(int x);
		}
	}

#endif