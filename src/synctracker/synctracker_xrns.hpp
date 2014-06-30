#ifndef MAMMOTH3D_SYNCTRACKER_XRNS_HPP
#define MAMMOTH3D_SYNCTRACKER_XRNS_HPP

	#include "synctracker.hpp"
	
	#include "tinyxml2/tinyxml2.h"
	
	// this allow XRNS patterns data to be imported
	namespace sync {
		class SyncTrackerXRNS {
			class Line {
				public:
					Line(int index) {
						_index = index;
					}
					~Line() {}
					
					int _index;
			};
			
			class Track {
				public:
					Track() {}
					~Track() {
						for (unsigned int i = 0; i < _lines.size(); i++) {
							delete _lines[i];
						}
					}
					
					std::string _name;
					std::vector<Line *> _lines;
			};
			
			class Pattern {
				public:
					Pattern(int lines) {
						_lines = lines;
					}

					~Pattern() {
						for (unsigned int i = 0; i < _tracks.size(); i++) {
							delete _tracks[i];
						}
					}
					
					int _lines;
					std::vector<Track *> _tracks;
			};
			
			SyncTrackerXRNS() {
				_stracker = SyncTracker::getInstance();
			}
			
			~SyncTrackerXRNS() {
				freeSong();
			}
			
			SyncTrackerXRNS(const SyncTrackerXRNS&);
			void operator=(const SyncTrackerXRNS&);
			static SyncTrackerXRNS *_singleton;
			
			SyncTracker *_stracker;
			
			std::vector<Pattern *> _patterns;
			std::vector<Track *> _tracks;
			
			public:
				void parseSong(const std::string& xrns);
				
				void freeSong() {
					for (unsigned int i = 0; i < _patterns.size(); i++) {
						delete _patterns[i];
					}
					
					for (unsigned int i = 0; i < _tracks.size(); i++) {
						delete _tracks[i];
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