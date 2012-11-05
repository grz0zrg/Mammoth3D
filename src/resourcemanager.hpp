#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

	#include <string>
	#include <iostream>

	namespace framework {
		class ResourceManager {
			private:
				ResourceManager() {
				}
				
				~ResourceManager() {
				}
				
				template <typename T>
				void logPretty(const std::string &str, T param) {
					std::cout << "[RsMng] " << str << "\"" << 
								param << "\"" << std::endl;
				}
				
				template <typename T>
				void log(const std::string &str, T param) {
					std::cout << "[RsMng] " << str << param << std::endl;
				}
					
				void log(const char *str) {
					std::cout << "[RsMng] " << str << std::endl;
				}
				
				ResourceManager(const ResourceManager&);
				void operator=(const ResourceManager&);
				static ResourceManager *_singleton;
	
			public:
				static ResourceManager *getInstance()
				{
					if (!_singleton) {
						_singleton =  new ResourceManager;
					}

					return _singleton;
				}

				static void kill()
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