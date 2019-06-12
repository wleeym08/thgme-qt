#include <iostream>
#include <cmath>
#include "extractor.h"


int load_info(const char* fmt_path, 
    TrackList& track_list) {
    std::ifstream fmt_stream(fmt_path, 
        std::ios::binary | std::ios::in | std::ios::ate);
    if (!fmt_stream.is_open()) {
        return -1;
    }

    std::streampos size = fmt_stream.tellg();
    fmt_stream.seekg(0, std::ios::beg);

    while (fmt_stream.tellg() < size - std::streampos(52)) {
        uint32_t total_size;
        TrackPtr t(new Track);
        t->name = new char[16];

        fmt_stream.read(t->name, 16);
        fmt_stream.read((char*) &(t->intro_offset), 4);
        fmt_stream.seekg(4, std::ios::cur);
        fmt_stream.read((char*) &(t->intro_size), 4);
        fmt_stream.read((char*) &total_size, 4);
        fmt_stream.seekg(20, std::ios::cur);
        t->loop_offset = t->intro_offset + t->intro_size;
        t->loop_size = total_size - t->intro_size;

        track_list.push_back(t);
    }

    fmt_stream.close();
    return 0;
}

int extract_one(int index, std::ifstream& dat_stream, 
    TrackList& track_list, std::string path) {
    TrackPtr t = track_list[index]; 
    std::ofstream file((path + std::string(t->name)).c_str(), std::ios::binary | std::ios::out);
    
    if (!file.is_open()) {
        return -1;
    }

    unsigned char param[20] = {16, 0, 0, 0,
                              1, 0, 2, 0,
                              68, 172, 0, 0,
                              16, 177, 2, 0,
                              4, 0, 16, 0};

    file.write("RIFF\0\0\0\1WAVEfmt ", 16);
    file.write((char*) param, 20);
    file.write("data\0\0\0\0", 4);
    
    int16_t sample[CHUNK / 2];

    dat_stream.seekg(t->intro_offset, std::ios::beg);
    for (uint32_t i = 0; i < t->intro_size / CHUNK; i++) {
        dat_stream.read((char*) &sample, CHUNK);
        file.write((char*) &sample, CHUNK);
    }

    for (uint32_t i = t->intro_size / CHUNK * CHUNK / 2; i < t->intro_size / 2; i++) {
        dat_stream.read((char*) &sample, 2);
        file.write((char*) &sample, 2);
    }

    for (int i = 0; i < LOOP; i++) {
        dat_stream.seekg(t->loop_offset, std::ios::beg);
        for (uint32_t i = 0; i < t->loop_size / CHUNK; i++) {
            dat_stream.read((char*) &sample, CHUNK);
            file.write((char*) &sample, CHUNK);
        }

        for (uint32_t i = t->loop_size / CHUNK * CHUNK / 2; i < t->loop_size / 2; i++) {
            dat_stream.read((char*) &sample, 2);
            file.write((char*) &sample, 2);
        }
    }

    uint32_t fadeout_count = 0;
    double volume = 1.0;
    uint32_t fadeout_sample = FADEOUT * RATE * 2;
    uint32_t extend_sample = EXTEND * RATE * 2;

    dat_stream.seekg(t->loop_offset, std::ios::beg);
    for (uint32_t i = 0; i < extend_sample * 2 / CHUNK; i++) {
        dat_stream.read((char*) &sample, CHUNK);
        file.write((char*) &sample, CHUNK);
    }

    for (uint32_t i = extend_sample * 2 / CHUNK * CHUNK / 2; i < extend_sample; i++) {
        dat_stream.read((char*) &sample, 2);
        file.write((char*) &sample, 2);
    }

    for (uint32_t i = 0; i < fadeout_sample; i++) {
        dat_stream.read((char*) &sample, 2);
        sample[0] = (int16_t) round(sample[0] * volume);
        file.write((char*) &sample, 2);
        fadeout_count++;
        if (fadeout_count == fadeout_sample / 1000) {
            if (volume > 0.000) {
                volume -= 0.001;
            }
            fadeout_count = 0;
        }
    }

    file.close();
    file.clear();
    file.open((path + std::string(t->name)).c_str(), std::ios::binary | std::ios::out | std::ios::in);
    if (!file.is_open()) {
        return -2;
    }

    file.seekp(0x28, std::ios::beg);
    uint32_t data_size = t->intro_size + LOOP * t->loop_size + 
        extend_sample * 2 + fadeout_sample * 2;
    file.write((char*) &data_size, 4);

    file.seekp(0x04, std::ios::beg);
    uint32_t file_size = data_size + 36;
    file.write((char*) &file_size, 4);

    file.close();
    return 0;
}

int extract_all(std::ifstream& dat_stream, TrackList& track_list, std::string path) {
    int track_number = track_list.size();
    int err;
    
    for (int i = 0; i < track_number; i++) {
        std::cout << "Extracting " << track_list[i]->name 
            << "..." << std::endl;
        err = extract_one(i, dat_stream, track_list, path);
        if (err) {
            return -1;
        }
        std::cout << "Done.\n\n";
    }

    return 0;
}


