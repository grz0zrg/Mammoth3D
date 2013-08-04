#ifndef SYNCTRACKER_HPP
#define SYNCTRACKER_HPP

	// heavily inspired by GNU Rocket http://rocket.sourceforge.net/
	// all interpolations methods come directly from it
	
	#include <vector>
	#include <string>
	#include <cmath>
	#include <iostream>
	#include <algorithm>

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
				
				void addTrack(const std::string& name, float *linked_value) {
					if (findTrack(name) >= 0) {
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
					double row_d = music_time * (bpm/60) * lpb;
					unsigned int row_index = (int)floor(row_d);
					
					for (unsigned int i = 0; i < tracks.size(); i++) {
						Track *track = tracks[i];
						
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