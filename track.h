#ifndef TRACK_H
#define TRACK_H

#include <cstdint>
#include <vector>
#include <memory>

typedef struct Track {
    char* name;
    uint32_t intro_offset;
    uint32_t intro_size;
    uint32_t loop_offset;
    uint32_t loop_size;
} Track;

typedef std::shared_ptr<Track> TrackPtr;
typedef std::vector<TrackPtr> TrackList;

#endif
