#ifndef SYNCTRACKER_XRNS_HPP
#define SYNCTRACKER_XRNS_HPP

	#include "synctracker.hpp"
	
	#include "tinyxml2/tinyxml2.h"
	
	// this allow XRNS patterns data to be imported
	namespace sync {
		class SyncTrackerXRNS {
			class Line {
				public:
					Line(int index) {
						this->index = index;
					}
					~Line() {}
					
					int index;
			};
			
			class Track {
				public:
					Track() {}
					~Track() {
						for (unsigned int i = 0; i < lines.size(); i++) {
							delete lines[i];
						}
					}
					
					std::string name;
					std::vector<Line *> lines;
			};
			
			class Pattern {
				public:
					Pattern(int lines) {
						this->lines = lines;
					}

					~Pattern() {
						for (unsigned int i = 0; i < tracks.size(); i++) {
							delete tracks[i];
						}
					}
					
					int lines;
					std::vector<Track *> tracks;
			};
			
			SyncTrackerXRNS() {
				stracker = SyncTracker::getInstance();
			}
			
			~SyncTrackerXRNS() {
				freeSong();
			}
			
			SyncTrackerXRNS(const SyncTrackerXRNS&);
			void operator=(const SyncTrackerXRNS&);
			static SyncTrackerXRNS *_singleton;
			
			SyncTracker *stracker;
			
			std::vector<Pattern *> patterns;
			std::vector<Track *> tracks;
			
			public:
				void parseSong(const std::string& xrns);
				
				void freeSong() {
					for (unsigned int i = 0; i < patterns.size(); i++) {
						delete patterns[i];
					}
					
					for (unsigned int i = 0; i < tracks.size(); i++) {
						delete tracks[i];
					}
				}
			
				static SyncTrackerXRNS *getInstance()
				{
					if (!_singleton) {
						_singleton =  new SyncTrackerXRNS;
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