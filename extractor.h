#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#include <fstream>
#include <memory>
#include <vector>
#include "track.h"

#define RATE 44100
#define LOOP 2
#define EXTEND 20
#define FADEOUT 10
#define CHUNK 512

int load_info(const char*, TrackList&);
int extract_one(int, std::ifstream&, TrackList&, std::string);
int extract_all(std::ifstream&, TrackList&, std::string);

#endif
