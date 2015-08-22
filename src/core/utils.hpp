#ifndef MAMMOTH3D_UTILS_HPP
#define MAMMOTH3D_UTILS_HPP
	
	#include <string>
	#include <sstream>

	namespace core {
		namespace utils {
			template<typename T>
			std::string toString(const T v) {
				std::stringstream s;
				s << v;

				return s.str();
			}
		}
	}

#endif