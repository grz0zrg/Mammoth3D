#include "builtingeometry.hpp"

const unsigned int builtingeometry::quadIndice[] = {0, 1, 2,
													2, 3, 0};

const float builtingeometry::quadVertice[] = {-1.0f,  1.0f, 0.0f,
											  -1.0f, -1.0f, 0.0f,
											   1.0f, -1.0f, 0.0f,
										       1.0f,  1.0f, 0.0f};
					
const float builtingeometry::quadUV[] = {0.0f, 1.0f, 
									     0.0f, 0.0f,
									     1.0f, 0.0f,
										 1.0f, 1.0f};