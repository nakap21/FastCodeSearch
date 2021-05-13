#include "shard.h"

#include <filesystem>
#include <fstream>

using namespace boost::archive;

namespace fs = std::filesystem;

void Shards::AddFile(int file_id, const Meta &meta) {
    if (files_shard.find(file_id) != files_shard.end()) {
//        already exist
        return;
    }

    auto it = std::find_if(files_cnt_on_shrads.begin(), files_cnt_on_shrads.end(),
                           [&meta](const auto &cur_cnt) {
                               return cur_cnt < meta.GetCntFilesInShard().value;
                           });
    if (it == files_cnt_on_shrads.end()) {
        files_cnt_on_shrads.push_back({1});
        int ind = files_cnt_on_shrads.size() - 1;
        files_shard[file_id] = ind;
        Index new_index;
        new_index.AddFile(file_id, meta);
        indexes.push_back(new_index);
    } else {
        (*it)++;
        int ind = it - files_cnt_on_shrads.begin();
        files_shard[file_id] = ind;
        indexes[ind].AddFile(file_id, meta);
    }
}

void Shards::DeleteFile(int file_id) {
    auto it = files_shard.find(file_id);
    if (it == files_shard.end()) {
//        no file in shards
        return;
    }
    int index = it->second;

    files_cnt_on_shrads[index]--;
    files_shard.erase(file_id);
    indexes[index].DeleteFile(file_id);
}

void Shards::SaveCntIndexes() {
    try {
        std::ofstream file{"cnt_indexes.bin"};
        binary_oarchive oa{file};
        oa << files_cnt_on_shrads.size();
    } catch (const std::exception &ex) {
        auto err = std::string(ex.what());
        throw std::runtime_error("Can't save cnt_indexes: " + err);
    }
}

size_t LoadCntIndexes(const std::string &file_path) {
    try {
        std::ifstream file{file_path};
        binary_iarchive ia{file};
        size_t cnt;
        ia >> cnt;
        return cnt;
    } catch (const std::exception &ex) {
        auto err = std::string(ex.what());
        throw std::runtime_error("Can't load file " + file_path + ": " + err);
    }
}

int LoadCapFilesCnt(const std::string &file_path) {
    try {
        std::ifstream file{file_path};
        binary_iarchive ia{file};
        int cnt;
        ia >> cnt;
        return cnt;
    } catch (const std::exception &ex) {
        auto err = std::string(ex.what());
        throw std::runtime_error("Can't load file " + file_path + ": " + err);
    }
}

void Shards::Clear() {
    for (size_t i = 0; i < files_cnt_on_shrads.size(); ++i) {
        auto index_file_name = GetIndexFileName(i);
        DeleteIndex(index_file_name);
    }
    files_cnt_on_shrads.clear();
    files_shard.clear();
    try {
        fs::remove("cnt_indexes.bin");
    } catch (const std::exception &ex) {
        auto err = std::string(ex.what());
        throw std::runtime_error("Can't remove file: " + err);
    }
}

void Shards::SaveShards() {
    SaveCntIndexes();
    SaveIndexes(indexes);
}
