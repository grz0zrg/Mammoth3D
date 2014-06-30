#include "synctracker_xrns.hpp"

sync::SyncTrackerXRNS *sync::SyncTrackerXRNS::_singleton = 0;

void sync::SyncTrackerXRNS::parseSong(const std::string& xrns) {
	freeSong();
					
	tinyxml2::XMLDocument doc;
					
	int ret = doc.LoadFile(xrns.c_str());
	if (ret != tinyxml2::XML_NO_ERROR) {
		std::cout << "[SyncTracker] Unable to load: " << xrns << " error code: " << ret << std::endl;
		return;
	}

	std::cout << "[SyncTracker] parsing: " << xrns << std::endl;
				
	tinyxml2::XMLElement* rootElement = doc.FirstChildElement("RenoiseSong");
	if (!rootElement) {
		std::cout << "[SyncTracker] Missing \"RenoiseSong\" tag, aborting." << std::endl;
		return;
	}
					
	tinyxml2::XMLElement* globalSongDataElement = rootElement->FirstChildElement("GlobalSongData");
	if(!globalSongDataElement) {
		std::cout << "[SyncTracker] Missing \"GlobalSongData\" tag, aborting." << std::endl;
		return;
	}
					
	// pattern sequences
	tinyxml2::XMLElement* patternSequence = rootElement->FirstChildElement("PatternSequence");
	if(!patternSequence) {
		std::cout << "[SyncTracker] Missing \"patternSequence\" tag, aborting." << std::endl;
		return;
	}

	tinyxml2::XMLElement* sequenceEntries = patternSequence->FirstChildElement("SequenceEntries");
	if(!sequenceEntries) {
		std::cout << "[SyncTracker] Missing \"SequenceEntries\" tag, aborting." << std::endl;
		return;
	}

	// tracks
	tinyxml2::XMLElement* tracks_element = rootElement->FirstChildElement("Tracks");
	if(!tracks_element) {
		std::cout << "[SyncTracker] Missing \"Tracks\" tag, aborting." << std::endl;
		return;
	}
					
	// patterns/tracks data
	tinyxml2::XMLElement* patternPool = rootElement->FirstChildElement("PatternPool");
	if(!patternPool) {
		std::cout << "[SyncTracker] Missing \"PatternPool\" tag, aborting." << std::endl;
		return;
	}

	tinyxml2::XMLElement* patterns_element = patternPool->FirstChildElement("Patterns");
	if(!patterns_element) {
		std::cout << "[SyncTracker] Missing \"Patterns\" tag, aborting." << std::endl;
		return;
	}
					
	tinyxml2::XMLElement* pattern_element = patterns_element->FirstChildElement("Pattern");
	if(!pattern_element) {
		std::cout << "[SyncTracker] Missing \"Pattern\" tag, aborting." << std::endl;
		return;
	}
					
	while(pattern_element) {
		tinyxml2::XMLElement* lines = pattern_element->FirstChildElement("NumberOfLines");
		if (!lines) {
			std::cout << "[SyncTracker] Missing \"NumberOfLines\" tag, aborting." << std::endl;
			freeSong();
			return;
		}
						
		int linesCount = atoi(lines->GetText());
						
		tinyxml2::XMLElement* tracks_element = pattern_element->FirstChildElement("Tracks");
		if (!tracks_element) {
			pattern_element = pattern_element->NextSiblingElement();
			continue;
		}
						
		tinyxml2::XMLElement* patterntrack_element = tracks_element->FirstChildElement("PatternTrack");
		if (!patterntrack_element) {
			pattern_element = pattern_element->NextSiblingElement();
			continue;
		}
						
		Pattern *pattern = new Pattern(linesCount);
						
		while(patterntrack_element) {
			tinyxml2::XMLElement* lines_element = patterntrack_element->FirstChildElement("Lines");
			if (!lines_element) {
				patterntrack_element = patterntrack_element->NextSiblingElement();
				continue;
			}
							
			Track *track = new Track();
							
			tinyxml2::XMLElement* line_element = lines_element->FirstChildElement("Line");
			while(line_element) {
				if (!line_element->Attribute("index")) {
					continue;
					line_element = line_element->NextSiblingElement();
				}
								
				int line_index = atoi(line_element->Attribute("index"));
							
				Line *line = new Line(line_index);
				track->_lines.push_back(line);
								
				line_element = line_element->NextSiblingElement();
			}
						
			pattern->_tracks.push_back(track);
							
			patterntrack_element = patterntrack_element->NextSiblingElement();
		}
						
		_patterns.push_back(pattern);
						
		pattern_element = pattern_element->NextSiblingElement();
	}
					
	// parse tracks
	tinyxml2::XMLElement* sequencertrack_element = tracks_element->FirstChildElement("SequencerTrack");
	while(sequencertrack_element) {
		tinyxml2::XMLElement* st_name = sequencertrack_element->FirstChildElement("Name");
					
		std::string tagName(sequencertrack_element->Name());
		if (tagName != "SequencerTrack") { // only standard tracks
			sequencertrack_element = sequencertrack_element->NextSiblingElement();
			continue;
		}
						
		Track *track = new Track();
		track->_name = std::string(st_name->GetText());
		_tracks.push_back(track);
						
		sequencertrack_element = sequencertrack_element->NextSiblingElement();
	}
					
	// parse sequences, final stage
	int currLine = 0; 
	tinyxml2::XMLElement* sequence_entry = sequenceEntries->FirstChildElement("SequenceEntry");
	while(sequence_entry) {
		tinyxml2::XMLElement* pattern_id_element = sequence_entry->FirstChildElement("Pattern");
						
		int pattern_id = atoi(pattern_id_element->GetText());
		Pattern *pattern = _patterns[pattern_id];
						
		for (unsigned int i = 0; i < pattern->_tracks.size(); i++) {
			Track *track = pattern->_tracks[i];
							
			for (unsigned int j = 0; j < track->_lines.size(); j++) {
				int index = track->_lines[j]->_index;
								
				Line *line = new Line(index+currLine);
				_tracks[i]->_lines.push_back(line);
			}
		}
						
		sequence_entry = sequence_entry->NextSiblingElement();
						
		currLine += pattern->_lines;
	}
					
	_stracker->_bpm = atoi(globalSongDataElement->FirstChildElement("BeatsPerMin")->GetText());
	_stracker->_lpb = atoi(globalSongDataElement->FirstChildElement("LinesPerBeat")->GetText());
}