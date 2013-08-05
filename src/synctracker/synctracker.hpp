#ifndef SYNCTRACKER_HPP
#define SYNCTRACKER_HPP

	// heavily inspired by GNU Rocket http://rocket.sourceforge.net/
	// all interpolations methods come directly from it
	
	#include <vector>
	#include <string>
	#include <cmath>
	#include <iostream>
	#include <algorithm>
	#include <fstream>

	namespace sync {
		enum InterpolationType {
			STEP,
			LINEAR,
			SMOOTH,
			RAMP
		};

		class SyncTracker {
			SyncTracker() {
				bpm = 100.0f;
				lpb = 4.0f;
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
									this->value = value;
									this->type = type;	
								}
								
								int row; 

								float value;
								InterpolationType type;
						};
						
						Track(const std::string& name, float *linked_value) {
							this->name = name;
							this->linked_value = linked_value;
						}
						
						~Track() {
							clear();
						}
						
						static bool sortRows(Row *t1, Row *t2) { 
							return (t1->row < t2->row); 
						}
						
						void sort() {
							std::sort(rows.begin(), rows.end(), sortRows);
						}
						
						void addRow(int row_index, float value, InterpolationType type) {
							Row *row = new Row(value, type);
							rows.push_back(row);
							row->row = row_index;
						}
						
						void setRow(int index, float value, InterpolationType type) {
							rows[index]->setValue(value, type);
						}
						
						void deleteRow(int index) {
							delete rows[index];
							
							rows.erase(rows.begin()+index);
							sort();
						}
						
						float linear(int row_index, double row) {
							Row *row1 = rows[row_index];
							Row *row2 = rows[row_index+1];
							
							double t = (row - row1->row) / (row2->row - row1->row);
							return (float)(row1->value + (row2->value - row1->value) * t);
						}
						
						float smooth(int row_index, double row) {
							Row *row1 = rows[row_index];
							Row *row2 = rows[row_index+1];
							
							double t = (row - row1->row) / (row2->row - row1->row);
							t = t * t * (3 - 2 * t);
							return (float)(row1->value + (row2->value - row1->value) * t);
						}
						
						float ramp(int row_index, double row) {
							Row *row1 = rows[row_index];
							Row *row2 = rows[row_index+1];
							
							double t = (row - row1->row) / (row2->row - row1->row);
							t = pow(t, 2.0);
							return (float)(row1->value + (row2->value - row1->value) * t);
						}
						
						void clear() {
							for (unsigned int i = 0; i < rows.size(); i++) {
								delete rows[i];
							}
							
							rows.clear();
						}
						
						std::string name;
						float *linked_value;
						std::vector<Row *> rows;
				};

				std::vector<Track *> tracks;
				
				float bpm, lpb;
				
				void setBPM(float bpm) {
					this->bpm = bpm;
				}
				
				void setLPB(float lpb) {
					this->lpb = lpb;
				}
				
				void setTrack(const std::string& name, float *linked_value) {
					int track_id = findTrack(name);
					if (track_id >= 0) {
						tracks[track_id]->linked_value = linked_value;
						return;
					}

					Track *track = new Track(name, linked_value);
					tracks.push_back(track);
				}
				
				void freeTracks() {
					for (unsigned int i = 0; i < tracks.size(); i++) {
						delete tracks[i];
					}
				}
				
				int findTrack(const std::string& name) {
					for (unsigned int i = 0; i < tracks.size(); i++) {
						if (tracks[i]->name == name) {
							return i;
						}
					}
					
					return -1;
				}
				
				int findRow(Track *track, int row) {
					int lo = 0, hi = track->rows.size();
					while (lo < hi) {
						int mi = (lo + hi) / 2;

						if (track->rows[mi]->row < row) {
							lo = mi + 1;
						} else if (track->rows[mi]->row > row) {
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
					
					Track *track = tracks[track_index];

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
					
					Track *track = tracks[track_index];
					
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
					
					for (unsigned int i = 0; i < tracks.size(); i++) {
						Track *track = tracks[i];
						
						if (track->linked_value == 0) {
							continue;
						}
						
						int id_row = findRow(track, row_index);
						if (id_row < 0) {
							id_row = -id_row - 2;
						}
						
						if (id_row < 0) {
							*track->linked_value = default_value;
							continue;
						}
						
						if (id_row > (int)track->rows.size() - 2) {
							*track->linked_value = track->rows[track->rows.size()-1]->value;
							continue;
						}
						
						Track::Row *row = track->rows[id_row];
						switch(row->type) {
							case STEP:
								*track->linked_value = row->value;
								break;
								
							case LINEAR:
								*track->linked_value = track->linear(id_row, row_d);
								break;
			
							case SMOOTH:
								*track->linked_value = track->smooth(id_row, row_d);
								break;
								
							case RAMP:
								*track->linked_value = track->ramp(id_row, row_d);
								break;
							
							default:
								*track->linked_value = default_value;
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

					file.write((char*)&bpm, sizeof(bpm));
					file.write((char*)&lpb, sizeof(lpb));
					
					unsigned int numTracks = tracks.size();
					file.write((char*)&numTracks, sizeof(numTracks));
					
					for (unsigned int i = 0; i < tracks.size(); i++) {
						Track *track = tracks[i];
						
						unsigned int nameLength = track->name.length()+1;
						file.write((char*)&nameLength, sizeof(nameLength));
						file.write(track->name.c_str(), sizeof(char)*nameLength);
						
						unsigned int numRows = track->rows.size();
						file.write((char*)&numRows, sizeof(numRows));
						
						for (unsigned int j = 0; j < track->rows.size(); j++) {
							Track::Row *row = track->rows[j];
							
							file.write((char*)&row->row, sizeof(row->row));
							file.write((char*)&row->value, sizeof(row->value));
							file.write((char*)&row->type, sizeof(row->type));
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
							std::cout << "[SyncTracker] Failed to load " << filename << std::endl;
							return;
						} 
						
						std::cout << "[SyncTracker] loading " << filename << std::endl;
						
						freeTracks();
						
						file.read((char*)&bpm, sizeof(bpm));
						file.read((char*)&lpb, sizeof(lpb));
						
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
							tracks.push_back(track);
							
							for (unsigned int j = 0; j < numRows; j++) {
								Track::Row *row = new Track::Row(0.0f, STEP);
								
								file.read((char*)&row->row, sizeof(row->row));
								file.read((char*)&row->value, sizeof(row->value));
								file.read((char*)&row->type, sizeof(row->type));
								
								track->rows.push_back(row);
							}
						}
						
						file.close();
					} catch (std::ifstream::failure e) {
						std::cout << "[SyncTracker] Failed to load " << filename << std::endl;
					}
				}
				
				double getRow(double music_time) {
					return (music_time * (bpm/60) * lpb);
				}
				
				unsigned long getSongPosition(double row) {
					return (unsigned long)(row / (bpm/60) / lpb);
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