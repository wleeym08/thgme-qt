#include "extractorWorker.h"
#include "extractor.h"
#include <fstream>
#include <string>

ExtractorWorker::ExtractorWorker(QString fmt_path, QString dat_path, QString out_path)
    : fmt_path(fmt_path), dat_path(dat_path), out_path(out_path) {}

ExtractorWorker::~ExtractorWorker() {}

void ExtractorWorker::process() {
    TrackList track_list;

    int err = load_info((this->fmt_path).toLocal8Bit().data(), track_list);
    if (err) {
        emit error("Error: Failed to open FMT file!");
        emit finished();
        return;
    }
    emit message("Loaded thbgm.fmt");

    std::ifstream dat_stream((this->dat_path).toLocal8Bit().data(),
                std::ios::binary | std::ios::in);
    if (!dat_stream.is_open()) {
        emit error("Error: Failed to open DAT file!");
        emit finished();
        return;
    }
    emit message("Loaded thbgm.dat");

    int size = track_list.size();
    for (int i = 0; i < size; i++) {
        emit message("Extracting " + QString(track_list[i]->name) +
                     "... (" + QString::number(i + 1) + "/" + QString::number(size) + ")");
        err = extract_one(i, dat_stream, track_list, (this->out_path).toStdString() + "/");
        if (err) {
            emit error("Error: Failed to extract files!");
            emit finished();
            return;
        }
    }

    for (auto i : track_list) {
        delete i->name;
    }

    emit message("All done.");
    emit finished();
}
