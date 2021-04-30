#include "shard.h"

#include <experimental/filesystem>
#include <fstream>

using namespace boost::archive;
namespace fs = std::experimental::filesystem;

namespace {

    std::string GetIndexFileName(int index) {
        return "index" + std::to_string(index) + ".txt";
    }

    Index LoadIndex(const std::string &file_name) {
        std::ifstream file{file_name};
        text_iarchive ia{file};
        Index index;
        ia >> index;
        return index;
    }

    void SaveIndex(const std::string &file_name, const Index &index) {
        std::ofstream file{file_name};
        text_oarchive oa{file};
        oa << index;
    }

}

void Shards::AddFile(const std::string &path, const Meta &meta) {
    if (files_shard.find(path) != files_shard.end()) {
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
        files_shard[path] = ind;
        std::string index_file_name = GetIndexFileName(ind);
        Index new_index;
        new_index.AddFile(path);
        SaveIndex(index_file_name, new_index);
    } else {
        (*it)++;
        int ind = it - files_cnt_on_shrads.begin();
        files_shard[path] = ind;
        std::string index_file_name = GetIndexFileName(ind);
        Index cur_index = LoadIndex(index_file_name);
        cur_index.AddFile(path);
        SaveIndex(index_file_name, cur_index);
    }
}

void Shards::DeleteFile(const std::string &path) {
    auto it = files_shard.find(path);
    if (it == files_shard.end()) {
//        no file in shards
        return;
    }
    int index = it->second;

    files_cnt_on_shrads[index]--;
    files_shard.erase(path);
    std::string index_file_name = GetIndexFileName(index);
    Index cur_index = LoadIndex(index_file_name);
    cur_index.DeleteFile(path);
    SaveIndex(index_file_name, cur_index);
}

void Shards::SaveShards() {
    std::ofstream file{"shards.txt"};
    text_oarchive oa{file};
    oa << *this;
}

void Shards::LoadShards() {
    std::ifstream file{"shards.txt"};
    text_iarchive ia{file};
    ia >> *this;
}

std::vector<Index> Shards::GetIndexData() {
    std::vector<Index> indexes;
    for (size_t i = 0; i < files_cnt_on_shrads.size(); ++i) {
        auto index_file_name = GetIndexFileName(i);
        auto index = LoadIndex(index_file_name);
        indexes.push_back(index);
    }
    return indexes;
}

void Shards::Clear() {
    for (size_t i = 0; i < files_cnt_on_shrads.size(); ++i) {
        auto index_file_name = GetIndexFileName(i);
        fs::remove(index_file_name);
    }
    files_cnt_on_shrads.clear();
    files_shard.clear();
    fs::remove("shards.txt");
}
