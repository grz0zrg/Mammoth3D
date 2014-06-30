#ifndef MAMMOTH3D_SYNCTRACKER_HPP
#define MAMMOTH3D_SYNCTRACKER_HPP

	// heavily inspired by GNU Rocket http://rocket.sourceforge.net/
	// all interpolations methods come directly from it
	
	#include <vector>
	#include <string>
	#include <cmath>
	#include <iostream>
	#include <algorithm>
	#include <fstream>
	#include <cstdlib>

	namespace sync {
		enum InterpolationType {
			STEP,
			LINEAR,
			SMOOTH,
			RAMP
		};

		class SyncTracker {
			SyncTracker() {
				_bpm = 100.0f;
				_lpb = 4.0f;
			}
			
			~SyncTracker() {
				freeTracks();
			}
			
			SyncTracker(const SyncTracker&);
			void operator=(const SyncTracker&);
			static SyncTracker *_singleton;
			
			public:
				class Track {
					public:
						class Row {
							public:
								Row(float value, InterpolationType type) {
									setValue(value, type);
								}
								
								~Row() { };
								
								void setValue(float value, InterpolationType type) {
									_value = value;
									_type = type;	
								}
								
								int _row; 

								float _value;
								InterpolationType _type;
						};
						
						Track(const std::string& name, float *linked_value) {
							_name = name;
							_linked_value = linked_value;
						}
						
						~Track() {
							clear();
						}
						
						static bool sortRows(Row *t1, Row *t2) { 
							return (t1->_row < t2->_row); 
						}
						
						void sort() {
							std::sort(_rows.begin(), _rows.end(), sortRows);
						}
						
						void addRow(int row_index, float value, InterpolationType type) {
							Row *row = new Row(value, type);
							_rows.push_back(row);
							row->_row = row_index;
						}
						
						void setRow(int index, float value, InterpolationType type) {
							_rows[index]->setValue(value, type);
						}
						
						void deleteRow(int index) {
							delete _rows[index];
							
							_rows.erase(_rows.begin()+index);
							sort();
						}
						
						float linear(int row_index, double row) {
							Row *row1 = _rows[row_index];
							Row *row2 = _rows[row_index+1];
							
							double t = (row - row1->_row) / (row2->_row - row1->_row);
							return (float)(row1->_value + (row2->_value - row1->_value) * t);
						}
						
						float smooth(int row_index, double row) {
							Row *row1 = _rows[row_index];
							Row *row2 = _rows[row_index+1];
							
							double t = (row - row1->_row) / (row2->_row - row1->_row);
							t = t * t * (3 - 2 * t);
							return (float)(row1->_value + (row2->_value - row1->_value) * t);
						}
						
						float ramp(int row_index, double row) {
							Row *row1 = _rows[row_index];
							Row *row2 = _rows[row_index+1];
							
							double t = (row - row1->_row) / (row2->_row - row1->_row);
							t = pow(t, 2.0);
							return (float)(row1->_value + (row2->_value - row1->_value) * t);
						}
						
						void clear() {
							for (unsigned int i = 0; i < _rows.size(); i++) {
								delete _rows[i];
							}
							
							_rows.clear();
						}
						
						std::string _name;
						float *_linked_value;
						std::vector<Row *> _rows;
				};

				std::vector<Track *> _tracks;
				
				float _bpm, _lpb;
				
				void setBPM(float bpm) {
					_bpm = bpm;
				}
				
				void setLPB(float lpb) {
					_lpb = lpb;
				}
				
				void setTrack(const std::string& name, float *linked_value) {
					int track_id = findTrack(name);
					if (track_id >= 0) {
						_tracks[track_id]->_linked_value = linked_value;
						return;
					}

					Track *track = new Track(name, linked_value);
					_tracks.push_back(track);
				}
				
				void freeTracks() {
					for (unsigned int i = 0; i < _tracks.size(); i++) {
						delete _tracks[i];
					}
				}
				
				int findTrack(const std::string& name) {
					for (unsigned int i = 0; i < _tracks.size(); i++) {
						if (_tracks[i]->_name == name) {
							return i;
						}
					}
					
					return -1;
				}
				
				int findRow(Track *track, int row) {
					int lo = 0, hi = track->_rows.size();
					while (lo < hi) {
						int mi = (lo + hi) / 2;

						if (track->_rows[mi]->_row < row) {
							lo = mi + 1;
						} else if (track->_rows[mi]->_row > row) {
							hi = mi;
						} else {
							return mi;
						}
					}

					return -lo - 1;
				}
				
				int deleteRow(const std::string& track_name, int row_index) {
					int track_index = findTrack(track_name);
					if (track_index < 0) {
						return 0;
					}
					
					Track *track = _tracks[track_index];

					int id_row = findRow(track, row_index);
					if (id_row < 0) {
						return 0;
					}
					
					track->deleteRow(id_row);
					
					return 1;
				}
				
				void setRow(const std::string& track_name, 
							int row_index, float value, 
							InterpolationType type = STEP) {
					int track_index = findTrack(track_name);
					if (track_index < 0) {
						return;
					}
					
					Track *track = _tracks[track_index];
					
					int id_row = findRow(track, row_index);
					if (id_row < 0) {
						track->addRow(row_index, value, type);
						track->sort();
					} else {
						track->setRow(id_row, value, type);
					}
				}
				
				void update(double music_time) {
					const float default_value = 0.0f;
					double row_d = getRow(music_time);
					unsigned int row_index = (int)floor(row_d);
					
					for (unsigned int i = 0; i < _tracks.size(); i++) {
						Track *track = _tracks[i];
						
						if (track->_linked_value == 0) {
							continue;
						}
						
						int id_row = findRow(track, row_index);
						if (id_row < 0) {
							id_row = -id_row - 2;
						}
						
						if (id_row < 0) {
							*track->_linked_value = default_value;
							continue;
						}
						
						if (id_row > (int)track->_rows.size() - 2) {
							*track->_linked_value = track->_rows[track->_rows.size()-1]->_value;
							continue;
						}
						
						Track::Row *row = track->_rows[id_row];
						switch(row->_type) {
							case STEP:
								*track->_linked_value = row->_value;
								break;
								
							case LINEAR:
								*track->_linked_value = track->linear(id_row, row_d);
								break;
			
							case SMOOTH:
								*track->_linked_value = track->smooth(id_row, row_d);
								break;
								
							case RAMP:
								*track->_linked_value = track->ramp(id_row, row_d);
								break;
							
							default:
								*track->_linked_value = default_value;
								break;
						}
					}
				}
				
				void save() {
					std::ofstream file("sync.mms", std::ios::out | std::ios::binary);
					if (!file.good() || !file.is_open()) {
						std::cout << "[SyncTracker] Failed to save \"sync.mms\"" << std::endl;
						return;
					}

					file.write((char*)&_bpm, sizeof(_bpm));
					file.write((char*)&_lpb, sizeof(_lpb));
					
					unsigned int numTracks = _tracks.size();
					file.write((char*)&numTracks, sizeof(numTracks));
					
					for (unsigned int i = 0; i < _tracks.size(); i++) {
						Track *track = _tracks[i];
						
						unsigned int nameLength = track->_name.length()+1;
						file.write((char*)&nameLength, sizeof(nameLength));
						file.write(track->_name.c_str(), sizeof(char)*nameLength);
						
						unsigned int numRows = track->_rows.size();
						file.write((char*)&numRows, sizeof(numRows));
						
						for (unsigned int j = 0; j < track->_rows.size(); j++) {
							Track::Row *row = track->_rows[j];
							
							file.write((char*)&row->_row, sizeof(row->_row));
							file.write((char*)&row->_value, sizeof(row->_value));
							file.write((char*)&row->_type, sizeof(row->_type));
						}
					}
			
					file.close();
					
					std::cout << "[SyncTracker] \"sync.mms\" saved" << std::endl;
				}
				
				void load(const std::string& filename) {
					std::ifstream file;
					file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
					
					try {
						file.open(filename.c_str(), std::ios::in|std::ios::binary);

						if (!file.is_open()) {
							std::cout << "[SyncTracker] Unable to load " << filename << std::endl;
							return;
						} 
						
						std::cout << "[SyncTracker] loading " << filename << std::endl;
						
						freeTracks();
						
						file.read((char*)&_bpm, sizeof(_bpm));
						file.read((char*)&_lpb, sizeof(_lpb));
						
						unsigned int numTracks = 0;
						file.read((char*)&numTracks, sizeof(numTracks));
						
						for (unsigned int i = 0; i < numTracks; i++) {
							unsigned int nameLength = 0;
							file.read((char*)&nameLength, sizeof(nameLength));
							
							char *c_name = new char[nameLength];
							file.read(c_name, sizeof(char)*nameLength);
							std::string track_name(c_name);
							
							delete[] c_name;
							
							unsigned int numRows = 0;
							file.read((char*)&numRows, sizeof(numRows));
							
							Track *track = new Track(track_name, 0);
							_tracks.push_back(track);
							
							for (unsigned int j = 0; j < numRows; j++) {
								Track::Row *row = new Track::Row(0.0f, STEP);
								
								file.read((char*)&row->_row, sizeof(row->_row));
								file.read((char*)&row->_value, sizeof(row->_value));
								file.read((char*)&row->_type, sizeof(row->_type));
								
								track->_rows.push_back(row);
							}
						}
						
						file.close();
					} catch (std::ifstream::failure e) {
						std::cout << "[SyncTracker] Unable to load " << filename << std::endl;
					}
				}
				
				double getRow(double music_time) {
					return (music_time * (_bpm/60) * _lpb);
				}
				
				unsigned long getSongPosition(double row) {
					return (unsigned long)(row / (_bpm/60) / _lpb);
				}
			
				static SyncTracker *getInstance()
				{
					if (!_singleton) {
						_singleton =  new SyncTracker;
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